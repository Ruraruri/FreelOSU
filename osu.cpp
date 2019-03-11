#include <exception>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <Windows.h>

#include "osu.h"
#include "global.h"

static inline bool split_line(std::wstring line, wchar_t delimiter, std::vector<std::wstring>& result) {
	std::wstringstream input_line(line);
	std::wstring current_line;

	while (std::getline(input_line, current_line, delimiter)) {
		if (!current_line.empty()) {
			result.push_back(current_line);
		}
	}

	return (result.size() >= 2);
};

static inline std::wstring string_to_wstring(const std::string& s)
{
    std::wstring temp(s.length(),L' ');
    std::copy(s.begin(), s.end(), temp.begin());
    return temp;
}


static inline std::string wstring_to_string(const std::wstring& s)
{
    std::string temp(s.length(), ' ');
    std::copy(s.begin(), s.end(), temp.begin());
    return temp;
}

namespace osu {
	// beatmap directory map and keybinds array initialisations
	std::wstring base_dir;
	std::wstring cache_dir;
    std::wstring freelosu_dir;
	std::string key_binds[MAX_KEYS];


    // beatmap class methods initialisations
	beatmap::beatmap(std::wstring directory, bool parse_notes) {
		map_dir = directory;

		if (!parse(parse_notes)) {
			throw std::runtime_error("error: could not parse beatmap");
		}
	}

	beatmap::~beatmap() {
	}

	std::wstring beatmap::get_full_name() {
        return artist + L" - " + title + L" [" + version + L"]";
	}

    size_t beatmap::columns() {
        return n_cols;
    }

    std::vector<note> beatmap::get_notes(int column) {
        return v_notes[column];
    }

	bool beatmap::parse(bool parse_notes) {
		std::wifstream beatmap_file(map_dir, std::ifstream::in);

		if (!beatmap_file.good()) {
			return false;
		}

		std::wstring current_line;
		while (std::getline(beatmap_file, current_line)) {
			static std::wstring current_section;

			if (!current_line.empty() && current_line.front() == '[' && current_line.back() == ']') {
				current_section = current_line.substr(1, current_line.length() - 2);
				continue;
			}

			if (current_section.empty()) {
				continue;
			}

            if (!current_section.compare(L"Metadata")) {
                this->parse_version(current_line);
            } else if (!current_section.compare(L"Difficulty")) {
				this->parse_difficulty(current_line);
			} else if (!current_section.compare(L"HitObjects") && parse_notes) {
				std::vector<std::wstring> values;

				if (!split_line(current_line, L',', values)) {
					continue;
				}

				this->parse_note(values);
			}
		}

		return true;
	}

    bool beatmap::parse_version(std::wstring version_line) {
        std::vector<std::wstring> values;

        if (split_line(version_line, ':', values)) {
            if (!_wcsicmp(values[0].c_str(), L"title")) {
				this->title = values[1];
				for (size_t i = 2; i < values.size(); i++) {
					this->title += L":" + values[i];
				}
            } else if (!_wcsicmp(values[0].c_str(), L"artist")) {
                this->artist = values[1];
            } else if (!_wcsicmp(values[0].c_str(), L"version")) {
                this->version = values[1];
            }
        }

        return true;
    }

	bool beatmap::parse_difficulty(std::wstring difficulty_line) {
		std::vector<std::wstring> values;

		if (split_line(difficulty_line, ':', values)) {
			if (!_wcsicmp(values[0].c_str(), L"circlesize")) {
				n_cols = std::stoi(values[1]);
			}
		}

		return true;
	}

	bool beatmap::parse_note(std::vector<std::wstring>& values) {
		note new_note;
		int32_t x = std::stoi(values.at(0));
		int32_t column = x / (MAX_COL_WIDTH / n_cols);

		if (column < 0) {
			column = 0;
		} else if (column >= n_cols) {
			column = n_cols - 1;
		}

		new_note.start_time = std::stoi(values.at(2));
		new_note.end_time = std::stoi(values.at(5));

        if (new_note.end_time < new_note.start_time + 25)
            new_note.end_time = new_note.start_time + 25;

		this->v_notes[column].push_back(new_note);

		return true;
	}


    // general osu methods
    const bool init(std::wstring directory) {
		base_dir = directory;
		DWORD file_type = GetFileAttributesW(base_dir.c_str());
		if (file_type == INVALID_FILE_ATTRIBUTES) {
			return false;
		}

        wchar_t buffer[MAX_DIR_LENGTH];
        if (!GetCurrentDirectoryW(MAX_DIR_LENGTH, buffer))
            return false;

        freelosu_dir = std::wstring(buffer);

		set_default_key_binds();
        load_key_binds();
		return load_dir_file();
       
    }

    const std::wstring get_beatmap_dir(std::wstring beatmap_name) {
		std::wifstream dirs_file(cache_dir, std::ifstream::in);

		if (!dirs_file.good()) {
            return nullptr;
		}

		std::wstring current_line;
		while (std::getline(dirs_file, current_line)) {
			std::vector<std::wstring> values;
			split_line(current_line, '`', values);
			if (!values[0].compare(beatmap_name)) {
				dirs_file.close();
				return values[1];
			}
		}

        return nullptr;
    }

	const bool load_dir_file() {
        cache_dir = freelosu_dir + L"\\dirs.dat";

		if (!std::filesystem::exists(cache_dir)) {
			std::cout << "Could not find dirs.dat, creating..." << std::endl;
			return get_directories();
		}

		return true;
	}

	const bool set_default_key_binds() {
		// default key settings
		key_binds[0] = DEFAULT_1K;
		key_binds[1] = DEFAULT_2K;
		key_binds[2] = DEFAULT_3K;
		key_binds[3] = DEFAULT_4K;
		key_binds[4] = DEFAULT_5K;
		key_binds[5] = DEFAULT_6K;
		key_binds[6] = DEFAULT_7K;
		key_binds[7] = DEFAULT_8K;
		key_binds[8] = DEFAULT_9K;
		return true;
	}

    const bool get_directories() {
		std::filesystem::remove(cache_dir);

		std::wofstream new_file;
		new_file.open(cache_dir);

		std::wstring songs_dir = base_dir;
		songs_dir.append(L"\\Songs");

		int32_t num_parsed = 0;

		for (const auto & folder_entry : std::filesystem::directory_iterator(songs_dir)) {
			for (const auto & file_entry : std::filesystem::directory_iterator(folder_entry.path())) {
				std::wstring map_file = file_entry.path();

				if (!map_file.substr(map_file.size() - 4, 4).compare(L".osu")) {
					try {
						osu::beatmap *map = new osu::beatmap(map_file, false);
                        new_file << map->get_full_name() << L"`" << map_file << std::endl;
                        std::wcout << L"parsed dir: " << map->get_full_name() << std::endl;
					}
					catch (const std::exception& e) {
						std::wcerr << e.what() << L" (" << map_file << L")" << std::endl;
						return false;
					}

					num_parsed++;
				}
			}
		}

		new_file.close(); 
		return (num_parsed > 0);
	}

    const bool load_key_binds() {
        std::wstring keybind_dir = freelosu_dir + L"\\keybinds.cfg";

        if (!std::filesystem::exists(keybind_dir)) {
            std::cout << "Could not find keybinds.cfg, creating..." << std::endl;
            std::wofstream new_file;
            new_file.open(keybind_dir);
            for (size_t i = 0 ; i < MAX_KEYS; i ++) {
                new_file << (i + 1) << L"K:" << string_to_wstring(key_binds[i]) << std::endl;
            }
            new_file.close();
        } else {
            std::wifstream keybind_file(keybind_dir, std::ifstream::in);

            if (!keybind_file.good()) {
                return false;
            }

            std::wstring current_line;
            for (int i = 0; i < MAX_KEYS; i ++) {
                std::getline(keybind_file, current_line);
                std::vector<std::wstring> values;
                split_line(current_line, ':', values);
                key_binds[i] = wstring_to_string(values[1]);
            }
        }

        return true;
    }
}
