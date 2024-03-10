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

#ifndef TimeManager_h
#define TimeManager_h

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "util/LogManager/LogManager.h"

class TimeManager {
public:
    static TimeManager& getInstance() {             // Singleton instance
        static TimeManager instance;
        return instance;
    }

    void setup();
    void update();

    String getLogTime();
    String getLongDate();
    String getShortDate();
    String getTimeString();
    unsigned long getCurrentTimestamp();

private:
    TimeManager();                                  // Private constructor/destructor for singleton
    ~TimeManager() = default;
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

    WiFiUDP ntpUDP_;
    NTPClient timeClient_;
    const long updateInterval_; // Sync interval in milliseconds
    unsigned long lastSyncTime_;

    void syncTime();
};

#endif // TimeManager_h