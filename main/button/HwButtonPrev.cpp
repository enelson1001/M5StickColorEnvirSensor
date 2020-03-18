/****************************************************************************************
 * HwButtonPrev.cpp - A class that creates a PREV hardware button
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
#include "button/HwButtonPrev.h"
#include <smooth/core/ipc/Publisher.h>

using namespace smooth::core::ipc;

namespace redstone
{
    // Constructor
    HwButtonPrev::HwButtonPrev(gpio_num_t pin, bool pullup, bool pulldn) : HwButton(pin, pullup, pulldn)
    {
        hw_btn_prev_pressed.set_button_id(static_cast<int>(pin));
        hw_btn_prev_released.set_button_id(static_cast<int>(pin));
    }

    // Publicize button pressed
    void HwButtonPrev::publicize_button_pressed()
    {
        Publisher<HwBtnPrevPressed>::publish(hw_btn_prev_pressed);
    }

    // Publicize button released
    void HwButtonPrev::publicize_button_released()
    {
        Publisher<HwBtnPrevReleased>::publish(hw_btn_prev_released);
    }
}
