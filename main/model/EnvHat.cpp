/****************************************************************************************
 * EnvHat.cpp - A program to retrieve environmental measurements from DHT12 and BMP280 devices.
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
#include "model/EnvHat.h"
#include <smooth/core/ipc/Publisher.h>

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::application::sensor;

namespace redstone
{
    // Class constants
    static const char* TAG = "EnvHat";

    // Constructor
    EnvHat::EnvHat() :
            i2c1_master(I2C_NUM_1,                       // I2C Port 1
                        GPIO_NUM_26,                     // SCL pin
                        false,                           // SCL internal pullup NOT enabled
                        GPIO_NUM_0,                      // SDA pin
                        false,                           // SDA internal pullup NOT enabled
                        100 * 1000)                      // clock frequency - 100kHz
    {
    }

    // Initialize the Envir HAT
    void EnvHat::initialize()
    {
        initialize_dht12();
        initialize_bme280();
    }

    // Initialize the I2C DHT12 device
    void EnvHat::initialize_dht12()
    {
        auto device = i2c1_master.create_device<DHT12>(0x5C);   // DHT12 i2c device address  0x5c
        Log::info(TAG, "Scanning for DHT12 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            dht12_initialized = true;
            dht12 = std::move(device);
        }

        Log::info(TAG, "DHT12 initialization --- {}", dht12_initialized ? "Succeeded" : "Failed");
    }

    // Initialize the I2C BMP280 device
    void EnvHat::initialize_bme280()
    {
        auto device = i2c1_master.create_device<BME280>(0x76);   // BMP280 i2c device address  0x76
        Log::info(TAG, "Scanning for BMP280 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            bool measuring = false;
            bool loading_from_nvm = false;

            Log::info(TAG, "BMP280 reset: {}", device->reset());

            while (!device->read_status(measuring, loading_from_nvm) || loading_from_nvm)
            {
                Log::info(TAG, "Waiting for BMP280 to complete reset operation... {} {}", measuring, loading_from_nvm);
            }

            bmp280_initialized = device->configure_sensor(BME280Core::SensorMode::Normal,
                                                          BME280Core::OverSampling::Oversamplingx1,
                                                          BME280Core::OverSampling::Oversamplingx1,
                                                          BME280Core::OverSampling::Oversamplingx1,
                                                          BME280Core::StandbyTimeMS::ST_1000,
                                                          BME280Core::FilterCoeff::FC_OFF);

            if (bmp280_initialized)
            {
                bmp280 = std::move(device);
            }
        }

        Log::info(TAG, "BMP280 initialization --- {}", bmp280_initialized ? "Succeeded" : "Failed");
    }

    // Read measurements
    void EnvHat::read_measurements()
    {
        if (dht12_initialized)
        {
            float temperature, humidity;
            dht12->read_measurements(humidity, temperature);
            envir_value.set_temperture_degree_C(temperature);
            envir_value.set_relative_humidity(humidity);
        }

        if (bmp280_initialized)
        {
            float temperature, humidity, pressure;
            bmp280->read_measurements(humidity, pressure, temperature);
            envir_value.set_bmp280_temperture_degree_C(temperature);
            envir_value.set_pressure_hPa(pressure);
        }

        Publisher<EnvirValue>::publish(envir_value);
    }
}
