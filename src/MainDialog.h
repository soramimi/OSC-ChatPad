#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog {
	Q_OBJECT
private:
	Ui::MainDialog *ui;
	struct Private;
	Private *m;

	void setImeState(bool on);
	QString translate(const QString &text, const QString &target_lang);
	void setTypingIndicator(bool f);
	void setupNetwork();
protected:
	void keyPressEvent(QKeyEvent *event);
public:
	explicit MainDialog(QWidget *parent = nullptr);
	~MainDialog();
private slots:
	void on_plainTextEdit_textChanged();
	void on_pushButton_clear_clicked();
	void on_pushButton_insert_clicked();
	void on_pushButton_osc_reset_clicked();
	void on_pushButton_paste_clicked();
	void on_pushButton_send_clicked();
	void on_pushButton_settings_save_clicked();
	void on_pushButton_swap_clicked();
	void on_pushButton_translate_clicked();
	void on_radioButton_ime_off_clicked();
	void on_radioButton_ime_on_clicked();
	void on_tabWidget_currentChanged(int index);
};

#endif // MAINDIALOG_H
