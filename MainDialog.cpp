#include "MainDialog.h"
#include "ui_MainDialog.h"
#include "osc.h"
#include "sock.h"
#include <Windows.h>
#include <imm.h>

struct MainDialog::Private {
	osc::Transmitter tx;
};

MainDialog::MainDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::MainDialog)
	, m(new Private)
{
	ui->setupUi(this);

	m->tx.open("127.0.0.1");

	ui->radioButton_ime_off->click();
}

MainDialog::~MainDialog()
{
	m->tx.close();
	delete m;
	delete ui;
}

void MainDialog::on_pushButton_clear_clicked()
{
	ui->plainTextEdit->clear();
}

void MainDialog::on_pushButton_send_clicked()
{
	QString text = ui->plainTextEdit->toPlainText();
	m->tx.send_chatbox("/chatbox/input", text.toStdString());
}

void MainDialog::setImeState(bool on)
{
	HWND hwnd = (HWND)this->winId();
	HIMC himc = ImmGetContext(hwnd);
	if (himc) {
		if (on) {
			ImmSetOpenStatus(himc, true);
		} else {
			ImmSetOpenStatus(himc, false);
		}

		ImmReleaseContext(hwnd, himc);
	}
}

void MainDialog::on_radioButton_ime_off_clicked()
{
	setImeState(false);
}

void MainDialog::on_radioButton_ime_on_clicked()
{
	setImeState(true);
}

