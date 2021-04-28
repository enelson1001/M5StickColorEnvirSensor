/****************************************************************************************
 * CPHumidity.cpp - A content pane that displays humidity, heat index and dew point
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
#include "gui/CPHumidity.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "CPHumidity";

    // Constructor
    CPHumidity::CPHumidity(smooth::core::Task& task_lvgl) :
            subr_queue_envir_value(SubQEnvirValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this content pane can listen for
            // EnvirValue events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane
    void CPHumidity::create(int width, int height)
    {
        Log::info(TAG, "Creating CPHumidity");

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
        lv_style_set_bg_color(&content_container_style, LV_STATE_DEFAULT, lv_color_hex3(0x00c));  // blue

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_add_style(content_container, LV_CONT_PART_MAIN, &content_container_style);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 10); // Offset so content pane is below title pane
        lv_obj_set_hidden(content_container, true);

        // create style for labels
        lv_style_init(&text_label_style);
        lv_style_set_text_color(&text_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_text_font(&text_label_style, LV_STATE_DEFAULT, &lv_font_montserrat_12);

        // create description for humidity
        lv_obj_t* label_humid = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_humid, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_humid, "Humidity  :  ");
        lv_obj_align(label_humid, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 4);

        // create description for heat index
        lv_obj_t* label_heat_index = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_heat_index, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_heat_index, "Heat Index:  ");
        lv_obj_align(label_heat_index, label_humid, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create description for dew point
        lv_obj_t* label_dew_point = lv_label_create(content_container, NULL);
        lv_obj_add_style(label_dew_point, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(label_dew_point, "Dew Point :  ");
        lv_obj_align(label_dew_point, label_heat_index, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

        // create a dynamic label for humidity measurement value
        humidity_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(humidity_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(humidity_value_label, "--");
        lv_obj_align(humidity_value_label, label_humid, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for heat index measurement value
        heat_index_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(heat_index_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(heat_index_value_label, "--");
        lv_obj_align(heat_index_value_label, label_heat_index, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // create a dynamic label for dew point measurement value
        dew_point_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(dew_point_value_label, LV_LABEL_PART_MAIN, &text_label_style);
        lv_label_set_text(dew_point_value_label, "--");
        lv_obj_align(dew_point_value_label, label_dew_point, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }

    // The published EnvirValue event
    void CPHumidity::event(const EnvirValue& event)
    {
        humidity = event.get_relative_humidity();
        heat_index = event.get_heat_index_fahrenheit();
        dew_point = event.get_dew_point_fahrenheit();
        update_value_texts();
    }

    // Update the value text labels
    void CPHumidity::update_value_texts()
    {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(1) << humidity;
        std::string humid_text = stream.str() + "%RH";
        lv_label_set_text(humidity_value_label, humid_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(1) << heat_index;
        std::string heat_index_text = stream.str() + "F";
        lv_label_set_text(heat_index_value_label, heat_index_text.c_str());

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(1) << dew_point;
        std::string dew_point_text = stream.str() + "F";
        lv_label_set_text(dew_point_value_label, dew_point_text.c_str());
    }

    // Show the content pane
    void CPHumidity::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void CPHumidity::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
