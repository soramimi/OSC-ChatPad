#include "ImageWidget.h"

#include <QPainter>

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget{parent}
{
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
	int w = width();
	int h = height();
	int n = std::min(w, h);
	int x = (width() - n) / 2;
	int y = (height() - n) / 2;
	QPainter painter{this};
	QImage img = image_.scaled(n, n, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	painter.drawImage(QRect(x, y, n, n), img, img.rect());
}
