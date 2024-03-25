#include "MySettings.h"
#include "Global.h"

MySettings::MySettings(QObject *)
	: QSettings(global->config_file_path, QSettings::IniFormat)
{
}
