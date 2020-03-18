/****************************************************************************************
 * HwBtnTask.cpp - A task to manage hardware button
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
#include "button/HwBtnTask.h"
#include "button/HwButtonNext.h"
#include "button/HwButtonPrev.h"

using namespace std::chrono;

namespace redstone
{
    // Constructor
    HwBtnTask::HwBtnTask() :
            smooth::core::Task("HwBtnTask", 3000, 10, milliseconds(10))

            // The Task Name = "HwBtnTask"
            // The stack size is 3000 bytes
            // The priority is set to 10
            // The tick interval is set for 10 milliseconds
    {
    }

    // Task init
    void HwBtnTask::init()
    {
        hw_buttons[0] = std::make_unique<HwButtonNext>(GPIO_NUM_39, false, false);
        hw_buttons[1] = std::make_unique<HwButtonPrev>(GPIO_NUM_37, false, false);
    }

    // Task tick
    void HwBtnTask::tick()
    {
        // loop thru all the buttons, debounce, and publish event
        for (auto const& hw_btn : hw_buttons)
        {
            hw_btn->update_button_history();

            if (hw_btn->is_button_pressed())
            {
                hw_btn->publicize_button_pressed();
            }

            if (hw_btn->is_button_released())
            {
                hw_btn->publicize_button_released();
            }
        }
    }
}
