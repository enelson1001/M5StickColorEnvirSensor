/****************************************************************************************
 * CPHumidity.h -  A content pane that displays humidity, heat index and dew point
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
#include "model/EnvirValue.h"

namespace redstone
{
    class CPHumidity : public IPane, public smooth::core::ipc::IEventListener<EnvirValue>
    {
        public:
            /// Constructor
            /// \param task_lvgl The task this class is running under
            CPHumidity(smooth::core::Task& task_lvgl);

            /// Show the content pane
            void show() override;

            /// Hide the content pane
            void hide() override;

            /// Create the content pane
            /// \param width The width of the content pane
            /// \param height The height of the content pane
            void create(int width, int height) override;

            /// The EnvirValue event that this instance listens for
            void event(const EnvirValue& event) override;

        private:
            /// Update the value texts
            void update_value_texts();

            // Subscriber's queue's
            using SubQEnvirValue = smooth::core::ipc::SubscribingTaskEventQueue<EnvirValue>;
            std::shared_ptr<SubQEnvirValue> subr_queue_envir_value;

            lv_style_t plain_style;
            lv_style_t content_container_style;
            lv_style_t text_label_style;
            lv_obj_t* content_container;
            lv_obj_t* humidity_value_label;
            lv_obj_t* heat_index_value_label;
            lv_obj_t* dew_point_value_label;

            float humidity;
            float heat_index;
            float dew_point;
    };
}
