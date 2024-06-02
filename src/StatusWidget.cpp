#include "StatusWidget.h"

#include <QPainter>

StatusWidget::StatusWidget(QWidget *parent)
	: QWidget{parent}
{
	pm_dots_.load(":/image/dots.png");
	pm_check_.load(":/image/check.png");
	connect(&timer_, &QTimer::timeout, this, &StatusWidget::onTimer);
	timer_.start(10);
}

void StatusWidget::setStatus(Status status)
{
	if (status_ != status) {
		status_ = status;

		elapsed_.invalidate();
		elapsed_.start();
	}
	update();
}

void StatusWidget::paintEvent(QPaintEvent *event)
{
	if (status_ == Normal) return;

	QPainter pr(this);

	QPixmap *pm = nullptr;
	if (status_ == Typing) {
		pm = &pm_dots_;
	} else if (status_ == Sent) {
		pm = &pm_check_;
	}

	if (pm) {
		if (progress_ < 0.25) {
			int x = 0;
			int y = 0;
			int w = width() * progress_ * 4;
			int h = height();
			pr.setClipRect(x, y, w, h);
		} else if (progress_ > 0.75) {
			int x = width() * (progress_ - 0.75) * 4;
			int y = 0;
			int w = width() - x;
			int h = height();
			pr.setClipRect(x, y, w, h);
		}
		auto pm2 = pm->scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		pr.drawPixmap(0, 0, pm2);
	}
}

void StatusWidget::onTimer()
{
	if (status_ == Typing || status_ == Sent) {
		progress_ = elapsed_.elapsed() / 2000.0f;
		if (progress_ >= 1) {
			progress_ = 0;
			status_ = Normal;
		}
		update();
	}
}
