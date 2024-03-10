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

#include "util/TimeManager/TimeManager.h"

TimeManager::TimeManager() 
    : ntpUDP_(), 
      timeClient_(ntpUDP_, NTP_SERVER, TIMEZONE_OFFSET, updateInterval_), 
      updateInterval_((1000 * 60) * 120), // 120 minutes in milliseconds
      lastSyncTime_(0) {}
      

void TimeManager::setup() {
    timeClient_.begin();
}

void TimeManager::update() {
    unsigned long currentMillis = millis();
    if (lastSyncTime_ == 0 || currentMillis - lastSyncTime_ >= updateInterval_) {
        if (WiFi.status() == WL_CONNECTED) {
            syncTime();
            lastSyncTime_ = currentMillis;
        }
    }

    // Any other time keeping update code
}

void TimeManager::syncTime() {
    LogManager::getInstance().log(INFO, "Attempting time synchronization...");
    if (!timeClient_.forceUpdate()) {
        LogManager::getInstance().log(WARN, "Failed to sync time with NTP server.");
    }
    else {
        LogManager::getInstance().log(INFO, "Time synchronization sucessful.");
    }
}

unsigned long TimeManager::getCurrentTimestamp() {
    return timeClient_.getEpochTime();
}

String TimeManager::getLogTime() {
    // TODO: This should provide millis until we have time sync, then correct unix timestamps.
    // TODO: Might be good to log both millis() AND unix stamp.

    return String(millis());
}

String TimeManager::getLongDate() {
    return "Unavailable";//timeClient_.getFormattedTime() + " - " + timeClient_.getDay() + ", " + timeClient_.getFormattedTime();
}

String TimeManager::getShortDate() {
    return timeClient_.getFormattedTime() + " - " + timeClient_.getDay();
}

String TimeManager::getTimeString() {
    return timeClient_.getFormattedTime();
}