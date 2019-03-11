#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <Windows.h>
#include "settings.h"

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

namespace settings {
    int32_t input_delay = 0;
    int32_t min_variance = 0;
    int32_t median_variance = 0;
    int32_t max_variance = 0;

    double a = 0;
    double b = 0;
    double c = 0;

    void calculate_graph() {
        c = double(min_variance);

        // Use Cramer's Rule to solve simultaneous equation
        double a1 = 25;
        double b1 = 5;
        double c1 = static_cast<double>(median_variance) - c;
        double a2 = 100;
        double b2 = 10;
        double c2 = static_cast<double>(max_variance) - c;

        b = (c1 * a2) - (c2 * a1);
        b /= (b1 * a2) - (b2 * a1);

        a = ((c1 - (b1 * b)) / a1);
    }

    const int32_t get_variance(int32_t num_notes) {
        int32_t variance = int32_t(c);
        variance += static_cast<int32_t>(a) * num_notes * num_notes;
        variance += static_cast<int32_t>(b) * num_notes;
        return variance;
    }

    const bool save() {
        wchar_t buffer[256];
        if (!GetCurrentDirectoryW(256, buffer)) {
            std::cout << "shudn't be here" << std::endl;
            return false;
        }

        std::wstring save_dir = std::wstring(buffer) + L"\\freelosu.cfg";

        std::filesystem::remove(save_dir);

        std::wofstream save_file;
        save_file.open(save_dir);

        save_file << L"input delay:" << input_delay << std::endl;
        save_file << L"min variance:" << min_variance << std::endl;
        save_file << L"median variance:" << median_variance << std::endl;
        save_file << L"max variance:" << max_variance << std::endl;

        save_file.close();

        return true;
    }

    const bool load() {
        wchar_t buffer[256];
        if (!GetCurrentDirectoryW(256, buffer))
            return false;

        std::wstring save_dir = std::wstring(buffer) + L"\\freelosu.cfg";

        std::wifstream save_file(save_dir, std::ifstream::in);

        if (!save_file.good()) {
            return false;
        }

        std::wstring current_line;

        while (std::getline(save_file, current_line)) {
            std::vector<std::wstring> values;
            split_line(current_line, ':', values);

            if(!_wcsicmp(values[0].c_str(), L"input delay")) {
                input_delay = std::stoi(values[1]);
            } else if (!_wcsicmp(values[0].c_str(), L"min variance")) {
                min_variance = std::stoi(values[1]);
            } else if (!_wcsicmp(values[0].c_str(), L"median variance")) {
                median_variance = std::stoi(values[1]);
            } else if (!_wcsicmp(values[0].c_str(), L"max variance")) {
                max_variance = std::stoi(values[1]);
            }
        }

        return true;
    }
}
