/*
 * Copyright (C) [2024] Bradley James Hammond / Distracted Labs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * For inquiries, please contact martiantux@proton.me | hello@distractedlabs.cc.
 */

#ifndef WiFiManager_h
#define WiFiManager_h

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "util/LEDStatusManager/LEDStatusManager.h"
#include "util/LogManager/LogManager.h"

class WiFiManager {
public:
    static WiFiManager& getInstance() {             // Singleton instance
        static WiFiManager instance;
        return instance;
    }
    
    void setup();                                   // Takes care of anything that needs to be initialized in setup()
    void update();                                  // Periodically called to manage WiFi connection

private:
    WiFiManager();                                  // Private constructor/destructor for singleton
    ~WiFiManager() = default;
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;

    unsigned long lastCheckTime_;                   // Tracks time for managing connection attempts
    bool attemptingConnection_;                     // Indicates if a connection attempt is underway
    int reconnectionAttempts_;                      // Counts the number of reconnection attempts

    unsigned long calculateBackoffDuration();       // Calculates the backoff duration for reconnection attempts
    void attemptConnection();                       // Initiates a WiFi connection attempt
    void rebootDevice();                            // Reboots the device in an attempt to correct WiFi module issues
    void logConnectionStatus();                     // Logs the current WiFi connection status
};

#endif // WiFiManager_h
