/****************************************************************************************
 * App.cpp - The Application class
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

//*****************************************************************************************************************
// Typical output on M5Stick-Color
//
//  W (7459471) APP: ============ M5StickColorEnvir Tick  =============
//  I (7459474) M5StickC: Tue 25 Feb 2020 - 3:12:18 PM 
//  I (7459475) M5StickC: Tue 25 - 1:12:00 PM 
//  E (7459478) APP: The Alarm Active Count = 3
//  I (7459484) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
//  I (7459495) MemStat: INTERNAL |      173328 |          65536 |       172564 |      201776 |          65536 |       201000
//  I (7459506) MemStat:      DMA |      173412 |          65536 |       172564 |      173412 |          65536 |       172564
//  I (7459518) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
//  I (7459530) MemStat: 
//  I (7459532) MemStat:             Name |      Stack |  Min free stack |  Max used stack
//  I (7459541) MemStat:         LvglTask |       4096 |             336 |            3760
//  I (7459550) MemStat:        HwBtnTask |       3000 |             596 |            2404
//  I (7459558) MemStat: SocketDispatcher |      20480 |           18080 |            2400
//  I (7459567) MemStat:         MainTask |      16384 |           12276 |            4108
//
// Esp32-IDF version: v4.3-beta3 - commit e9cf9e2 - April 14, 2021
// Toolchain version: esp-2020r3-8.4.0/xtensa-esp32-elf
// Lvgl version:  v7.11.0 - commit: ec9de51, March, 2021
// Smooth version: master - commit: 5578b8b, April 15, 2021
// Bin file size: 1,368,000 bytes
//******************************************************************************************************************
#include "App.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>

using namespace smooth::core;
using namespace std::chrono;

namespace redstone
{
    // Class Constants
    static const char* TAG = "APP";

    // Constructor
    App::App() : Application(APPLICATION_BASE_PRIO, seconds(1))
    {
    }

    // Initialize the application
    void App::init()
    {
        Log::warning(TAG, "============ Starting APP  ===========");
        Application::init();
        m5stickC.initialize();
        env_hat.initialize();
        hw_btn_task.start();
        lvgl_task.start();
        
    }

    // Tick event happens every 1 second
    void App::tick()
    {
        if (!heap_caps_check_integrity_all(true))
        {
            Log::error(TAG, "========= Heap Corrupted  ===========");
        }

        tick_count += 1;

        perform_01_second_tasks();
        
        if (tick_count == 60)
        {
            tick_count = 0;
            perform_60_second_tasks();
        }
    }

    void App::perform_01_second_tasks()
    {
        env_hat.read_measurements();
        m5stickC.read_axp_measurements();
    }

    void App::perform_60_second_tasks()
    {
        Log::warning(TAG, "============ M5StickColorEnvir Tick  =============");
        m5stickC.get_time();
        m5stickC.get_alarm();

        if (m5stickC.is_alarm_active())
        {
            alarm_active_count += 1;
            Log::error(TAG, "The Alarm Active Count = {}", alarm_active_count);
            m5stickC.clear_alarm_active();
        }

        SystemStatistics::instance().dump();
        //m5stickC.print_axp192_report();
    }
}
