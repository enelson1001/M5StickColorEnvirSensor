/****************************************************************************************
 * HwButton.h - A class that creates a hardware PUSHBUTTON
 *
 * Created on Mar. 20, 2020
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

/////////////////////////////////////////////////////////////////////////////////////////
//  This class along with HwBtnTask class and HwBtnEvent class are used to debounce a
//  hardware pushbutton and publish an "button pressed event" or "button released event"
//  if the hardware butoon is pressed or released. This software is based upon the
//  "Hackaday - Embed with Elliot: Debounce your noisy buttons, part II" as described at
//  https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
//
//  Breif description:
//  The software uses a "pattern matching” debounce algorithm to determine if a button is
//  pressed, is released, is up or is down.  This is accomplished by using a byte that will
//  the hold 80 milliseconds of button state history.  Every 10 milliseconds a new button
//  state is shifted into the history byte. This byte is then compared to a known pattern
//  to determine if the button is pressed, released, up or down.
//  The HwBtnTask task is set for 10 milliseconds and the tick() function calls the hwButton
//  update_button_history() and then checks to see if button is pressed or released.  If
//  the button is pressed it calls the hwButton function publicize_button_pressed. If the
//  button is released it calls the hwButton function publicize_button_released.
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <smooth/core/io/Input.h>

namespace redstone
{
    class HwButton
    {
        public:
            /// Constructor
            /// \param pin The GPIO pin used for the button
            /// \param pullup Set true to enable pullup on pin, set false to disable pullup
            /// \param pulldn Set true to enable pulldown on pin, set false to disable pulldown
            HwButton(gpio_num_t pin, bool pullup, bool pulldn);

            /// Destructor
            virtual ~HwButton() {}

            /// Update button history - periodically called
            virtual void update_button_history();

            /// Is button in UP position
            /// \param return Return true is button is in up position, false if not
            virtual bool is_button_up();

            /// Is button in DOWN position
            /// \param return Return true is button is in down position, false if not
            virtual bool is_button_down();

            /// Is button in PRESSED
            /// \param return Return true is button is pressed, false if not
            virtual bool is_button_pressed();

            /// Is button in RELEASED
            /// \param return Return true is button is released, false if not
            virtual bool is_button_released();

            /// Publicize button pressed
            virtual void publicize_button_pressed() = 0;

            /// Publicize button released
            virtual void publicize_button_released() = 0;

        private:
            smooth::core::io::Input hw_btn;

            // For M5StickC the button input is high when not pressed
            uint8_t btn_history{ 0xFF };

            // 0b11xxx111 where xxx represent don't care bits which we set to 0
            static constexpr uint8_t MASK = 0b11000111;
    };
}
