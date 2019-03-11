
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <psapi.h>
#include "memory.h"
#include "global.h"

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

static BOOL is_main_window(HWND handle) {
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

static BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle))
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

namespace memory {

	DWORD time_address = NULL;
	DWORD game_process_id = NULL;
	HANDLE game_handle = NULL;
	HWND game_window = NULL;

    bool init() {
		// find process id of osu!
		game_process_id = get_process_id();

		if (!game_process_id) {
			std::cerr << "error: failed to find osu! process." << std::endl;
		}

		// open the process with memory reading access.
		game_handle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, game_process_id);

		if (!game_handle) {
			std::cerr << "error: failed to open handle to osu! process. (" << GetLastError() << ")" << std::endl;
			return false;
		}

		// find the osu! window
		game_window = find_window();

		if (!game_window) {
			std::cerr << "error: failed to find osu! window." << std::endl;
			return false;
		}

		// find the time address.
        time_address = find_time_address();

		if (!time_address) {
			std::cerr << "error: failed to find pointer to time address." << std::endl;
			return false;
		}

		return true;
	}

    bool exit() {
        CloseHandle(game_handle);
        return true;
    }

    DWORD get_process_id() {
		// store process ID to return
		DWORD process_id = NULL;

		// iterate process list for "osu!.exe"
		HANDLE process_list = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 entry = { 0 };
        entry.dwSize = sizeof (PROCESSENTRY32);

		if (Process32First(process_list, &entry)) {
			while (Process32Next(process_list, &entry)) {
				if (_wcsicmp(entry.szExeFile, L"osu!.exe") == 0) {
					process_id = entry.th32ProcessID;
					break;
				}
			}
		}

		CloseHandle(process_list);

		return process_id;
	}

    DWORD find_time_address() {
		// scan process memory for array of bytes
		DWORD time_ptr = find_pattern(PBYTE(TIME_SIGNATURE), sizeof(TIME_SIGNATURE) - 1) + TIME_OFFSET;
		DWORD time_address = NULL;
        
        if (!ReadProcessMemory(game_handle, LPCVOID(time_ptr), &time_address, sizeof (DWORD), nullptr)) {
			return false;
		}

		return time_address;
	}

    HWND find_window() {
		handle_data data;
		data.process_id = game_process_id;
        data.window_handle = nullptr;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.window_handle;
	}

    int32_t get_current_time() {
		int32_t current_time = NULL;

        if (!ReadProcessMemory(game_handle, LPCVOID(time_address), &current_time, sizeof (int32_t), nullptr)) {
			return false;
		}

        return (int32_t)current_time;
	}

	DWORD find_pattern(unsigned char pattern[], const size_t signature_size) {
        const size_t read_size = 4096;
        bool hit = false;

        unsigned char chunk[read_size];

        for (size_t i = 0; i < INT_MAX; i += read_size - signature_size) {
            ReadProcessMemory(game_handle, LPCVOID(i), &chunk, read_size, NULL);

            for (size_t a = 0; a < read_size; a++) {
                hit = true;

                for (size_t j = 0; j < signature_size && hit; j++) {
                    if (chunk[a + j] != pattern[j]) {
                        hit = false;
                    }
                }

                if (hit) {
                    return i + a;
                }
            }
        }

		return NULL;
	}

    bool is_in_game() {
		wchar_t buffer[256] = { 0 };
		GetWindowTextW(game_window, buffer, 256);
		std::wstring window_name(buffer);

		return window_name.compare(L"osu!") ? true : false;
	}

    std::wstring get_current_map() {
		wchar_t buffer[256] = { 0 };
		GetWindowTextW(game_window, buffer, 256);
		std::wstring window_name(buffer);

		return window_name.substr(8, window_name.length() - 8);
	}

    std::wstring get_active_dir() {
        wchar_t buffer[256] = { 0 };
        GetModuleFileNameExW(game_handle, NULL, buffer, 256);
        std::wstring dir = std::wstring(buffer);
        return dir.substr(0, dir.size() - 9);
    }

}
