/****************************************************************************************
 * CPAxpPmu2.h - A content pane that displays the AxpPMU misc values.
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

#include <memory>  // for shared pointer
#include <lvgl/lvgl.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include "gui/IPane.h"
#include "model/AxpValue.h"

namespace redstone
{
    class CPAxpPmu2 : public IPane, public smooth::core::ipc::IEventListener<AxpValue>
    {
        public:
            /// Constructor
            /// \param task_lvgl The task this class is running under
            CPAxpPmu2(smooth::core::Task& task_lvgl);

            /// Show the content pane
            void show() override;

            /// Hide the content pane
            void hide() override;

            /// Create the content pane
            /// \param width The width of the content pane
            /// \param height The height of the content pane
            void create(int width, int height) override;

            /// The AxpValue event that this instance listens for
            void event(const AxpValue& event) override;

        private:
            /// Update the axp value text
            void update_value_texts();

            // Subscriber's queue's
            using SubQAxpValue = smooth::core::ipc::SubscribingTaskEventQueue<AxpValue>;
            std::shared_ptr<SubQAxpValue> subr_queue_axp_value;

            lv_style_t plain_style;
            lv_style_t content_container_style;
            lv_style_t text_label_style;
            lv_style_t heat_index_label_style;
            lv_obj_t* content_container;
            lv_obj_t* aps_voltage_value_label;
            lv_obj_t* axp_device_temp_value_label;
            lv_obj_t* battery_power_value_label;

            float aps_voltage;
            float axp_device_temperature;
            float battery_power;
    };
}
