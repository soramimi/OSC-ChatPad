#ifndef WINDOWSSHORTCUTFILE_H
#define WINDOWSSHORTCUTFILE_H

#include <string>

struct Win32ShortcutData {
	std::wstring lnkpath;
	std::wstring targetpath;
	std::wstring arguments;
	std::wstring workingdir;
	std::wstring iconpath;
	int iconindex = 0;
	bool runas = false;
};

class WindowsShortcutFile {
public:
	static void createWin32Shortcut(const Win32ShortcutData &data);
};

#endif // WINDOWSSHORTCUTFILE_H
