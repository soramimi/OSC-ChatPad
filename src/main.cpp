
#include "MainDialog.h"
#include "Global.h"
#include "joinpath.h"
#include "sock.h"
#include <QApplication>
#include <QStandardPaths>
#include <Windows.h>
#include <thread>

HANDLE hMutex;

Global *global;

int main(int argc, char **argv)
{
	wchar_t const *name = L"OSC Chat Assistant";
	hMutex = CreateMutex(NULL, FALSE, name);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		HWND hwnd = FindWindowW(NULL, name);
		if (hwnd) {
			ShowWindow(hwnd, SW_RESTORE);
			SetForegroundWindow(hwnd);
		}
		return 0;
	}

	sock::startup();

	Global g;
	global = &g;
	global->organization_name = "soramimi.jp";
	global->application_name = "OSC-ChatAssistant";
	global->generic_config_dir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	global->app_config_dir = global->generic_config_dir / global->organization_name / global->application_name;
	global->config_file_path = joinpath(global->app_config_dir, global->application_name + ".ini");

	QApplication a(argc, argv);

	MainDialog w;
	w.setWindowTitle(QString::fromUtf16((ushort const *)name));
	w.exec();

	sock::cleanup();
	return 0;
}
