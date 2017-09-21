#include "ZoneSelection.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QAction>
#include <QFileDialog>

ZoneSelection::ZoneSelection(QWidget *parent)
	: QLabel(parent)
{
	selectionStarted = false;

	QAction *setAction = contextMenu.addAction("Sélectionner cette zone");
	QAction *resetAction = contextMenu.addAction("Réinitialiser la zone de sélection");

	connect(setAction, SIGNAL(triggered()), this, SLOT(setZone()));
	connect(resetAction, SIGNAL(triggered()), this, SLOT(resetZone()));
}

ZoneSelection::~ZoneSelection()
{

}

void ZoneSelection::setImageForSelection(QPixmap& image)
{
	if (!image.isNull())
	{
		this->setPixmap(image);
		this->setFixedSize(image.size());
	}
}

void ZoneSelection::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter painter(this);
	painter.setPen(QPen(QBrush(QColor(0, 0, 0, 180)), 1, Qt::DashLine));
	painter.setBrush(QBrush(QColor(255, 255, 255, 120)));
	painter.drawRect(selectionRect);
}

void ZoneSelection::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		contextMenu.exec(this->mapToGlobal(e->pos()));
	}
	else
	{
		selectionStarted = true;
		selectionRect.setTopLeft(e->pos());
		selectionRect.setBottomRight(e->pos());
	}

}

void ZoneSelection::mouseMoveEvent(QMouseEvent *e)
{
	if (selectionStarted)
	{
		selectionRect.setBottomRight(e->pos());
		repaint();
	}
}

void ZoneSelection::mouseReleaseEvent(QMouseEvent *e)
{
	selectionStarted = false;
}

void ZoneSelection::setZone()
{
	//Send the coordinates to fenetre for mapping
	QPoint tempBotLeft = selectionRect.bottomLeft();
	QPoint tempTopRight = selectionRect.topRight();
	//Correct with reference
	tempBotLeft.setX(tempBotLeft.x() - PX_H_REF_WCAM);
	tempBotLeft.setY(tempBotLeft.y() - PX_V_REF_WCAM);
	tempTopRight.setX(tempTopRight.x() - PX_H_REF_WCAM);
	tempTopRight.setY(tempTopRight.y() - PX_V_REF_WCAM);

	//Changer l'axe vertical pour adapter l'orientation
	//tempBotLeft.setY(-tempBotLeft.y() + this->height());
	//tempTopRight.setY(-tempTopRight.y() + this->height());
	emit sendCoordsToWindow(tempTopRight, tempBotLeft); 
}

void ZoneSelection::resetZone()
{
	//Reset coordinates too and send the refresh to fenetre
	selectionRect.setTopLeft(QPoint(-10, -10));
	selectionRect.setBottomRight(QPoint(-10, -10));
	repaint();
	emit resetCoordsToWindow();
}
