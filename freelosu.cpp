#include "freelosu.h"
#include "ui_freelosu.h"

FreelOSU::FreelOSU(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FreelOSU)
{
    ui->setupUi(this);
    sb = ui->log_box->verticalScrollBar();
    sb->setVisible(false);

    load_settings();
}

FreelOSU::~FreelOSU()
{
    delete ui;
}

void FreelOSU::display_log(const QString& s) {
    log_string += s + '\n';
    ui->log_box->setText(log_string);
    sb->setValue(sb->maximum());
}

void FreelOSU::refresh_beatmap_cache() {
    display_log("Refreshing beatmap cache...");
    ui->refresh_cache_button->setEnabled(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    if (!osu::get_directories()) {
        display_log("Refresh failed.");
    } else {
        display_log("Refresh complete.");
    }
    ui->refresh_cache_button->setEnabled(true);
}

void FreelOSU::get_key_binds() {
    ui->set_keybind_button->setEnabled(false);

    if (!osu::load_key_binds()) {
        display_log("Failed to get key binds.");
    } else {
        display_log("Successfully loaded key binds.");
    }

    ui->set_keybind_button->setEnabled(true);
}

void FreelOSU::set_input_delay(int value) {
    settings::input_delay = value;
}

void FreelOSU::set_max_variance(int value) {
    settings::max_variance = value;
    settings::calculate_graph();
}

void FreelOSU::set_min_variance(int value) {
    settings::min_variance = value;
    settings::calculate_graph();
}

void FreelOSU::set_median_variance(int value) {
    settings::median_variance = value;
    settings::calculate_graph();
}

void FreelOSU::save_settings() {
    ui->save_button->setEnabled(false);

    if (settings::save()) {
        display_log("Saved settings.");
    } else {
        display_log("Failed to save settings.");
    }

    ui->save_button->setEnabled(true);
}

void FreelOSU::load_settings() {
    ui->load_button->setEnabled(false);

    if (settings::load()) {
        ui->input_delay_box->setValue(settings::input_delay);
        ui->min_variance_box->setValue(settings::min_variance);
        ui->median_variance_box->setValue(settings::median_variance);
        ui->max_variance_box->setValue(settings::max_variance);
        display_log("Loaded settings.");
    } else {
        display_log("Failed to load settings.");
    }

    ui->load_button->setEnabled(true);
}
