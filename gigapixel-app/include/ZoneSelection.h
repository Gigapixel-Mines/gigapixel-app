#ifndef ZONE_SELECTION
#define ZONE_SELECTION

#include <QLabel>
#include <QMenu>
#include <QPixmap>

class ZoneSelection : public QLabel
{
	Q_OBJECT

public:
	ZoneSelection(QWidget *parent = 0);
	~ZoneSelection();
	void setImageForSelection(QPixmap& image);

signals:
	void sendCoordsToWindow(QPoint, QPoint);
	void resetCoordsToWindow();

protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
private:

	bool selectionStarted;
	QRect selectionRect;
	QMenu contextMenu;
public slots:
	void setZone();
	void resetZone();
};

#endif // !ZONE_SELECTION
