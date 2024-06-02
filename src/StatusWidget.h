#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>

class StatusWidget : public QWidget {
	Q_OBJECT
public:
	enum Status {
		Normal,
		Typing,
		Sent,
	};
private:
	Status status_ = Normal;
	float progress_ = 0;
	QTimer timer_;
	QElapsedTimer elapsed_;
	QPixmap pm_dots_;
	QPixmap pm_check_;
protected:
	void paintEvent(QPaintEvent *event);
public:
	explicit StatusWidget(QWidget *parent = nullptr);

	void setStatus(Status status);

private slots:
	void onTimer();
};

#endif // STATUSWIDGET_H
