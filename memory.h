#pragma once
#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include "freelosu.h"

namespace memory {

	extern DWORD time_address;
	extern DWORD game_process_id;
	extern HANDLE game_handle;
	extern HWND game_window;

    bool init();
    bool exit();
    DWORD get_process_id();
    DWORD find_time_address();
    HWND find_window();

    int32_t get_current_time();
	
	DWORD find_pattern(unsigned char pattern[], const size_t signature_size);

    bool is_in_game();
    std::wstring get_current_map();
    std::wstring get_active_dir();
}
