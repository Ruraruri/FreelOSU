#pragma once
#include <stdint.h>

namespace settings {
	extern int32_t input_delay;
    extern int32_t max_variance;
    extern int32_t median_variance;
    extern int32_t min_variance;

    // y = ax^2 + bx + c (y = variance, x = # of notes/s)
    extern double a;
    extern double b;
    extern double c;

    void calculate_graph();
    const int32_t get_variance(int32_t num_notes);

    const bool save();
    const bool load();
}
