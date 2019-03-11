#pragma once
#include <vector>

#define MAX_COL_WIDTH 512
#define MAX_KEYS 9

#define DEFAULT_1K " "
#define DEFAULT_2K "fj"
#define DEFAULT_3K "f j"
#define DEFAULT_4K "dfjk"
#define DEFAULT_5K "df jk"
#define DEFAULT_6K "sdfjkl"
#define DEFAULT_7K "sdf jkl"
#define DEFAULT_8K "asdf jkl"
#define DEFAULT_9K "asdf jkl;"

namespace osu {
	extern std::wstring base_dir;
    extern std::wstring freelosu_dir;
	extern std::wstring cache_dir;
	extern std::string key_binds[MAX_KEYS];

	struct note {
		int32_t start_time;
		int32_t end_time;
	};

	class beatmap {
	public:
		beatmap(std::wstring map_dir, bool parse_notes);
		~beatmap();
		std::wstring get_full_name();
        size_t columns();
        std::vector<note> get_notes(int column);

	private:
		int32_t n_cols;
		std::wstring map_dir;
		std::wstring title;
        std::wstring artist;
        std::wstring version;
		std::vector<note> v_notes[MAX_KEYS];

		bool parse(bool parse_notes);
        bool parse_version(std::wstring version_line);
		bool parse_difficulty(std::wstring difficulty_line);
		bool parse_note(std::vector<std::wstring>& values);
	};

    const bool init(std::wstring directory);
    const std::wstring get_beatmap_dir(std::wstring beatmap_name);
	const bool load_dir_file();
	const bool get_directories();
    const bool set_default_key_binds();
    const bool load_key_binds();
}
