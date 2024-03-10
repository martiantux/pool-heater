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

#ifndef LogManager_h
#define LogManager_h

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "util/config.h"
#include "util/TimeManager/TimeManager.h"

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class LogManager {
public:
    static LogManager& getInstance() {        // Singleton instance
        static LogManager instance;
        return instance;
    }

    void setup();
    void update();
    void log(LogLevel level, const String& message);
    String getBuffer();
    String getLastLogs(size_t lastCount);
    void clearBuffer();

private:
    LogManager();                           // Private constructor/destructor for singleton
    ~LogManager() = default;
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    LogLevel currentLogLevel_;
    JsonDocument logBuffer_;

    void sendToDiscord(const String& message);
};

#endif // LogManager_h
