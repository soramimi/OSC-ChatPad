#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QSettings>

class MySettings : public QSettings {
public:
	explicit MySettings(QObject *parent = nullptr);
};

#endif // MYSETTINGS_H
