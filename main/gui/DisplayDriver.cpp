/****************************************************************************************
 * DisplayDriver.h - A LittlevGL Display Driver for ST7735S - Resolution 80x160
 *
 * Created on Dec. 03, 2019
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
#include "gui/DisplayDriver.h"
#include <esp_freertos_hooks.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::io::spi;
using namespace smooth::application::display;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace redstone
{
    // Class Constants
    static const char* TAG = "DisplayDriver";

    // Constructor
    DisplayDriver::DisplayDriver() :
            spi_host(HSPI_HOST),            // Use HSPI as host

            spi_master(
                spi_host,                   // host HSPI
                DMA_1,                      // use DMA
                GPIO_NUM_15,                // mosi gpio pin
                GPIO_NUM_NC,                // miso not available
                GPIO_NUM_13,                // clock gpio pin
                MAX_DMA_LEN)                // max transfer size
    {
    }

    // Initialize the display driver
    bool DisplayDriver::initialize()
    {
        Log::info(TAG, "Initializing Lvgl Display Driver ........");

        display_initialized = init_display();

        if (display_initialized)
        {
            // Set the screen orientation
            set_screen_rotation();

            // initialize LittlevGL graphics library
            lv_init();

            // The video_display_buffer1 is used by LittlevGL to draw screen content
            // Verify that video_display_buffer1 has been created.
            if (video_display_buffer1.is_buffer_allocated())
            {
                // initialize a display buffer
                vdb1 = reinterpret_cast<lv_color1_t*>(video_display_buffer1.data());
                lv_disp_buf_init(&disp_buf, vdb1, NULL, MAX_DMA_LEN / COLOR_SIZE);

                // initialize and register a display driver
                lv_disp_drv_init(&disp_drv);
                disp_drv.buffer = &disp_buf;
                disp_drv.flush_cb = display_flush_cb;
                disp_drv.user_data = this;
                lv_disp_drv_register(&disp_drv);
            }
            else
            {
                display_initialized = false;
            }
        }

        return display_initialized;
    }

    // Initialize the lcd display
    bool DisplayDriver::init_display()
    {
        auto device = spi_master.create_device<DisplaySpi>(
                        GPIO_NUM_5,             // chip select gpio pin
                        GPIO_NUM_23,            // data command gpio pin
                        0,                      // spi command_bits
                        0,                      // spi address_bits,
                        0,                      // bits_between_address_and_data_phase,
                        0,                      // spi_mode = 0,
                        128,                    // spi positive_duty_cycle,
                        0,                      // spi cs_ena_posttrans,
                        SPI_MASTER_FREQ_26M,    // spi-sck = 26MHz, actual = 26.67MHz
                        0,                      // full duplex (4-wire)
                        7,                      // queue_size,
                        true,                   // use pre-trans callback
                        true);                  // use post-trans callback

        bool res = device->init(spi_host);

        if (res)
        {
            Log::info(TAG, "Initializing SPI Device: DisplaySpi");
            device->add_reset_pin(std::make_unique<DisplayPin>(GPIO_NUM_18, false, false, false));
            device->sw_reset(milliseconds(10));
            res &= device->send_init_cmds(init_cmds_R_part1.data(), init_cmds_R_part1.size());
            res &= device->send_init_cmds(init_cmds_R_grn_tab_part2.data(), init_cmds_R_grn_tab_part2.size());
            res &= device->send_cmd(LcdCmd::INVON);  // display inversion on
            res &= device->send_init_cmds(init_cmds_R_part3.data(), init_cmds_R_part3.size());

            display = std::move(device);
        }
        else
        {
            Log::error(TAG, "Initializing of SPI Device: DisplaySpi --- FAILED");
        }

        return res;
    }

    // Set screen rotation
    void DisplayDriver::set_screen_rotation()
    {
        bool res = true;

        // The default screen oreintation via init_cmds is portrait but check
        // the lv_config file for the screen oreintation used for this project
        if (LV_VER_RES_MAX > LV_HOR_RES_MAX)
        {
            // Portrait
            res = display->set_madctl(0xC8);
        }
        else
        {
            // Landscape
            res = display->set_madctl(0xA8);
        }

        if (!res)
        {
            Log::error(TAG, "Setting screen rotation --- FAILED");
        }
    }

    // A class instance callback to flush the display buffer and thereby write colors to screen
    void DisplayDriver::display_drv_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
    {
        uint32_t x1 = area->x1;
        uint32_t y1 = area->y1;
        uint32_t x2 = area->x2;
        uint32_t y2 = area->y2;

        // The M5StickC (ST7735S) needs offsets applied otherwise display is not shown correctly on screen
        if (LV_VER_RES_MAX > LV_HOR_RES_MAX) // portrait
        {
            x1 += offsets_green_tab_160x80.col_offset;
            y1 += offsets_green_tab_160x80.row_offset;
            x2 += offsets_green_tab_160x80.col_offset;
            y2 += offsets_green_tab_160x80.row_offset;
        }
        else    // landscape
        {
            x1 += offsets_green_tab_160x80.row_offset;
            y1 += offsets_green_tab_160x80.col_offset;
            x2 += offsets_green_tab_160x80.row_offset;
            y2 += offsets_green_tab_160x80.col_offset;
        }

        uint32_t number_of_bytes_to_flush = (x2 - x1 + 1) * (y2 - y1 + 1) * COLOR_SIZE;
        uint32_t number_of_dma_blocks_with_complete_lines_to_send = number_of_bytes_to_flush / MAX_DMA_LEN;
        uint32_t number_of_bytes_in_not_complete_lines_to_send = number_of_bytes_to_flush % MAX_DMA_LEN;

        uint32_t start_row = y1;
        uint32_t end_row = y1 + LINES_TO_SEND - 1;

        // Drawing area that has a height of LINES_TO_SEND
        while (number_of_dma_blocks_with_complete_lines_to_send--)
        {
            display->send_lines(x1, start_row, x2, end_row, reinterpret_cast<uint8_t*>(color_map), MAX_DMA_LEN);
            display->wait_for_send_lines_to_finish();

            // color_map is pointer to type lv_color_t were the data type is based on color size so the
            // color_map pointer may have a data type of uint8_t or uint16_t or uint32_t.  MAX_DMA_LEN is
            // a number based on uint8_t so we need to divide MAX_DMA_LEN by the color size to increment
            // color_map pointer correctly
            color_map += MAX_DMA_LEN / COLOR_SIZE;

            // update start_row and end_row since we have sent a quantity of LINES_TO_SEND rows
            start_row = end_row + 1;
            end_row = start_row + LINES_TO_SEND - 1;
        }

        // Drawing area that has a height of less than LINE_TO_SEND
        if (number_of_bytes_in_not_complete_lines_to_send)
        {
            end_row = y2;
            display->send_lines(x1, start_row, x2, end_row,
                                reinterpret_cast<uint8_t*>(color_map),
                                number_of_bytes_in_not_complete_lines_to_send);

            display->wait_for_send_lines_to_finish();
        }

        // Inform the lvgl graphics library that we are ready for flushing the VDB buffer
        lv_disp_t* disp = lv_refr_get_disp_refreshing();
        lv_disp_flush_ready(&disp->driver);
    }

    // The "C" style callback required by LittlevGL
    void IRAM_ATTR DisplayDriver::display_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
    {
        DisplayDriver* driver = reinterpret_cast<DisplayDriver*>(drv->user_data);
        driver->display_drv_flush(drv, area, color_map);
    }
}
