/****************************************************************************************
 * HwButtonNext.cpp - A class that creates a NEXT hardware button
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
#include "button/HwButtonNext.h"
#include <smooth/core/ipc/Publisher.h>

using namespace smooth::core::ipc;

namespace redstone
{
    // Constructor
    HwButtonNext::HwButtonNext(gpio_num_t pin, bool pullup, bool pulldn) : HwButton(pin, pullup, pulldn)
    {
        hw_btn_next_pressed.set_button_id(static_cast<int>(pin));
        hw_btn_next_released.set_button_id(static_cast<int>(pin));
    }

    // Publicize button pressed
    void HwButtonNext::publicize_button_pressed()
    {
        Publisher<HwBtnNextPressed>::publish(hw_btn_next_pressed);
    }

    // Publicize button released
    void HwButtonNext::publicize_button_released()
    {
        Publisher<HwBtnNextReleased>::publish(hw_btn_next_released);
    }
}
