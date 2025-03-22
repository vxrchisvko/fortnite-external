#pragma once
#include <Windows.h>
#include <cstdint>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <cstring>

#define code_read_physical CTL_CODE(FILE_DEVICE_UNKNOWN, 0x551, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_write CTL_CODE(FILE_DEVICE_UNKNOWN, 0x552, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_get_base_address CTL_CODE(FILE_DEVICE_UNKNOWN, 0x553, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_decrypt_cr3 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x554, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_mm CTL_CODE(FILE_DEVICE_UNKNOWN, 0x772, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_overlay CTL_CODE(FILE_DEVICE_UNKNOWN, 0x556, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define code_change_handle CTL_CODE(FILE_DEVICE_UNKNOWN, 0x558, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define code_get_handle CTL_CODE(FILE_DEVICE_UNKNOWN, 0x559, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define MOUSE_LEFT_BUTTON_DOWN   0x0001
#define MOUSE_LEFT_BUTTON_UP     0x0002

typedef struct read_write
{
	int32_t process_id;
	uint64_t address;
	uint64_t buffer;
	uint64_t size;
}rw, * prw;
typedef struct _base_invoke {
	int32_t process_id;
	uint64_t* address;
} base_invoke, * pbase_invoke;

typedef struct _dtb_invoke {
	INT32 process_id;
} dtb_invoke, * pdtb_invoke;

typedef struct s_move_mouse
{
	LONG X;
	LONG Y;
	USHORT ButtonFlags;
} move_mouse, * p_move_mouse;

typedef struct _TargetProcessNameStruct
{
	WCHAR process_name[260];
} TargetProcessNameStruct, * PTargetProcessNameStruct;

class interface_t {
public:
	HANDLE driver_handle;
	INT32 pid;
	uintptr_t module_base;

	bool setup_driver();
	bool read_memory(PVOID address, PVOID buffer, DWORD size);
	bool write_memory(PVOID address, PVOID buffer, DWORD size);
	void move_mouse(int x, int y);
	void click_mouse();
	void hide_overlay(HWND hwnd);
	bool get_driver_handle();
	bool change_driver_handle(const std::wstring& new_driver_handle);
	bool change_driver_handle_kernel(const std::wstring& new_driver_handle);
	LPVOID allocate_memory(HANDLE process, SIZE_T size);
	void free_memory(HANDLE process, LPVOID address);

	uintptr_t get_module();
	uintptr_t get_pid(LPCTSTR process_name);

	bool fix_dtb();

	template <typename T>
	T read(uint64_t address) {
		T buffer{};
		read_memory((PVOID)address, &buffer, sizeof(T));
		return buffer;
	}

	template <typename T>
	T write(uint64_t address, T buffer) {
		write_memory((PVOID)address, &buffer, sizeof(T));
		return buffer;
	}

	template <typename T>
	bool read_array(uintptr_t address, T* out, size_t len) {
		if (!out || len == 0) return false;

		size_t byteSize = len * sizeof(T);
		std::vector<uint8_t> buffer(byteSize);

		if (!read_memory(reinterpret_cast<PVOID>(address), buffer.data(), byteSize)) {
			return false;
		}

		std::memcpy(out, buffer.data(), byteSize);
		return true;
	}

	template <typename T>
	bool read_array2(uintptr_t address, T* out, size_t len) {
		if (!out) return false;

		for (size_t i = 0; i < len / sizeof(T); ++i) {
			T value; read_memory((PVOID)(address + i * sizeof(T)), &value, sizeof(T));

			if (value == T{}) { return false; }
			out[i] = value;
		}
		return true;
	}


	auto is_valid(const uint64_t address) -> bool {
		if (address <= 0x400000 || address == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(address) == nullptr || address > 0x7FFFFFFFFFFFFFFF) {
			return false;
		}
		return true;
	}
}; extern interface_t driver;