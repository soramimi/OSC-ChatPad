#define NOMINMAX
#include "MainDialog.h"
#include "ui_MainDialog.h"
#include "Global.h"
#include "MySettings.h"
#include "StatusWidget.h"
#include "WindowsShortcutFile.h"
#include "joinpath.h"
#include "jstream.h"
#include "osc.h"
#include "sock.h"
#include "webclient.h"
#include <QClipboard>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>
#include <Windows.h>
#include <imm.h>

#define APP_VERSION "v2.0.1"
#define COPYRIGHT_YEAR "2024"

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

	ui->label_version->setText(APP_VERSION);
	ui->label_copyright->setText(QString("Copyright (C) %1 Shinichi Fuchita").arg(COPYRIGHT_YEAR));

	setWindowIcon(QIcon(":/image/OSC-ChatboxAssistant.svg"));
	ui->widget_app_icon->setImage(QImage{":/image/OSC-ChatboxAssistant.svg"});
	ui->widget_creator_icon->setImage(QImage{":/image/soramimi.jpg"});

	layout()->setContentsMargins(2, 2, 2, 2);
	layout()->setSpacing(0);

	setupNetwork();

	ui->radioButton_ime_off->click();

	ui->radioButton_en->click();

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

void MainDialog::setupNetwork()
{
	MySettings settings;
	QString host = settings.value("OSC_Receiver_Address").toString();
	int port = settings.value("OSC_Receiver_Port").toInt();
	if (host.isEmpty()) {
		host = "localhost";
	}
	if (port < 1 || port > 65535) {
		port = 9000;
	}
	ui->lineEdit_osc_address->setText(host);
	ui->lineEdit_osc_port->setText(QString::number(port));
	m->tx.open(host.toStdString().c_str(), port);
}

void MainDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) {
		// inhibit closing by ESC key
		event->ignore();
		return;
	}
	QDialog::keyPressEvent(event);
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

void MainDialog::on_pushButton_clear_clicked()
{
	bool is_tab_text = (ui->tabWidget->currentWidget() == ui->tab_text);
	bool is_tab_trans = (ui->tabWidget->currentWidget() == ui->tab_translation);
	if (is_tab_text) {
		ui->plainTextEdit->clear();
	}
	if (is_tab_trans) {
		ui->plainTextEdit_trans_from->clear();
		ui->plainTextEdit_trans_to->clear();
	}
}

void MainDialog::setTypingIndicator(bool f)
{
	m->tx.send_bool("/chatbox/typing", f);

	ui->widget_status->setStatus(f ? StatusWidget::Typing : StatusWidget::Normal);
}

void MainDialog::on_pushButton_send_clicked()
{
	QString text = ui->plainTextEdit->toPlainText();
	m->tx.send_chatbox("/chatbox/input", text.toStdString());

	ui->widget_status->setStatus(StatusWidget::Sent);
}

QString MainDialog::translate(QString const &text, QString const &target_lang)
{
	MySettings settings;
	QString apikey = settings.value("DeepL_API_Key").toString();
	if (apikey.isEmpty()) {
		return "DeepL API Key is not set.";
	}

	std::string json = "{\"text\": [\"";

	std::string utf8 = text.toStdString();
	for (auto c : utf8) {
		if (c == '\n') {
			json += "\\n";
		} else if (c == '\r') {
			// nop // json += "\\r";
		} else if (c == '\t') {
			json += "\\t";
		} else if (c == '\\') {
			json += "\\\\";
		} else if (c == '\"') {
			json += "\\\"";
		} else {
			json += c;
		}
	}

	json += "\"],\"target_lang\": \"" + target_lang.toStdString() + "\"}";

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

QString MainDialog::languageID() const
{
	if (ui->radioButton_en->isChecked()) return "EN";
	if (ui->radioButton_ja->isChecked()) return "JA";
	if (ui->radioButton_ko->isChecked()) return "KO";
	if (ui->radioButton_zh->isChecked()) return "ZH";
	return "EN";
}

void MainDialog::on_pushButton_translate_clicked()
{
	QString text = ui->plainTextEdit_trans_from->toPlainText();
	QString lang = languageID();

	text = translate(text, lang);

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
	ui->pushButton_clear->setEnabled(is_tab_text || is_tab_trans);
	ui->pushButton_paste->setEnabled(is_tab_text || is_tab_trans);
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

void MainDialog::on_pushButton_osc_reset_clicked()
{
	ui->lineEdit_osc_address->setText("localhost");
	ui->lineEdit_osc_port->setText("9000");
}

void MainDialog::on_pushButton_settings_save_clicked()
{
	MySettings settings;
	settings.setValue("OSC_Receiver_Address", ui->lineEdit_osc_address->text());
	settings.setValue("OSC_Receiver_Port", ui->lineEdit_osc_port->text().toInt());
	settings.setValue("DeepL_API_Key", ui->lineEdit_deepl_api_key->text());
	setupNetwork();
}


void MainDialog::on_pushButton_cut_clicked()
{
	if (ui->tabWidget->currentWidget() == ui->tab_text) {
		ui->plainTextEdit->cut();
	} else if (ui->tabWidget->currentWidget() == ui->tab_translation) {
		ui->plainTextEdit_trans_from->cut();
	}
}

void MainDialog::on_pushButton_copy_clicked()
{
	if (ui->tabWidget->currentWidget() == ui->tab_text) {
		ui->plainTextEdit->copy();
	} else if (ui->tabWidget->currentWidget() == ui->tab_translation) {
		ui->plainTextEdit_trans_from->copy();
	}
}

void MainDialog::on_pushButton_paste_clicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();

	if (ui->tabWidget->currentWidget() == ui->tab_text) {
		ui->plainTextEdit->insertPlainText(text);
	} else if (ui->tabWidget->currentWidget() == ui->tab_translation) {
		ui->plainTextEdit_trans_from->insertPlainText(text);
	}
}

void MainDialog::on_pushButton_select_all_clicked()
{
	if (ui->tabWidget->currentWidget() == ui->tab_text) {
		ui->plainTextEdit->selectAll();
	} else if (ui->tabWidget->currentWidget() == ui->tab_translation) {
		ui->plainTextEdit_trans_from->selectAll();
	}
}



void MainDialog::on_pushButton_save_shortcut_file_clicked()
{
	QString desktop_dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QString target_path = QApplication::applicationFilePath();

	Win32ShortcutData data;
	data.targetpath = (wchar_t const *)target_path.utf16();

	QString home = QDir::home().absolutePath();
	QString shortcut_dir = desktop_dir;
	QString name = global->application_name;
	QString iconpath = target_path;
	QString shortcut_path = shortcut_dir / name + ".lnk";
	QString lnkpath = QFileDialog::getSaveFileName(this, tr("Save Shortcut File"), shortcut_path, "Shortcut files (*.lnk)");
	data.iconpath = (wchar_t const *)iconpath.utf16();
	data.lnkpath = (wchar_t const *)lnkpath.utf16();

	if (!shortcut_path.isEmpty()) {
		WindowsShortcutFile::createWin32Shortcut(data);
	}

}

