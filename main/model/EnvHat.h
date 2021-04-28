/****************************************************************************************
 * EnvHat.h - A program to retrieve environmental measurements from DHT12 and BMP280 devices.
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

#include "model/EnvirValue.h"
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/io/i2c/DHT12.h>
#include <smooth/application/io/i2c/BME280.h>

namespace redstone
{
    class EnvHat
    {
        public:
            /// Constructor
            EnvHat();

            /// Initialize the Envir HAT
            void initialize();

            /// Read measurements for the Envir HAT
            void read_measurements();

        private:
            /// Initialize the DHT12 I2C device
            void initialize_dht12();

            /// Initialize the BME280 I2C device
            void initialize_bme280();

            smooth::core::io::i2c::Master i2c1_master;
            std::unique_ptr<smooth::application::sensor::DHT12> dht12{};
            std::unique_ptr<smooth::application::sensor::BME280> bmp280{};
            bool dht12_initialized{ false };
            bool bmp280_initialized{ false };
            EnvirValue envir_value{};
    };
}
