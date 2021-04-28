/****************************************************************************************
 * HwBtnEvent.h - This header file contains class events for hardware buttons that
 * subscribers listen for.
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
#pragma once

namespace redstone
{
    class HwBtnEvent
    {
        public:
            /// Constructor
            HwBtnEvent() {}

            /// Get button id
            /// \param return Return the button id
            virtual int get_button_id() const
            {
                return btn_id;
            }

            // Set button id
            /// \param id The button id
            virtual void set_button_id(int id)
            {
                btn_id = id;
            }

        private:
            int btn_id;
    };

    /// Hardware button NEXT PRESSED event
    class HwBtnNextPressed : public HwBtnEvent
    {
        public:
            HwBtnNextPressed() {}
    };

    /// Hardware button NEXT RELEASED event
    class HwBtnNextReleased : public HwBtnEvent
    {
        public:
            HwBtnNextReleased() {}
    };

    /// Hardware button PREV PRESSED event
    class HwBtnPrevPressed : public HwBtnEvent
    {
        public:
            HwBtnPrevPressed() {}
    };

    /// Hardware button PREV RELEASED event
    class HwBtnPrevReleased : public HwBtnEvent
    {
        public:
            HwBtnPrevReleased() {}
    };
}
