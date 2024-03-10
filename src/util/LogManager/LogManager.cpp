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

#include "util/LogManager/LogManager.h"

LogManager::LogManager()
    : currentLogLevel_(INFO) {}


void LogManager::setup() {
    // Setup stuffs
}

void LogManager::update() {
    // Placeholder for future needs
}

const char* logLevelToString(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void LogManager::log(LogLevel level, const String& message) {
    // Get current log time from TimeManager
    String logTime = TimeManager::getInstance().getLogTime();

    // Format the log message for serial/Discord output
    String formattedMessage = String(logTime + " [" + logLevelToString(level) + "] " + message);

    // Log formatted message to serial
    Serial.println(formattedMessage);

    // Create a JSON object for the log entry
    JsonDocument logEntry;
    logEntry["time"] = logTime;
    logEntry["level"] = logLevelToString(level);
    logEntry["message"] = message;

    // Add the log entry to the log buffer array
    logBuffer_.add(logEntry);

    // Limit the number of log entries to MAX_LOG_ENTRIES
    if (logBuffer_.size() > MAX_BUFFER_SIZE) {
        logBuffer_.remove(0); // Remove the oldest entry
    }
}

String LogManager::getBuffer() {
    String output;
    JsonArray tempBuffer = logBuffer_.to<JsonArray>(); // Create a temporary JsonArray
    tempBuffer.clear(); // Ensure the temporary buffer is empty

    // Iterate over the original logBuffer in reverse and add each element to the temporary buffer
    for (int i = logBuffer_.size() - 1; i >= 0; --i) {
        tempBuffer.add(logBuffer_[i]);
    }

    serializeJson(tempBuffer, output); // Serialize the temporary buffer, which is now in reverse order
    return output;
}

String LogManager::getLastLogs(size_t lastCount) {
    String output;
    JsonDocument lastLogs;
    size_t endIndex = logBuffer_.size(); // Determine the end index
    size_t startIndex = endIndex > lastCount ? endIndex - lastCount : 0; // Calculate the start index based on lastCount

    // Iterate in reverse within the determined range and add each element to lastLogs
    for (size_t i = endIndex; i > startIndex; --i) {
        lastLogs.add(logBuffer_[i - 1]); // Adjust index as we're decrementing i
    }

    serializeJson(lastLogs, output); // Serialize the lastLogs, which now contains the last logs in reverse order
    return output;
}

void LogManager::clearBuffer() {
    logBuffer_.clear();
}

void LogManager::sendToDiscord(const String& message) {
    // Implement later, must be non-blocking
}
