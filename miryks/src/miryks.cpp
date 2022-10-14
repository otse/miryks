#include <windows.h>
#include <winreg.h>

#include <miryks/miryks.hpp>

#include <miryks/model.h>
#include <miryks/player.h>

bool showCursor = false;

namespace miryks
{
	namespace hooks
	{
		bool (*hooks_some_behavior)(int) = 0;
		void (*load_interior)(const char *, bool) = 0;
		void (*load_world_space)(const char *, bool) = 0;
	}

	std::string installed_path;

	std::map<const char *, nif *> nis;

	interior *ginterior = nullptr;
	worldspace *gworldspace = nullptr;

	Player *player1 = nullptr;
}

LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
	strValue = strDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		strValue = szBuffer;
	}
	return nError;
}

std::string WStringToString(const std::wstring& wstr)
{
	std::string str;
	size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

void miryks::init_miryks()
{
	#define KEY L"SOFTWARE\\Wow6432Node\\Bethesda Softworks\\Skyrim Special Edition"

	HKEY hKey;
	LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KEY, 0, KEY_READ, &hKey);

	bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
	bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);

	// D:\Steam\steamapps\common\Skyrim Special Edition\

	std::wstring strValueOfBinDir;
	LONG error = GetStringRegKey(hKey, L"installed path", strValueOfBinDir, L"bad");
	
	if (error == ERROR_SUCCESS) {
		installed_path = WStringToString(strValueOfBinDir);
		printf("skyrim registry installed path is %s\n", installed_path.c_str());
	}
	else if (exists("editme.txt"))
	{
		printf("make sure editme.txt points to skyrim installation path\n");
		installed_path = fread(EDIT_ME);
		printf("install dir is `%s`\n", installed_path.c_str());
	}
	else
	{
		printf("can't find skyrim in registry, creating editme.txt\n");
		fwrite("editme.txt", "path to skyrim");
	}
}

void miryks::view_in_place(resource *res)
{
	static Model *model = nullptr;
	static GroupDrawer *groupDrawer = nullptr;
	if (model)
	{
		groupDrawer->parent->Remove(groupDrawer);
		delete model;
		delete groupDrawer;
	}
	model = new Model(res);
	groupDrawer = new GroupDrawer(
		model->baseGroup, translate(mat4(1.0), personCam->pos));
	sceneDef->bigGroup->Add(groupDrawer);
	showCursor = false;
	cameraCur = viewerCam;
	viewerCam->pos = groupDrawer->aabb.center();
	// viewerCam->pos = personCam->pos;
	viewerCam->radius = groupDrawer->aabb.radius2() * 2;
}