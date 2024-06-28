#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

class ImageWidget : public QWidget {
	Q_OBJECT
private:
	QImage image_;
public:
	explicit ImageWidget(QWidget *parent = nullptr);
	void setImage(QImage const &image)
	{
		image_ = image;
		update();
	}
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // IMAGEWIDGET_H
