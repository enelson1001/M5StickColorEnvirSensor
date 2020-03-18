/****************************************************************************************
 * CPAxpPmu2.cpp - A content pane that displays the AxpPMU misc values
 *
 * Created on Jan. 04, 2020
 * Copyright (c) 2019 Ed Nelson (https://github.com/enelson1001)
 * Licensed under MIT License (see LICENSE file)
 *
 * Derivative Works
 * Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
 * Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * LittlevGL - A powerful and easy-to-use embedded GUI
 * Copyright (c) 2016 Gábor Kiss-Vámosi (https://github.com/littlevgl/lvgl)
 * Licensed under MIT License
 ***************************************************************************************/
#include <sstream>
#include <iomanip>  // for set precision
#include "gui/CPAxpPmu2.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "CPAxpPmu2";

    // Constructor
    CPAxpPmu2::CPAxpPmu2(smooth::core::Task& task_lvgl) :
            subr_queue_axp_value(SubQAxpValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this content pane can listen for
            // EnvirValue events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane
    void CPAxpPmu2::create(int width, int height)
    {
        Log::info(TAG, "Creating CPAxpPmu2");

        // create style for the content container
        lv_style_copy(&content_container_style, &lv_style_plain);
        content_container_style.body.main_color = lv_color_hex3(0X090);     // green
        content_container_style.body.grad_color = lv_color_hex3(0X090);     // green

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_cont_set_style(content_container, LV_CONT_STYLE_MAIN, &content_container_style);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 10); // Offset so content pane is below title pane
        lv_obj_set_hidden(content_container, true);

        // create style for text value
        lv_style_copy(&text_label_style, &lv_style_plain);
        text_label_style.text.color = LV_COLOR_WHITE;

        // create description for APS
        lv_obj_t* label_aps = lv_label_create(content_container, NULL);
        lv_obj_set_style(label_aps, &text_label_style);
        lv_label_set_text(label_aps, "IPSOUT   :");
        lv_obj_align(label_aps, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 4);

        // create description for Axp Temp
        lv_obj_t* label_axp_temp = lv_label_create(content_container, NULL);
        lv_obj_set_style(label_axp_temp, &text_label_style);
        lv_label_set_text(label_axp_temp, "AXP Temp :");
        lv_obj_align(label_axp_temp, label_aps, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create description for battery power
        lv_obj_t* label_batt_pwr = lv_label_create(content_container, NULL);
        lv_obj_set_style(label_batt_pwr, &text_label_style);
        lv_label_set_text(label_batt_pwr, "BAT Power:");
        lv_obj_align(label_batt_pwr, label_axp_temp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create a dynamic label for APS voltage value
        aps_voltage_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(aps_voltage_value_label, &text_label_style);
        lv_label_set_text(aps_voltage_value_label, "--");
        lv_obj_align(aps_voltage_value_label, label_aps, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for AXP device temperature value
        axp_device_temp_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(axp_device_temp_value_label, &text_label_style);
        lv_label_set_text(axp_device_temp_value_label, "--");
        lv_obj_align(axp_device_temp_value_label, label_axp_temp, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for battery power value
        battery_power_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(battery_power_value_label, &text_label_style);
        lv_label_set_text(battery_power_value_label, "--");
        lv_obj_align(battery_power_value_label, label_batt_pwr, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }

    // The published AxpValue event
    void CPAxpPmu2::event(const AxpValue& event)
    {
        aps_voltage = event.get_aps_voltage();
        axp_device_temperature = event.get_axp_device_temperature();
        battery_power = event.get_battery_power();
        update_value_texts();
    }

    // Update the axp value labels
    void CPAxpPmu2::update_value_texts()
    {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(2) << aps_voltage;
        std::string aps_voltage_text = stream.str() + "V";
        lv_label_set_text(aps_voltage_value_label, aps_voltage_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(1) << axp_device_temperature;
        std::string axp_device_temp_text = stream.str() + "C";
        lv_label_set_text(axp_device_temp_value_label, axp_device_temp_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << battery_power;
        std::string batt_pwr_text = stream.str() + "mW";
        lv_label_set_text(battery_power_value_label, batt_pwr_text.c_str());
    }

    // Show the content pane
    void CPAxpPmu2::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void CPAxpPmu2::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
