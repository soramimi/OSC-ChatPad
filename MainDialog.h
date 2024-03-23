#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog {
	Q_OBJECT
private:
	struct Private;
	Private *m;


public:
	explicit MainDialog(QWidget *parent = nullptr);
	~MainDialog();

private slots:
	void on_pushButton_clear_clicked();
	void on_pushButton_send_clicked();
	void on_radioButton_ime_off_clicked();
	void on_radioButton_ime_on_clicked();

private:
	Ui::MainDialog *ui;
	void setImeState(bool on);
};

#endif // MAINDIALOG_H
