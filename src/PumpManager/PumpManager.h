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

#ifndef PumpManager_h
#define PumpManager_h

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "util/config.h"
#include "util/LogManager/LogManager.h"
#include "util/TimeManager/TimeManager.h"

class PumpManager {
public:
    static PumpManager& getInstance() {
        static PumpManager instance;       // Singleton instance
        return instance;
    }

    void setup();
    void update();
    static void pulseCounter();     // Interrupt service routine for pulse counting

private:
    PumpManager();                         // Private constructor/destructor for singleton
    ~PumpManager() = default;
    PumpManager(const PumpManager&) = delete;
    PumpManager& operator=(const PumpManager&) = delete;

    enum State { INITIALIZING, SENSORS_STABILIZING, ACTIVE, HIBERNATING, MAINTENANCE } pumpState = INITIALIZING;

    OneWire oneWire_;
    DallasTemperature sensors_;
    WebServer server_;
    HTTPUpdateServer httpUpdater_;

    static volatile byte pulseCount;

    uint8_t inputTempAddr_[8];
    uint8_t outputTempAddr_[8];
    uint8_t enclosureTempAddr_[8];

    long lastTempPoll_;
    unsigned long lastPoolTempTime_;
    float lastPoolTemp_;
    float inputTemp_;
    float outputTemp_;
    float enclosureTemp_;
    long lastPumpUpdate_;

    unsigned long stabilityStartTime_;          // millis to track how long waiting for stability
    float energyCapture_;                       // The current energy being captured, in watts
    float lastEnergyInsufficient_;              // Unix stamp of the last time the delta was too low
    unsigned long lastHibernationTime_;         // millis to track time in hibernation
    unsigned long lastMaintenanceToggle_;       // unix stamp to track when it has been far enough from maintenance toggle to operate

    float flowRate_;
    long currentFlowMillis_;
    long previousFlowMillis_;
    int flowInterval_;
    float calibrationFactor_;
    byte pulse1Sec_;
    unsigned int flowMilliLitres_;
    unsigned long totalMilliLitres_;

    void pumpControlUpdater();
    void handleStyle();
    void handleScript();
    void handleRoot();
    void handleData();
    void handleMaintenance();
    void handleLogs();
    void handleUpdate();
    void handleNotFound();

    String getUptime();
    String formatPower(double powerInWatts);
    String pumpStateToString(State pumpState);
    String calculatePoolLastTime();
};

#endif // PumpManager_h
