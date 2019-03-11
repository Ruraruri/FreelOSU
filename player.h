#pragma once
#include <atomic>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <QtConcurrent/QtConcurrent>

#include "memory.h"
#include "input.h"

namespace player {
    extern FreelOSU *gui;
    const void setup(FreelOSU *w);
    const void play_loop();
    const bool random_chance(int32_t chance);

    extern std::atomic<bool> keys_exit_flag;

    void play_key(std::vector<osu::note> notes, size_t start, char key);
    const bool play_map(osu::beatmap* map);
    const std::vector<osu::note> apply_settings(std::vector<osu::note> notes);

    void update_log(const QString& s);
}
