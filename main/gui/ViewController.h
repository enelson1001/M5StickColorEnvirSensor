/****************************************************************************************
 * ViewController.h - Controls which view will be displayed on the M5Stick
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
#pragma once

#include <memory>                   // for unique_ptr
#include <unordered_map>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>

#include "gui/DisplayDriver.h"
#include "gui/IPane.h"
#include "button/HwBtnEvent.h"


namespace redstone
{
    class ViewController : public smooth::core::ipc::IEventListener<HwBtnNextPressed>,
                           public smooth::core::ipc::IEventListener<HwBtnPrevReleased>
    {
        public:
            // Constants & Enums
            enum ViewID : int
            {
                Temp = 0,
                Humidity,
                Bmp280,
                AxpPmu1,
                AxpPmu2,
                AxpPmu3
            };

            // Constructor
            ViewController(smooth::core::Task& task_lvgl);

            /// Initialize the view controller
            void init();

            /// Show the new view
            void show_new_view();

            /// Hide the current view
            void hide_current_view();

            /// Show the next view
            void show_next_view();

            /// Show the prev view
            void show_prev_view();

            /// The NEXT button pressed event
            void event(const HwBtnNextPressed& event) override;

            /// The PREV button released event
            void event(const HwBtnPrevReleased& event) override;

        private:
            smooth::core::Task& task_lvgl;
            DisplayDriver display_driver{};

            // Subscriber's queue's
            using SubQNextBtnPressedEvent = smooth::core::ipc::SubscribingTaskEventQueue<HwBtnNextPressed>;
            std::shared_ptr<SubQNextBtnPressedEvent> subr_queue_next_btn_pressed;

            using SubQPrevBtnReleasedEvent = smooth::core::ipc::SubscribingTaskEventQueue<HwBtnPrevReleased>;
            std::shared_ptr<SubQPrevBtnReleasedEvent> subr_queue_prev_btn_released;

            std::unique_ptr<IPane> content_pane;
            std::unique_ptr<IPane> title_pane;

            std::unordered_map<ViewID, std::unique_ptr<IPane>> content_panes;
            std::unordered_map<ViewID, std::unique_ptr<IPane>> title_panes;
            ViewID current_view_id{ Temp };
            ViewID new_view_id{ Temp };
    };
}
