/****************************************************************************************
 * M5StickC.cpp - A driver for the M5Stick core module
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

/************************************************************************************
   SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

    M5StickC Module
        Axp192      M5Stick         Vin/Vout    Description
        -----------------------------------------------------------------------------
        ACIN:       VBUS-VIN        3.8V-6.3V   Connected to HAT pin
        LDO1:       RTC-VDD         3.3V        RTC power
        LDO2:       LCD-BL-VCC      3.0V        LCD Display backlight
        LDO3:       LCD-LOGIC-VCC   3.0V        LCD Display power
        LDOio0:     MIC-VCC         3.3V        Vmic (GPIO0 set as LDO)
        DCDC1:      VESP-3V3        3.3V        Main rail, when not set chip shuts down.
        DCDC3:                      1.8V        Not used
        ISPOUT      ISPOUT                      Connect to EXTEN regulator to supply 5.0V
        BAT         VABT-Li-ion                 Connect to Lithium battery
        BACKUP      VBAT-RTC                    Connected to RTC battery
        VBUS        VBUS-USB                    Connected to USB connector
        PWROK       ESP32-EN                    Connected to EN pin on ESP32
        N_VBUSEN    GPIO27                      Connected to GPIO27 on ESP32
        PWRON       PWR-KEY                     Connected to PWR-KEY on M5Stick
        IRQ         SYS-INT                     Connected to GPIO35 on ESP32
        EXTEN       EXT-BOOST-EN                Connected to enable on extrn regulator


        Lipo Battery = 80mAh
        AXP192 Pwr Mngmt IC  == SDA=GPIO21, SCL=GPIO22
        MPU6886 - 6 axis IMU == SDA=GPIO21, SCL=GPIO22
        BM8563 - RTC         == SDA=GPIO21, SCL=GPIO22
        Microphone - SPM1423 == SDA=GPIO34, SCL=GPIO0 <--- I2S ?????
        Groove Port          == SDA=GPIO32, SCL=GPIO33
        HAT Env              == SDA=GPIO0,  SCL=GPIO26
*************************************************************************************/
#include "model/M5StickC.h"
#include "model/Axp192Init.h"

#include <smooth/core/logging/log.h>
#include <smooth/application/io/i2c/AxpRegisters.h>
#include <smooth/core/ipc/Publisher.h>

using namespace smooth::core::logging;
using namespace smooth::core::ipc;
using namespace smooth::application::sensor;

namespace redstone
{
    // Class constants
    static const char* TAG = "M5StickC";

    // Constructor
    M5StickC::M5StickC() : i2c0_master(I2C_NUM_0,               // I2C Port 0
                                       GPIO_NUM_22,             // SCL pin
                                       true,                    // SCL internal pullup enabled
                                       GPIO_NUM_21,             // SDA pin
                                       true,                    // SDA internal pullup enabled
                                       400 * 1000)              // clock frequency - 400kHz
    {
    }

    void M5StickC::initialize()
    {
        initialize_axp192();
        initialize_rtc_bm8563();
        initialize_gyro_mpu6886();

        set_time();
        set_alarm();
        bm8563->clear_alarm_flag();
    }

    // Set screen brightness - LDO2 voltage is used to adjust screen brightness
    void M5StickC::set_screen_brightness(uint8_t brightness)
    {
        // max brightness level is 12 = 3.00V
        brightness = brightness > 12 ? 12 : brightness;
        axp192->write_register_bits(AxpRegister::Reg28H_Ldo2_Ldo3_VSet, 0x0F, 4, brightness);
    }

    // Initialize the RTC - BM8563
    void M5StickC::initialize_rtc_bm8563()
    {
        auto device = i2c0_master.create_device<PCF8563>(0x51);   // AXP i2c device address  0x51
        Log::info(TAG, "Scanning for BM8563 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            bm8563_initialized = true;
            bm8563 = std::move(device);
        }

        Log::info(TAG, "BM8563 initialization --- {}", bm8563_initialized ? "Succeeded" : "Failed");
    }

    // Initalize the Gyro - MPU6886
    void M5StickC::initialize_gyro_mpu6886()
    {
    }

    // Initialize the AXP192
    void M5StickC::initialize_axp192()
    {
        auto device = i2c0_master.create_device<AxpPMU>(0x34);   // AXP i2c device address  0x34
        Log::info(TAG, "Scanning for Axp192 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            Log::info(TAG, "Configuring AXP192");
            axp192_initialized = device->write_init_regs(axp192_init_cmds_1.data(), axp192_init_cmds_1.size());

            if (axp192_initialized)
            {
                axp192 = std::move(device);
            }
        }

        Log::info(TAG, "Axp192 initialization --- {}", axp192_initialized ? "Succeeded" : "Failed");
    }

    // Read the measuremnets from AXP192
    void M5StickC::read_axp_measurements()
    {
        float value;

        axp192->get_acin_voltage(value);
        axp_value.set_acin_voltage(value);

        axp192->get_acin_current(value);
        axp_value.set_acin_current(value);

        axp192->get_vbus_voltage(value);
        axp_value.set_vbus_voltage(value);

        axp192->get_vbus_current(value);
        axp_value.set_vbus_current(value);

        axp192->get_ts_voltage(value);
        axp_value.set_ts_pin_voltage(value);

        axp192->get_aps_voltage(value);
        axp_value.set_aps_voltage(value);

        axp192->get_axp_device_temperature(value);
        axp_value.set_axp_device_temperature(value);

        axp192->get_battery_voltage(value);
        axp_value.set_battery_voltage(value);

        axp192->get_battery_charging_current(value);
        axp_value.set_battery_charging_current(value);

        axp192->get_battery_discharging_current(value);
        axp_value.set_battery_discharging_current(value);

        axp192->get_battery_capacity(value);
        axp_value.set_battery_capacity(value);

        axp192->get_battery_power(value);
        axp_value.set_battery_power(value);

        Publisher<AxpValue>::publish(axp_value);
    }

    // Print AXP192 report
    void M5StickC::print_axp192_report()
    {
        float value;
        Log::error(TAG, "===============================================");

        axp192->get_ts_voltage(value);
        Log::warning(TAG, "TS voltage       = {:.2f} V", value);

        axp192->get_acin_voltage(value);
        Log::warning(TAG, "ACIN voltage     = {:.2f} V", value);

        axp192->get_acin_current(value);
        Log::warning(TAG, "ACIN current     = {:.2f} mA", value);

        axp192->get_vbus_voltage(value);
        Log::warning(TAG, "VBUS voltage     = {:.2f} V", value);

        axp192->get_vbus_current(value);
        Log::warning(TAG, "VBUS current     = {:.2f} mA", value);

        axp192->get_battery_voltage(value);
        Log::warning(TAG, "BATT voltage     = {:.2f} V", value);

        axp192->get_aps_voltage(value);
        Log::warning(TAG, "APS voltage      = {:.2f} V", value);

        axp192->get_axp_device_temperature(value);
        Log::warning(TAG, "Device temperature         = {:.2f} C", value);

        axp192->get_battery_charging_current(value);
        Log::warning(TAG, "BATT charging current      = {:.2f} mA", value);

        axp192->get_battery_discharging_current(value);
        Log::warning(TAG, "BATT discharging current   = {:.2f} mA", value);

        axp192->get_battery_power(value);
        Log::warning(TAG, "BATT power                 = {:.2f} mW", value);

        axp192->get_battery_capacity(value);
        Log::warning(TAG, "BATT capacity              = {:.2f} mAh", value);
    }

    // Set RTC time
    void M5StickC::set_time()
    {
        Log::info(TAG, "********** Setting Time *********");
        PCF8563::RtcTime tm;
        tm.seconds = 0;
        tm.minutes = 8;
        tm.hours24 = 13;
        tm.days = 25;
        tm.weekdays = PCF8563::DayOfWeek::Tuesday;
        tm.months = PCF8563::Month::February;
        tm.years = 2020;

        if (!bm8563->set_rtc_time(tm))
        {
            Log::error(TAG, "Error setting RTC time");
        }
    }

    // Get RTC time
    void M5StickC::get_time()
    {
        PCF8563::RtcTime tm;

        if (bm8563->get_rtc_time(tm))
        {
            // Tue 25 Feb 2020 - 1:08:00 PM
            Log::info(TAG, "{} {} {} {} - {} ",
                        PCF8563::DayOfWeekStrings[static_cast<int>(tm.weekdays)],
                        tm.days,
                        PCF8563::MonthStrings[static_cast<int>(tm.months)],
                        tm.years,
                        bm8563->get_12hr_time_string(tm.hours24, tm.minutes, tm.seconds));
        }
        else
        {
            Log::error(TAG, "Error reading RTC time");
        }
    }

    // Set RTC Alarm
    void M5StickC::set_alarm()
    {
        Log::info(TAG, "********** Setting Alarm *********");
        PCF8563::AlarmTime tm;
        tm.minute = 12;
        tm.hour24 = 13;
        tm.day = 25;
        tm.weekday = PCF8563::DayOfWeek::Tuesday;
        tm.ena_alrm_minute = true;
        tm.ena_alrm_hour = false;
        tm.ena_alrm_day = false;
        tm.ena_alrm_weekday = false;

        if (!bm8563->set_alarm_time(tm))
        {
            Log::error(TAG, "Error setting ALARM time");
        }
    }

    // Get RTC Alarm
    void M5StickC::get_alarm()
    {
        PCF8563::AlarmTime tm;

        if (bm8563->get_alarm_time(tm))
        {
            // Tue 25 - 1:12:00 PM
            Log::info(TAG, "{} {} - {} ",
                        PCF8563::DayOfWeekStrings[static_cast<int>(tm.weekday)],
                        tm.day,
                        bm8563->get_12hr_time_string(tm.hour24, tm.minute, 0));
        }
        else
        {
            Log::error(TAG, "Error reading ALARM time");
        }
    }

    // Is RTC alarm active
    bool M5StickC::is_alarm_active()
    {
        bool is_active;
        bm8563->is_alarm_flag_active(is_active);

        return is_active;
    }

    // Clear RTC Alarm active
    void M5StickC::clear_alarm_active()
    {
        bm8563->clear_alarm_flag();
    }
}
