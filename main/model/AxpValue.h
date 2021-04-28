/****************************************************************************************
 * AxpValue.h - This class instance is published and received by subscribers
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

#include <string>
#include <math.h>

namespace redstone
{
    class AxpValue
    {
        public:
            AxpValue() {}

            /// Set the ACIN voltage
            /// \param value The ACIN voltage
            void set_acin_voltage(float value)
            {
                acin_voltage = value;
            }

            /// Get the ACIN voltage in volts
            /// \param return Return the ACIN voltage
            float get_acin_voltage() const
            {
                return acin_voltage;
            }

            /// Set the VBUS voltage
            /// \param value The VBUS voltage
            void set_vbus_voltage(float value)
            {
                vbus_voltage = value;
            }

            /// Get the VBUS voltage in volts
            /// \param return Return the VBUS voltage
            float get_vbus_voltage() const
            {
                return vbus_voltage;
            }

            /// Set the battery voltage
            /// \param value The battery voltage
            void set_battery_voltage(float value)
            {
                battery_voltage = value;
            }

            /// Get the battery voltage in volts
            /// \param return Return the batteryc voltage
            float get_battery_voltage() const
            {
                return battery_voltage;
            }

            /// Set the TS pin voltage
            /// \param value The TS pin voltage
            void set_ts_pin_voltage(float value)
            {
                ts_pin_voltage = value;
            }

            /// Get the TS pin voltage in volts
            /// \param return Return the TS pin voltage
            float get_ts_pin_voltage() const
            {
                return ts_pin_voltage;
            }

            /// Set the APS voltage
            /// \param value The APS voltage
            void set_aps_voltage(float value)
            {
                aps_voltage = value;
            }

            /// Get the APS voltage in volts
            /// \param return Return the APS voltage
            float get_aps_voltage() const
            {
                return aps_voltage;
            }

            /// Set the Axp device temperature
            /// \param value The Axp device temperature
            void set_axp_device_temperature(float value)
            {
                axp_device_temperature = value;
            }

            /// Get the Axp device temperature in degree C
            /// \param return Return the Axp device temperature
            float get_axp_device_temperature() const
            {
                return axp_device_temperature;
            }

            /// Set the ACIN current
            /// \param value The ACIN current
            void set_acin_current(float value)
            {
                acin_current = value;
            }

            /// Get the ACIN current in mA
            /// \param return Return the ACIN current
            float get_acin_current() const
            {
                return acin_current;
            }
            
            /// Set the VBUS current
            /// \param value The VBUS current
            void set_vbus_current(float value)
            {
                vbus_current = value;
            }

            /// Get the VBUS current in mA
            /// \param return Return the VBUS current
            float get_vbus_current() const
            {
                return vbus_current;
            }

            /// Set the battery charging current
            /// \param value The battery charging current
            void set_battery_charging_current(float value)
            {
                battery_charging_current = value;
            }

            /// Get the battery charging current in mA
            /// \param return Return the battery charging current
            float get_battery_charging_current() const
            {
                return battery_charging_current;
            }

            /// Set the battery discharging current
            /// \param value The battery discharging current
            void set_battery_discharging_current(float value)
            {
                battery_discharging_current = value;
            }

            /// Get the battery discharging current in mA
            /// \param return Return the battery discharging current
            float get_battery_discharging_current() const
            {
                return battery_discharging_current;
            }
            
            /// Set the battery power
            /// \param value The battery power
            void set_battery_power(float value)
            {
                battery_power = value;
            }

            /// Get the battery power in mW
            /// \param return Return the battery power
            float get_battery_power() const
            {
                return battery_power;
            }

            /// Set the battery capacity
            /// \param value The battery capacity
            void set_battery_capacity(float value)
            {
                battery_capacity = value;
            }

            /// Get the battery capacity in mAh
            /// \param return Return the battery capacity
            float get_battery_capacity() const
            {
                return battery_capacity;
            }

        private:
            float acin_voltage;
            float acin_current;
            float vbus_voltage;
            float vbus_current;
            float battery_voltage;
            float aps_voltage;
            float ts_pin_voltage;
            float axp_device_temperature;
            float battery_charging_current;
            float battery_discharging_current;
            float battery_capacity;
            float battery_power;
    };
}
