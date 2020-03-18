/****************************************************************************************
 * HwButton.cpp - A class that creates a hardware PUSHBUTTON
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
#include "button/HwButton.h"

namespace redstone
{
    /// Constructor
    HwButton::HwButton(gpio_num_t pin, bool pullup, bool pulldn) : hw_btn(pin, pullup, pulldn)
    {
    }

    // Update button history - periodically called
    void HwButton::update_button_history()
    {
        // shift left 1 position to make space for new state of button, add new button state
        btn_history = (btn_history << 1) | hw_btn.read();
    }

    // Is button in UP position
    bool HwButton::is_button_up()
    {
        return btn_history == 0b11111111;
    }

    // Is button in DOWN position
    bool HwButton::is_button_down()
    {
        return btn_history == 0b00000000;
    }

    // Is button in PRESSED
    bool HwButton::is_button_pressed()
    {
        bool btn_pressed = false;

        if ((btn_history & MASK) == 0b11000000)
        {
            btn_pressed = true;
            btn_history = 0b00000000;
        }

        return btn_pressed;
    }

    // Is button in RELEASED
    bool HwButton::is_button_released()
    {
        bool btn_released = false;

        if ((btn_history & MASK) == 0b00000111)
        {
            btn_released = true;
            btn_history = 0b11111111;
        }

        return btn_released;
    }
}
