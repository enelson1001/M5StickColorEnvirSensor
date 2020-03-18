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
//  I (7459495) MemStat: INTERNAL |      150332 |         113804 |       149516 |      200752 |         113804 |       199928
//  I (7459506) MemStat:      DMA |      150248 |         113804 |       149516 |      150248 |         113804 |       149516
//  I (7459518) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
//  I (7459530) MemStat: 
//  I (7459532) MemStat:             Name |      Stack |  Min free stack |  Max used stack
//  I (7459541) MemStat:         LvglTask |       4096 |             400 |            3696
//  I (7459550) MemStat:        HwBtnTask |       3000 |             924 |            2076
//  I (7459558) MemStat: SocketDispatcher |      20480 |           18396 |            2084
//  I (7459567) MemStat:         MainTask |      16384 |           12556 |            3828
//
// Esp32-IDF version: v4.0-beta2
// Toolchain version: xtensa-esp32-elf-gcc (crosstool-NG esp32-2019r1) 8.2.0
// Lvgl version: v6.1.2 - SHA1: 2ead4959
// Smooth version: master SHA1: b4bf80b4
// Bin file size: 1,173,424 bytes
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
