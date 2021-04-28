/****************************************************************************************
 * HwButtonNext.h - A class that creates a NEXT Hardware button
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

#include "button/HwButton.h"
#include "button/HwBtnEvent.h"

namespace redstone
{
    class HwButtonNext : public HwButton
    {
        public:
            /// Constructor
            /// \param pin The GPIO pin used for the NEXT button
            /// \param pullup Set true to enable pullup on pin, set false to disable pullup
            /// \param pulldn Set true to enable pulldown on pin, set false to disable pulldown
            HwButtonNext(gpio_num_t pin, bool pullup, bool pulldn );

            /// Publicize button pressed
            void publicize_button_pressed() override;

            /// Publicize button released
            void publicize_button_released() override;

        private:
            HwBtnNextPressed hw_btn_next_pressed;
            HwBtnNextReleased hw_btn_next_released;
    };
}
