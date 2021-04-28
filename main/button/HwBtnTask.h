/****************************************************************************************
 * HwBtnTask.h - A task to manage hardware button
 *
 * Created on Mar. 16, 2020
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

#include <smooth/core/Task.h>
#include "button/HwButton.h"
#include <array>

namespace redstone
{
    class HwBtnTask : public smooth::core::Task
    {
        public:
            /// Constructor
            HwBtnTask();

            /// Task tick
            void tick() override;

            /// Task initialize
            void init() override;

        private:
            std::array<std::unique_ptr<HwButton>, 2> hw_buttons;
    };
}
