/****************************************************************************************
 * CPAxpPmu1.cpp - A content pane that displays the AxpPMU power input values.
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
#include "gui/CPAxpPmu1.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "CPAxpPmu1";

    // Constructor
    CPAxpPmu1::CPAxpPmu1(smooth::core::Task& task_lvgl) :
            subr_queue_axp_value(SubQAxpValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this content pane can listen for
            // EnvirValue events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane
    void CPAxpPmu1::create(int width, int height)
    {
        Log::info(TAG, "Creating CPAxpPmu1");

        // create a plain style
        lv_style_init(&plain_style);
        lv_style_set_pad_top(&plain_style, LV_STATE_DEFAULT, 10);
        lv_style_set_pad_bottom(&plain_style, LV_STATE_DEFAULT, 10);
        lv_style_set_pad_left(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_right(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_line_opa(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_inner(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_margin_all(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_border_width(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_radius(&plain_style, LV_STATE_DEFAULT, 0);

        // create style for the content container
        lv_style_copy(&content_container_style, &plain_style);
        lv_style_set_bg_color(&content_container_style, LV_STATE_DEFAULT, lv_color_hex3(0x090));  // green

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_add_style(content_container, LV_CONT_PART_MAIN, &content_container_style);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 10); // Offset so content pane is below title pane
        lv_obj_set_hidden(content_container, true);

        // create style for text value
        lv_style_init(&text_label_style);
        lv_style_set_text_color(&text_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_text_font(&text_label_style, LV_STATE_DEFAULT, &lv_font_montserrat_12);

        // create description for ACIN
        lv_obj_t* label_acin = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_acin, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_acin, "ACIN:  ");
        lv_obj_align(label_acin, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 4);

        // create description for VBUS
        lv_obj_t* label_vbus = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_vbus, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_vbus, "VBUS:  ");
        lv_obj_align(label_vbus, label_acin, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create description for Battery
        lv_obj_t* label_battery = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_battery, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_battery, "BATT:  ");
        lv_obj_align(label_battery, label_vbus, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create a dynamic label for ACIN voltage value
        acin_voltage_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(acin_voltage_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(acin_voltage_value_label, "--");
        lv_obj_align(acin_voltage_value_label, label_acin, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for VBUS voltage value
        vbus_voltage_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(vbus_voltage_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(vbus_voltage_value_label, "--");
        lv_obj_align(vbus_voltage_value_label, label_vbus, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for battery voltage value
        battery_voltage_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(battery_voltage_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(battery_voltage_value_label, "--");
        lv_obj_align(battery_voltage_value_label, label_battery, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for ACIN current value
        acin_current_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(acin_current_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(acin_current_value_label, "--");
        lv_obj_align(acin_current_value_label, label_acin, LV_ALIGN_OUT_RIGHT_MID, 50, 0);

        // create a dynamic label for VBUS current value
        vbus_current_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(vbus_current_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(vbus_current_value_label, "--");
        lv_obj_align(vbus_current_value_label, label_vbus, LV_ALIGN_OUT_RIGHT_MID, 50, 0);

        // create a dynamic label for battery current value
        battery_current_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(battery_current_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(battery_current_value_label, "--");
        lv_obj_align(battery_current_value_label, label_battery, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
    }

    // The published AxpValue event
    void CPAxpPmu1::event(const AxpValue& event)
    {
        acin_voltage = event.get_acin_voltage();
        vbus_voltage = event.get_vbus_voltage();
        battery_voltage = event.get_battery_voltage();
        acin_current = event.get_acin_current();
        vbus_current = event.get_vbus_current();
        battery_current = event.get_battery_charging_current() - event.get_battery_discharging_current();
        update_value_texts();
    }

    // Update the axp value labels
    void CPAxpPmu1::update_value_texts()
    {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(2) << acin_voltage;
        std::string acin_voltage_text = stream.str() + "V";
        lv_label_set_text(acin_voltage_value_label, acin_voltage_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(2) << vbus_voltage;
        std::string vbus_voltage_text = stream.str() + "V";
        lv_label_set_text(vbus_voltage_value_label, vbus_voltage_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(2) << battery_voltage;
        std::string battery_voltage_text = stream.str() + "V";
        lv_label_set_text(battery_voltage_value_label, battery_voltage_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << acin_current;
        std::string acin_current_text = stream.str() + "mA";
        lv_label_set_text(acin_current_value_label, acin_current_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << vbus_current;
        std::string vbus_current_text = stream.str() + "mA";
        lv_label_set_text(vbus_current_value_label, vbus_current_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << battery_current;
        std::string battery_current_text = stream.str() + "mA";
        lv_label_set_text(battery_current_value_label, battery_current_text.c_str());
    }

    // Show the content pane
    void CPAxpPmu1::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void CPAxpPmu1::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
