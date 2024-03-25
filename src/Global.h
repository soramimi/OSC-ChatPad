#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

struct Global {
	QString organization_name;
	QString application_name;
	QString generic_config_dir;
	QString app_config_dir;
	QString config_file_path;
};

extern Global *global;

#endif // GLOBAL_H
