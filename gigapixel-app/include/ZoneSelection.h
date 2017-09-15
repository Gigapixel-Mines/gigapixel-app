#ifndef ZONE_SELECTION
#define ZONE_SELECTION

#include <QLabel>
#include <QMenu>

class ZoneSelection : public QLabel
{
	Q_OBJECT

public:
	ZoneSelection(QWidget *parent = 0);
	~ZoneSelection();
protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
private:

	bool selectionStarted;
	QRect selectionRect;
	QMenu contextMenu;
	private slots:
	void saveSlot();

};

#endif // !ZONE_SELECTION
