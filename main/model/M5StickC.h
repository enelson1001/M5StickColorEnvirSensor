/****************************************************************************************
 * M5StickC.h - A driver for the M5Stick core module
 *
 * Created on Jan. 15, 2020
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
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/io/i2c/PCF8563.h>
#include <smooth/application/io/i2c/AxpPMU.h>
#include "model/AxpValue.h"

namespace redstone
{
    class M5StickC
    {
        public:
            /// Constructor
            M5StickC();

            /// Initialize the M5StickC
            void initialize();

            /// Set screen brightness
            /// \param brightness The brightness level; 0x00=1.8V=Dark, 0x0F=3.3V=Bright
            void set_screen_brightness(uint8_t brightness);

            /// Read measurement from the AxpPMU device
            void read_axp_measurements();

            /// Print the Axp192 measurements
            void print_axp192_report();

            /// Set RTC time
            void set_time();

            /// Get RTC time
            void get_time();

            /// Set RTC alarm
            void set_alarm();

            /// Get RTC alarm
            void get_alarm();

            /// Is RTC alarm active
            bool is_alarm_active();

            /// Clear RTC alarm
            void clear_alarm_active();

        private:
            /// Initialize the Axp192
            void initialize_axp192();

            /// Initialize the RTC BM8563
            void initialize_rtc_bm8563();

            /// Initialize the GYRO MPU6886 - to be completed later
            void initialize_gyro_mpu6886();

            smooth::core::io::i2c::Master i2c0_master;
            std::unique_ptr<smooth::application::sensor::AxpPMU> axp192{};
            std::unique_ptr<smooth::application::sensor::PCF8563> bm8563{};

            bool axp192_initialized{ false };
            bool bm8563_initialized{ false };
            AxpValue axp_value;
    };
}
