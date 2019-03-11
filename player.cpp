#include "player.h"
#include <iostream>
namespace player {
    FreelOSU *gui;
    std::atomic<bool> keys_exit_flag{ false };

    const bool random_chance(int32_t chance) {
        return ((rand() % 100) < chance);
    }

    const void play_loop() {
        bool got_error = false;
        osu::beatmap *map = nullptr;
        std::srand(std::time(nullptr));

        while (true) {
            if (memory::is_in_game() && !got_error) {
                std::wstring current_map = memory::get_current_map();
                QString current_map_QStr = QString::fromStdWString(current_map);

                try {
                    map = new osu::beatmap(osu::get_beatmap_dir(current_map), true);
                    update_log("Now playing: " + current_map_QStr);

                    player::play_map(map);

                    if (map != nullptr) {
                        delete map;
                    }
                }
                catch (std::exception &e) {
                    update_log("Could not parse map: (" + current_map_QStr + ")");
                    got_error = true;
                }
            } else {
                if (!memory::is_in_game())
                    got_error = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    const void setup(FreelOSU *w) {
        gui = w;
        if (!memory::init()) {
            update_log("error: failed to initialise memory module.");
            return;
        }

        if (!osu::init(memory::get_active_dir())) {
            update_log("error: failed to inialise file handling module.");
            return;
        }

        update_log("Succesfully initialised.");

        play_loop();
    }

    void play_key(std::vector<osu::note> notes, size_t start, char key) {
        size_t i = start;
        int32_t time = 0;
        bool key_down = false;

        while (i < notes.size() && !keys_exit_flag) {
            time = memory::get_current_time();

            if (time >= notes[i].start_time && !key_down) {
                // press key.
                set_key_pressed(key, true);
                key_down = true;
            }

            if (time > notes[i].end_time && key_down) {
                // release key.
                set_key_pressed(key, false);
                key_down = false;

                // advance to next note.
                i++;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (key_down)
            set_key_pressed(key, false);
    }

    const bool play_map(osu::beatmap* map) {

		int n_keys = map->columns();
        int32_t time = memory::get_current_time();
        int current_note[MAX_KEYS] = { 0 };

        // wind forward to where we need to start from.
        for (int i = 0; i < n_keys; i++) {
            std::vector<osu::note> col = map->get_notes(i);
            for (int j = 0; j < col.size(); j++) {
                if (col[j].start_time > time) {
                    current_note[i] = j;
                    break;
                }
            }
        }

        // start up threads
        keys_exit_flag = false;
        for (int i = 0; i < map->columns(); i++) {
            std::thread key_thread(play_key,
                                   apply_settings(map->get_notes(i)),
                                   current_note[i],
                                   osu::key_binds[n_keys - 1][i]);
            key_thread.detach();
        }

        // set exit flag for threads if not in game
        while (memory::is_in_game()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        keys_exit_flag = true;

        return true;
    }

    const std::vector<osu::note> apply_settings(std::vector<osu::note> notes) {
        std::vector<osu::note> new_notes;
        size_t prev_index = 0;

        for (size_t i = 0; i < notes.size(); i ++) {
            while (notes[prev_index].start_time < notes[i].start_time - 1000) {
                prev_index ++;
            }

            // apply variance
            int32_t variance = (random_chance(50) ? -1 : 1);
            variance *= rand() % (settings::get_variance(i - prev_index) + 1);

            osu::note new_note;
            new_note.start_time = notes[i].start_time + variance - settings::input_delay;
            new_note.end_time = notes[i].end_time + variance - settings::input_delay;
            new_notes.push_back(new_note);

        }

        return new_notes;
    }


    void update_log(const QString& s) {
        QMetaObject::invokeMethod(gui,
                                  "display_log",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, s));
    }

}
