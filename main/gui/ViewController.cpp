/****************************************************************************************
 * ViewController.cpp - Controls which view will be displayed on the M5Stick
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
#include "gui/ViewController.h"
#include "gui/TitlePane.h"
#include "gui/CPTemperature.h"
#include "gui/CPHumidity.h"
#include "gui/CPAxpPmu1.h"
#include "gui/CPAxpPmu2.h"
#include "gui/CPAxpPmu3.h"
#include "gui/CPBmp280.h"

#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "ViewController";

    // Constructor
    ViewController::ViewController(smooth::core::Task& task_lvgl) : 
        task_lvgl(task_lvgl),

        subr_queue_next_btn_pressed(SubQNextBtnPressedEvent::create(2, task_lvgl, *this)),
            // Create Subscriber Queue so this view can listen for NEXT button pressed events
            // Create Subscriber Queue (SubQ) so the ViewController can listen for
            // NEXT button pressed events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events

        subr_queue_prev_btn_released(SubQPrevBtnReleasedEvent::create(2, task_lvgl, *this))
            // Create Subscriber Queue so this view can listen for NEXT button pressed events
            // Create Subscriber Queue (SubQ) so the ViewController can listen for
            // PREV button released events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Initialize view controller
    void ViewController::init()
    {
        Log::info(TAG, "====== Initializing ViewController ======");

        // initialize the display driver
        display_driver.initialize();
    
        // create the title panes
        title_pane = std::make_unique<TitlePane>("DHT12  Temperature");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[Temp] = std::move(title_pane);

        title_pane = std::make_unique<TitlePane>("DHT12  Humidity");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[Humidity] = std::move(title_pane);
    
        title_pane = std::make_unique<TitlePane>("BMP280");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[Bmp280] = std::move(title_pane);
    
       
        title_pane = std::make_unique<TitlePane>("AxpPMU #1");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[AxpPmu1] = std::move(title_pane);
     
        title_pane = std::make_unique<TitlePane>("AxpPMU #2");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[AxpPmu2] = std::move(title_pane);

        title_pane = std::make_unique<TitlePane>("AxpPMU #3");
        title_pane->create(LV_HOR_RES, 20);
        title_panes[AxpPmu3] = std::move(title_pane);

        // create content panes
        content_pane = std::make_unique<CPTemperature>(task_lvgl);
        content_pane->create(LV_HOR_RES, 59);
        content_panes[Temp] = std::move(content_pane);

        content_pane = std::make_unique<CPHumidity>(task_lvgl);
        content_pane->create(LV_HOR_RES, 58);
        content_panes[Humidity] = std::move(content_pane);

        content_pane = std::make_unique<CPBmp280>(task_lvgl);
        content_pane->create(LV_HOR_RES, 58);
        content_panes[Bmp280] = std::move(content_pane); 
    
        content_pane = std::make_unique<CPAxpPmu1>(task_lvgl);
        content_pane->create(LV_HOR_RES, 58);
        content_panes[AxpPmu1] = std::move(content_pane);
     
        content_pane = std::make_unique<CPAxpPmu2>(task_lvgl);
        content_pane->create(LV_HOR_RES, 58);
        content_panes[AxpPmu2] = std::move(content_pane);

        content_pane = std::make_unique<CPAxpPmu3>(task_lvgl);
        content_pane->create(LV_HOR_RES, 58);
        content_panes[AxpPmu3] = std::move(content_pane);
    
        // show new view
        show_new_view();
    }

    // Show new view
    void ViewController::show_new_view()
    {
        title_panes[new_view_id]->show();
        content_panes[new_view_id]->show();
        current_view_id = new_view_id;
    }

    // Hide current view
    void ViewController::hide_current_view()
    {
        title_panes[current_view_id]->hide();
        content_panes[current_view_id]->hide();
    }

    // Show next view
    void ViewController::show_next_view()
    {
        hide_current_view();
        new_view_id = current_view_id == AxpPmu3 ? Temp : static_cast<ViewID>(static_cast<int>(current_view_id) + 1);
        show_new_view();
    }

    // Show previous view
    void ViewController::show_prev_view()
    {
        hide_current_view();
        new_view_id = current_view_id == Temp ? AxpPmu3 : static_cast<ViewID>(static_cast<int>(current_view_id) - 1);
        show_new_view();
    }

    // The NEXT button pressed event
    void ViewController::event(const HwBtnNextPressed& event)
    {
        show_next_view();
    }

    /// The PREV button released event
    void ViewController::event(const HwBtnPrevReleased& event)
    {
        show_prev_view();
    }

}
