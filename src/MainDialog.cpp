#include "MainDialog.h"
#include "ui_MainDialog.h"
#include "jstream.h"
#include "osc.h"
#include "sock.h"
#include "webclient.h"
#include "MySettings.h"
#include <QClipboard>
#include <Windows.h>
#include <imm.h>
#include <QThread>
#include <QSettings>

struct MainDialog::Private {
	osc::Transmitter tx;
	WebContext webcx;
};

MainDialog::MainDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::MainDialog)
	, m(new Private)
{
	ui->setupUi(this);

	m->tx.open("127.0.0.1");

	ui->radioButton_ime_off->click();

	ui->comboBox_to->addItem("EN");
	ui->comboBox_to->addItem("JA");
	ui->comboBox_to->addItem("KO");
	ui->comboBox_to->addItem("ZH");

	ui->tabWidget->setCurrentWidget(ui->tab_text);
}

MainDialog::~MainDialog()
{
	setTypingIndicator(false);
	QThread::msleep(100);
	m->tx.close();
	delete m;
	delete ui;
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

void MainDialog::on_pushButton_paste_clicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();

	if (ui->tabWidget->currentWidget() == ui->tab_text) {
		ui->plainTextEdit->insertPlainText(text);
	} else if (ui->tabWidget->currentWidget() == ui->tab_translation) {
		ui->plainTextEdit_trans_from->setPlainText(text);
	}
}

void MainDialog::on_pushButton_clear_clicked()
{
	ui->plainTextEdit->clear();
	ui->plainTextEdit_trans_from->clear();
	ui->plainTextEdit_trans_to->clear();
}

void MainDialog::setTypingIndicator(bool f)
{
	m->tx.send_bool("/chatbox/typing", f);
}

void MainDialog::on_pushButton_send_clicked()
{
	QString text = ui->plainTextEdit->toPlainText();
	m->tx.send_chatbox("/chatbox/input", text.toStdString());
}

QString MainDialog::translate(QString const &text, QString const &target_lang)
{
	MySettings settings;
	QString apikey = settings.value("DeepL_API_Key").toString();
	if (apikey.isEmpty()) {
		return "DeepL API Key is not set.";
	}

	QString text2 = text;
	text2.replace("\"", "\\\"");

	std::string json = "{\"text\": [\"" + text2.toStdString() + "\"],\"target_lang\": \"" + target_lang.toStdString() + "\"}";

	QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	WebClient wc(&m->webcx);
	WebClient::Post post;
	post.content_type = "application/json";
	post.data.insert(post.data.end(), json.begin(), json.end());
	WebClient::Request req("https://api-free.deepl.com/v2/translate");
	req.add_header("Authorization: DeepL-Auth-Key " + apikey.toStdString());
	wc.post(req, &post);
	auto const &res = wc.response();

	std::string translated;

	jstream::Reader reader(res.content.data(), res.content.size());
	while (reader.next()) {
		if (reader.match("{translations[{text")) {
			translated += reader.string();
		}
	}

	QGuiApplication::restoreOverrideCursor();

	return QString::fromStdString(translated);
}

void MainDialog::on_pushButton_translate_clicked()
{
	QString text = ui->plainTextEdit_trans_from->toPlainText();
	QString target_lang = ui->comboBox_to->currentText();

	text = translate(text, target_lang);

	ui->plainTextEdit_trans_to->setPlainText(text);
}


void MainDialog::on_pushButton_swap_clicked()
{
	QString left = ui->plainTextEdit_trans_from->toPlainText();
	QString right = ui->plainTextEdit_trans_to->toPlainText();
	ui->plainTextEdit_trans_from->setPlainText(right);
	ui->plainTextEdit_trans_to->setPlainText(left);
}

void MainDialog::on_pushButton_insert_clicked()
{
	QString text = ui->plainTextEdit_trans_to->toPlainText();
	if (text.isEmpty()) return;

	ui->plainTextEdit->insertPlainText(text);
	ui->tabWidget->setCurrentWidget(ui->tab_text);
}

void MainDialog::on_tabWidget_currentChanged(int index)
{
	bool is_tab_text = (ui->tabWidget->currentWidget() == ui->tab_text);
	bool is_tab_trans = (ui->tabWidget->currentWidget() == ui->tab_translation);
	bool is_tab_settings = (ui->tabWidget->currentWidget() == ui->tab_settings);
	ui->pushButton_send->setEnabled(is_tab_text);

	if (is_tab_settings) {
		MySettings settings;
		ui->lineEdit_deepl_api_key->setText(settings.value("DeepL_API_Key").toString());
	}
}

void MainDialog::on_plainTextEdit_textChanged()
{
	bool on = !ui->plainTextEdit->toPlainText().isEmpty();
	setTypingIndicator(on);
}

void MainDialog::on_pushButton_settings_apply_clicked()
{
	MySettings settings;
	settings.setValue("DeepL_API_Key", ui->lineEdit_deepl_api_key->text());
}
