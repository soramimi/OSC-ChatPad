#include <ShlObj.h>
#include "WindowsShortcutFile.h"

std::wstring replace_slash_to_backslash(std::wstring const &str)
{
	std::wstring sb;
	for (size_t i = 0; i < str.size(); i++) {
		wchar_t c = str[i];
		if (c == '/') {
			c = '\\';
		}
		sb += c;
	}
	return sb;
}

void WindowsShortcutFile::createWin32Shortcut(Win32ShortcutData const &data)
{
	Win32ShortcutData d = data;

	d.lnkpath = replace_slash_to_backslash(d.lnkpath);

	HRESULT hr;
	IShellLink *shlink = 0;
	IShellLinkDataList *psldl = 0;
	IPersistFile *pfile = 0;

	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&shlink);
	if (SUCCEEDED(hr)) {
		shlink->SetPath(d.targetpath.c_str());
		if (!d.arguments.empty()) {
			shlink->SetArguments(d.arguments.c_str());
		}
		if (!d.workingdir.empty()) {
			shlink->SetWorkingDirectory(d.workingdir.c_str());
		}
		if (!d.iconpath.empty()) {
			shlink->SetIconLocation(d.iconpath.c_str(), d.iconindex);
		}

		if (d.runas) {
			hr = shlink->QueryInterface(IID_IShellLinkDataList, (void **)&psldl);
			if (SUCCEEDED(hr)) {
				psldl->SetFlags(SLDF_RUNAS_USER);
			}
		}

		hr = shlink->QueryInterface(IID_IPersistFile, (void**)&pfile);
		if (SUCCEEDED(hr)) {
			hr = pfile->Save(d.lnkpath.c_str(), TRUE);
			pfile->Release();
		}

		shlink->Release();
	}
}

