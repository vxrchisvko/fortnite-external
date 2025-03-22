#include <dependencies/kernel/driver.hpp>

interface_t driver;

bool interface_t::setup_driver()
{
	driver_handle = CreateFileA(("\\\\.\\\9fefd164-dd2c-45ed-82ea-efd759faf08c"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
		return false;

	return true;
}

uintptr_t interface_t::get_module()
{
	uintptr_t image_address = { NULL };
	_base_invoke arguments = { NULL };
	arguments.process_id = pid;
	arguments.address = (ULONGLONG*)&image_address;

	DeviceIoControl(driver_handle, code_get_base_address, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	return image_address;
}

bool interface_t::read_memory(PVOID address, PVOID buffer, DWORD size)
{
	read_write arguments = { 0 };
	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = pid;

	return DeviceIoControl(driver_handle, code_read_physical, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

bool interface_t::write_memory(PVOID address, PVOID buffer, DWORD size)
{
	read_write arguments = { 0 };

	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = pid;

	return DeviceIoControl(driver_handle, code_write, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

bool interface_t::fix_dtb()
{
	_dtb_invoke arguments = { NULL };
	arguments.process_id = pid;

	return DeviceIoControl(driver_handle, code_decrypt_cr3, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

void interface_t::move_mouse(int x, int y)
{
	s_move_mouse arguments{ NULL };

	arguments.X = (LONG)x;
	arguments.Y = (LONG)y;

	DeviceIoControl(driver_handle, code_mm, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

void interface_t::click_mouse()
{
	s_move_mouse arguments = { 0 };
	arguments.X = 0;
	arguments.Y = 0;
	arguments.ButtonFlags = MOUSE_LEFT_BUTTON_DOWN;
	DeviceIoControl(driver_handle, code_mm, &arguments, sizeof(arguments), &arguments, sizeof(arguments), 0, 0);
	arguments.ButtonFlags = MOUSE_LEFT_BUTTON_UP;
	DeviceIoControl(driver_handle, code_mm, &arguments, sizeof(arguments), &arguments, sizeof(arguments), 0, 0);
}

void interface_t::hide_overlay(HWND hwnd)
{
	wchar_t processName[MAX_PATH];
	DWORD processNameLength = GetModuleFileNameW(NULL, processName, MAX_PATH);

	if (processNameLength == 0) {
		return;
	}
	std::wstring processNameStr = processName;
	size_t pos = processNameStr.find_last_of(L"\\");
	if (pos != std::wstring::npos) {
		processNameStr = processNameStr.substr(pos + 1);
	}

	DWORD bytesReturned;
	BOOL result = DeviceIoControl(driver_handle, code_overlay,
		(LPVOID)processNameStr.c_str(),
		(DWORD)(processNameStr.length() * sizeof(wchar_t)),
		NULL, 0, &bytesReturned, NULL);

	if (result) {
		ShowWindow(hwnd, SW_SHOW);
	}
	else {}
}

bool interface_t::get_driver_handle() {
	HKEY hKey;
	LONG status;
	DWORD dwSize;
	DWORD dwType;
	wchar_t driverHandleBuffer[1024];

	status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\GenDrv", 0, KEY_READ, &hKey);
	if (status != ERROR_SUCCESS) {
		return false;
	}

	dwSize = sizeof(driverHandleBuffer);
	status = RegQueryValueExW(hKey, L"device", NULL, &dwType, (LPBYTE)driverHandleBuffer, &dwSize);
	if (status != ERROR_SUCCESS || dwType != REG_SZ) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	driver_handle = CreateFileW(driverHandleBuffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (driver_handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool interface_t::change_driver_handle(const std::wstring& new_driver_handle) {
	HKEY hKey;
	LONG status;

	status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\GenDrv", 0, KEY_SET_VALUE, &hKey);
	if (status != ERROR_SUCCESS) {
		return false;
	}

	status = RegSetValueExW(hKey, L"device", 0, REG_SZ, (const BYTE*)new_driver_handle.c_str(), (new_driver_handle.length() + 1) * sizeof(wchar_t));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	driver_handle = CreateFileW(new_driver_handle.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (driver_handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool interface_t::change_driver_handle_kernel(const std::wstring& new_driver_handle) {
	HANDLE hDevice = CreateFileW(L"\\\\.\\GenDrv", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD bytesReturned;
	BOOL success = DeviceIoControl(hDevice,
		code_change_handle,
		(LPVOID)new_driver_handle.c_str(),
		(new_driver_handle.length() + 1) * sizeof(wchar_t),
		NULL, 0,
		&bytesReturned,
		NULL);

	CloseHandle(hDevice);
	return success;
}

//usage xd
//std::wstring new_handle = L"\\\\.\\handle_namexd";
//if (km->change_driver_handle(newDriverHandle)) {
//    std::wcout << L"Driver handle changed successfully!" << std::endl;
//} else {
//    std::wcout << L"Failed to change driver handle." << std::endl;
//}


uintptr_t interface_t::get_pid(LPCTSTR process_name)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = (CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, process_name)) {
				(CloseHandle)(hsnap);
				pid = pt.th32ProcessID;
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	} (CloseHandle)(hsnap); return { NULL };
}

LPVOID interface_t::allocate_memory(HANDLE process, SIZE_T size)
{
	return VirtualAllocEx(process, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void interface_t::free_memory(HANDLE process, LPVOID address)
{
	VirtualFreeEx(process, address, 0, MEM_RELEASE);
}