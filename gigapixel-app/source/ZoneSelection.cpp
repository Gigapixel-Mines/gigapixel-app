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

	QAction *saveAction = contextMenu.addAction("Save");

	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveSlot()));
}

ZoneSelection::~ZoneSelection()
{

}

void ZoneSelection::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter painter(this);
	painter.setPen(QPen(QBrush(QColor(0, 0, 0, 180)), 1, Qt::DashLine));
	painter.setBrush(QBrush(QColor(255, 255, 255, 120)));
	painter.drawRect(selectionRect);
	//QPainter painter(this);
	//QLabel::paintEvent(e);
	//painter.setPen(QPen(QBrush(QColor(0, 0, 0, 180)), 1, Qt::DashLine));
	//painter.setBrush(QBrush(QColor(255, 255, 255, 120)));

	//painter.drawRect(selectionRect);
}

void ZoneSelection::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		if (selectionRect.contains(e->pos())) contextMenu.exec(this->mapToGlobal(e->pos()));
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

void ZoneSelection::saveSlot()
{
	QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
		"/home",
		QObject::tr("Images (*.jpg)"));
	this->pixmap()->copy(selectionRect).save(fileName);
}
