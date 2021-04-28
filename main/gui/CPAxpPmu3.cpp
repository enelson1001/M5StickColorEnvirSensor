/****************************************************************************************
 * CPAxpPmu3.cpp - A content pane that displays the AxpPMU battery charging values.
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
#include "gui/CPAxpPmu3.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "CPAxpPmu3";

    // Constructor
    CPAxpPmu3::CPAxpPmu3(smooth::core::Task& task_lvgl) :
            subr_queue_axp_value(SubQAxpValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this content pane can listen for
            // EnvirValue events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane
    void CPAxpPmu3::create(int width, int height)
    {
        Log::info(TAG, "Creating CPAxpPmu3");

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

        // create description for battery capacity
        lv_obj_t* label_batt_cap = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_batt_cap, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_batt_cap, "Bat Charge:  ");
        lv_obj_align(label_batt_cap, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 4);

        // create description for battery charging current
        lv_obj_t* label_batt_chg = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_batt_chg, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_batt_chg, "Bat Icharg:  ");
        lv_obj_align(label_batt_chg, label_batt_cap, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create description for battery discharging current
        lv_obj_t* label_batt_dischg = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_batt_dischg, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_batt_dischg, "Bat Idchrg:  ");
        lv_obj_align(label_batt_dischg, label_batt_chg, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create a dynamic label for battery capacity value
        battery_capacity_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(battery_capacity_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(battery_capacity_value_label, "--");
        lv_obj_align(battery_capacity_value_label, label_batt_cap, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for battery charging current value
        battery_charging_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(battery_charging_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(battery_charging_value_label, "--");
        lv_obj_align(battery_charging_value_label, label_batt_chg, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for battery discharging current value
        battery_discharging_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(battery_discharging_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(battery_discharging_value_label, "--");
        lv_obj_align(battery_discharging_value_label, label_batt_dischg, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }

    // The published AxpValue event
    void CPAxpPmu3::event(const AxpValue& event)
    {
        battery_capacity = event.get_battery_capacity();
        battery_charging_current = event.get_battery_charging_current();
        battery_discharging_current = event.get_battery_discharging_current();
        update_value_texts();
    }

    // Update the axp value labels
    void CPAxpPmu3::update_value_texts()
    {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(1) << battery_capacity;
        std::string batt_cap_text = stream.str() + "mAh";
        lv_label_set_text(battery_capacity_value_label, batt_cap_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << battery_charging_current;
        std::string batt_chg_text = stream.str() + "mA";
        lv_label_set_text(battery_charging_value_label, batt_chg_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << battery_discharging_current;
        std::string batt_dischg_text = stream.str() + "mA";
        lv_label_set_text(battery_discharging_value_label, batt_dischg_text.c_str());
    }

    // Show the content pane
    void CPAxpPmu3::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void CPAxpPmu3::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
