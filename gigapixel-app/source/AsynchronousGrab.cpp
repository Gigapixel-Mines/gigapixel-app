/*=============================================================================
Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

Redistribution of this file, in original or modified form, without
prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

File:        AsynchronousGrab.cpp

Description: Qt dialog class for the GUI of the AsynchronousGrab example of
VimbaCPP.

-------------------------------------------------------------------------------

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/


#include "AsynchronousGrab.h"
#include <VimbaImageTransform\Include\VmbTransform.h>

#include <QFile>
#include <QTextStream>

#include <sstream>

#define NUM_COLORS 3
#define BIT_DEPTH 8

using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

AsynchronousGrab::AsynchronousGrab(QWidget *parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, m_bIsStreaming(false)
{
	// Start Vimba
	VmbErrorType err = m_ApiController.StartUp();
	Log("Starting Vimba", err);

	if (VmbErrorSuccess == err)
	{
		// Connect new camera found event with event handler
		QObject::connect(m_ApiController.GetCameraObserver(), SIGNAL(CameraListChangedSignal(int)), this, SLOT(OnCameraListChanged(int)));

		// Initially get all connected cameras
		UpdateCameraListBox();
		std::stringstream strMsg;
		strMsg << "Cameras found..." << m_cameras.size();
		Log(strMsg.str());
	}
}

AsynchronousGrab::~AsynchronousGrab()
{
	// if we are streaming stop streaming
	if (true == m_bIsStreaming)
		OnBnClickedButtonStartstop();

	// Before we close the application we stop Vimba
	m_ApiController.ShutDown();
}

void AsynchronousGrab::OnBnClickedButtonStartstop()
{
	VmbErrorType err;
	//int nRow = ui.m_ListBoxCameras->currentRow();
	Log("OnBnClickedButtonStartstop called");
	if (m_cameras.size() > 0)
	{
		if (false == m_bIsStreaming)
		{
			// Start acquisition
			err = m_ApiController.StartContinuousImageAcquisition(m_cameras[0]);
			// Set up Qt image
			if (VmbErrorSuccess == err)
			{
				Log("Success");
				m_Image = new QImage(m_ApiController.GetWidth(),
					m_ApiController.GetHeight(),
					QImage::Format_RGB888);

				QObject::connect(m_ApiController.GetFrameObserver(), SIGNAL(FrameReceivedSignal(int)), this, SLOT(OnFrameReady(int)));
			}
			Log("Starting Acquisition", err);
			m_bIsStreaming = VmbErrorSuccess == err;
		}
		else
		{
			m_bIsStreaming = false;
			// Stop acquisition
			err = m_ApiController.StopContinuousImageAcquisition();
			// Clear all frames that we have not picked up so far
			m_ApiController.ClearFrameQueue();
			m_Image = new QImage();
			Log("Stopping Acquisition", err);
		}

		if (false == m_bIsStreaming)
		{
			//ui.m_ButtonStartStop->setText(QString("Start Image Acquisition"));
		}
		else
		{
			//ui.m_ButtonStartStop->setText(QString("Stop Image Acquisition"));
		}
	}
}

//
// This event handler (Qt slot) is triggered through a Qt signal posted by the frame observer
//
// Parameters:
//  [in]    status          The frame receive status (complete, incomplete, ...)
//
void AsynchronousGrab::OnFrameReady(int status)
{
	Log("OnFrameReady called");
	if (true == m_bIsStreaming)
	{
		// Pick up frame
		FramePtr pFrame = m_ApiController.GetFrame();
		if (SP_ISNULL(pFrame))
		{
			Log("frame pointer is NULL, late frame ready message");
			return;
		}
		// See if it is not corrupt
		if (VmbFrameStatusComplete == status)
		{
			Log("1");
			VmbUchar_t * pBuffer;
			VmbErrorType err = SP_ACCESS(pFrame)->GetImage(pBuffer);
			if (VmbErrorSuccess == err)
			{
				Log("2");
				VmbUint32_t nSize;
				err = SP_ACCESS(pFrame)->GetImageSize(nSize);
				if (VmbErrorSuccess == err)
				{
					VmbUint32_t nWidth = 0;
					err = pFrame->GetWidth(nWidth);
					if (VmbErrorSuccess == err)
					{
						VmbUint32_t nHeight = 0;
						err = pFrame->GetHeight(nHeight);
						if (VmbErrorSuccess == err)
						{
							Log("3");
							VmbPixelFormatType ePixelFormat = m_ApiController.GetPixelFormat();
							if (!(*m_Image).isNull())
							{
								CopyToImage(pBuffer, ePixelFormat, *m_Image);
								emit ImageReceivedSignal(m_Image);
								Log("6 : signal emis");
							}
						}
					}
				}
			}
		}
		else
		{
			// If we receive an incomplete image we do nothing but logging
			Log("Failure in receiving image", VmbErrorOther);
		}

		// And queue it to continue streaming
		m_ApiController.QueueFrame(pFrame);
	}
}

//
// This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
//
// Parameters:
//  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
//
void AsynchronousGrab::OnCameraListChanged(int reason)
{
	bool bUpdateList = false;

	// We only react on new cameras being found and known cameras being unplugged
	if (AVT::VmbAPI::UpdateTriggerPluggedIn == reason)
	{
		Log("Camera list changed. A new camera was discovered by Vimba.");
		bUpdateList = true;
	}
	else if (AVT::VmbAPI::UpdateTriggerPluggedOut == reason)
	{
		Log("Camera list changed. A camera was disconnected from Vimba.");
		if (true == m_bIsStreaming)
		{
			OnBnClickedButtonStartstop();
		}
		bUpdateList = true;
	}

	if (true == bUpdateList)
	{
		UpdateCameraListBox();
	}

	//ui.m_ButtonStartStop->setEnabled(0 < m_cameras.size() || m_bIsStreaming);
}

//
// Copies the content of a byte buffer to a Qt image with respect to the image's alignment
//
// Parameters:
//  [in]    pInbuffer       The byte buffer as received from the cam
//  [in]    ePixelFormat    The pixel format of the frame
//  [out]   OutImage        The filled Qt image
//
VmbErrorType AsynchronousGrab::CopyToImage(VmbUchar_t *pInBuffer, VmbPixelFormat_t ePixelFormat, QImage &pOutImage, const float *Matrix //= NULL
                                           )
{
	const int           nHeight = m_ApiController.GetHeight();
	const int           nWidth = m_ApiController.GetWidth();

	VmbImage            SourceImage, DestImage;
	VmbError_t          Result;
	SourceImage.Size = sizeof(SourceImage);
	DestImage.Size = sizeof(DestImage);

	Result = VmbSetImageInfoFromPixelFormat(ePixelFormat, nWidth, nHeight, &SourceImage);
	if (VmbErrorSuccess != Result)
	{
		Log("Could not set source image info", static_cast<VmbErrorType>(Result));
		return static_cast<VmbErrorType>(Result);
	}
	QString             OutputFormat;
	const int           bytes_per_line = pOutImage.bytesPerLine();
	switch (pOutImage.format())
	{
	default:
		Log("unknown output format", VmbErrorBadParameter);
		return VmbErrorBadParameter;
	case QImage::Format_RGB888:
		if (nWidth * 3 != bytes_per_line)
		{
			Log("image transform does not support stride", VmbErrorWrongType);
			return VmbErrorWrongType;
		}
		OutputFormat = "RGB24";
		break;
	}
	Result = VmbSetImageInfoFromString(OutputFormat.toStdString().c_str(), OutputFormat.length(), nWidth, nHeight, &DestImage);
	if (VmbErrorSuccess != Result)
	{
		Log("could not set output image info", static_cast<VmbErrorType>(Result));
		return static_cast<VmbErrorType>(Result);
	}
	SourceImage.Data = pInBuffer;
	DestImage.Data = pOutImage.bits();
	// do color processing?
	if (NULL != Matrix)
	{
		VmbTransformInfo TransformParameter;
		Result = VmbSetColorCorrectionMatrix3x3(Matrix, &TransformParameter);
		if (VmbErrorSuccess == Result)
		{
			Result = VmbImageTransform(&SourceImage, &DestImage, &TransformParameter, 1);
		}
		else
		{
			Log("could not set matrix t o transform info ", static_cast<VmbErrorType>(Result));
			return static_cast<VmbErrorType>(Result);
		}
	}
	else
	{
		Result = VmbImageTransform(&SourceImage, &DestImage, NULL, 0);
	}
	if (VmbErrorSuccess != Result)
	{
		Log("could not transform image", static_cast<VmbErrorType>(Result));
		return static_cast<VmbErrorType>(Result);
	}
	return static_cast<VmbErrorType>(Result);
}

//
// Queries and lists all known camera
//
void AsynchronousGrab::UpdateCameraListBox()
{
	// Get all cameras currently connected to Vimba
	CameraPtrVector cameras = m_ApiController.GetCameraList();

	// Simply forget about all cameras known so far
	//ui.m_ListBoxCameras->clear();
	m_cameras.clear();

	// And query the camera details again
	for (CameraPtrVector::const_iterator iter = cameras.begin();
		cameras.end() != iter;
		++iter)
	{
		std::string strCameraName;
		std::string strCameraID;
		if (VmbErrorSuccess != (*iter)->GetName(strCameraName))
		{
			strCameraName = "[NoName]";
		}
		// If for any reason we cannot get the ID of a camera we skip it
		if (VmbErrorSuccess == (*iter)->GetID(strCameraID))
		{
			//ui.m_ListBoxCameras->addItem(QString::fromStdString(strCameraName + " " + strCameraID));
			m_cameras.push_back(strCameraID);
		}
	}

	//ui.m_ButtonStartStop->setEnabled(0 < m_cameras.size() || m_bIsStreaming);
}

//
// Prints out a given logging string, error code and the descriptive representation of that error code
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//  [in]    eErr            The API status code
//
void AsynchronousGrab::Log(std::string strMsg, VmbErrorType eErr)
{
	//strMsg += "..." + m_ApiController.ErrorCodeToMessage(eErr);
	//ui.m_ListLog->insertItem(0, QString::fromStdString(strMsg));
	QString filename = "Data.txt";
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		QTextStream stream(&file);
		stream << QString::fromUtf8(strMsg.c_str()) << endl;
	}
}

//
// Prints out a given logging string
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//
void AsynchronousGrab::Log(std::string strMsg)
{
    // UNCOMMENT TO DEBUG
	//ui.m_ListLog->insertItem(0, QString::fromStdString(strMsg));
    //QString filename = "C:/Data.txt";
    //QFile file(filename);
    //if (file.open(QIODevice::ReadWrite | QIODevice::Append))
    //{
    //	QTextStream stream(&file);
    //	stream << QString::fromUtf8(strMsg.c_str()) << endl;
    //}
}


