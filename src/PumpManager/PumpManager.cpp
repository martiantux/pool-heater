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

#include "PumpManager/PumpManager.h"

PumpManager::PumpManager() 
    : oneWire_(ONE_WIRE_BUS_PIN), 
        sensors_(&oneWire_), 
        server_(80),
        httpUpdater_(),
        inputTempAddr_({ 0x28, 0x37, 0xB0, 0x57, 0x04, 0xE1, 0x3C, 0x55 }),
        outputTempAddr_({ 0x28, 0x43, 0xE7, 0x57, 0x04, 0xE1, 0x3C, 0xD5 }),
        enclosureTempAddr_({ 0x28, 0xAF, 0x1A, 0x57, 0x04, 0xE1, 0x3C, 0xCB }),
        lastTempPoll_(0),
        lastPoolTempTime_(0),
        lastPoolTemp_(0),
        inputTemp_(0),
        outputTemp_(0),
        enclosureTemp_(0),
        lastPumpUpdate_(0),
        stabilityStartTime_(0),
        energyCapture_(0),
        lastEnergyInsufficient_(0),
        lastHibernationTime_(0),
        lastMaintenanceToggle_(0),
        flowRate_(0),
        currentFlowMillis_(0),
        previousFlowMillis_(0),
        flowInterval_(1000),
        calibrationFactor_(7.319),
        pulse1Sec_(0),
        flowMilliLitres_(0),
        totalMilliLitres_(0) {}

volatile byte PumpManager::pulseCount = 0;


String PumpManager::calculatePoolLastTime() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastPoolTempTime_;

    unsigned long minutes = (elapsedTime / 1000) / 60;

    // Format counter string
    return String(minutes) + " mins ago";
}

String PumpManager::pumpStateToString(State pumpState) {
    switch (pumpState) {
        case INITIALIZING: return "Initializing";
        case SENSORS_STABILIZING: return "Sensors stabilizing";
        case ACTIVE: return "Active";
        case HIBERNATING: return "Hibernating";
        case MAINTENANCE: return "MAINTENANCE";
        default: return "UNKNOWN STATE";
    }
}

void PumpManager::pumpControlUpdater() {

    unsigned long currentMillis = millis();

    // Calculate energy capture value in watts
    float tempDelta = outputTemp_ - inputTemp_;             // Delta/difference between input/output
    float flowJoulesPerDegree = (flowRate_ / 60) * 4180;    // Joules per litre/sec
    energyCapture_ = flowJoulesPerDegree * tempDelta;       // Multiply per degrees of temps delta

    // TODO: Maintain a total energy captured last 24hrs, 72hrs, and week.

    switch (pumpState) {

        case INITIALIZING:
            stabilityStartTime_ = currentMillis;
            lastHibernationTime_ = currentMillis;
            lastMaintenanceToggle_ = currentMillis - MAINTENANCE_PERIOD;
            lastEnergyInsufficient_ = currentMillis - HIBERNATION_TRIGGER_DELAY;

            digitalWrite(PUMP_CONTROL_PIN, HIGH); // Turn the pump on to cycle the system
            LogManager::getInstance().log(INFO, "Pump controller initialized, sensors stabilizing");
            pumpState = SENSORS_STABILIZING;
            break;


        case SENSORS_STABILIZING:
            lastPoolTempTime_ = currentMillis - (1000 * 3600);    // For now just hold the timer at an hour old

            if (currentMillis - stabilityStartTime_ > SENSOR_STABILITY_DELAY) {
                LogManager::getInstance().log(INFO, "Sensors stabilized");
                pumpState = ACTIVE;
            }
            break; // Do nothing, keep waiting for sensor values to be considered stable


        case ACTIVE:
            // Update the last known pool temp
            lastPoolTemp_ = inputTemp_;
            lastPoolTempTime_ = currentMillis;

            // Temp target check
            if (inputTemp_ > TARGET_TEMP) {
                pumpState = HIBERNATING;
                LogManager::getInstance().log(INFO, "Input temp > target temp, hibernating");
                digitalWrite(PUMP_CONTROL_PIN, LOW);
                lastHibernationTime_ = currentMillis;
            }
            // Energy delta check
            else if (energyCapture_ < ENERGY_CAPTURE_THRESHOLD && (currentMillis - lastEnergyInsufficient_) > HIBERNATION_TRIGGER_DELAY) {
                pumpState = HIBERNATING;
                LogManager::getInstance().log(INFO, "Energy delta insufficient > trigger period, hibernating");
                digitalWrite(PUMP_CONTROL_PIN, LOW);
                lastHibernationTime_ = currentMillis;
            }
            else { // Reset the hibernation trigger if the delta goes positive again
                lastEnergyInsufficient_ = currentMillis - HIBERNATION_TRIGGER_DELAY;
            }
            break;


        case HIBERNATING:
            // Check if hibernation timer is up and kick back to sensors stabilizing if so
            if (currentMillis - lastHibernationTime_ > HIBERNATION_PERIOD) {
                stabilityStartTime_ = currentMillis;
                digitalWrite(PUMP_CONTROL_PIN, HIGH);
                pumpState = SENSORS_STABILIZING;
                LogManager::getInstance().log(INFO, "Hibernation period reached, cycling system");
            }
            else {
                // Sleepy time
            }
            break;


        case MAINTENANCE:
            // Check if maintenance timer has expired and go back to init if so
            break;

    }
}

String PumpManager::getUptime() {
    unsigned long uptimeMillis = millis();

    // Calculate days, hours, minutes, and seconds
    unsigned long seconds = uptimeMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;

    // Calculate remaining hours, minutes, and seconds
    hours = hours % 24;
    minutes = minutes % 60;
    seconds = seconds % 60;

    // Format the result as a string
    String formattedUptime = String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";

    return formattedUptime;
}

String PumpManager::formatPower(double powerInWatts) {
    if (powerInWatts < 1000) {
        return String(round(powerInWatts)) + " W";
    } else {
        double powerInKW = powerInWatts / 1000.0;
        return String(powerInKW, 2) + " kW"; // Round to two decimal places
    }
}

void PumpManager::pulseCounter() {
    pulseCount++;
}

void PumpManager::handleStyle() {
    File file = SPIFFS.open("/style.css", "r");
    if (!file) {
        LogManager::getInstance().log(ERROR, "Failed to open files for web /style.css");
        return;
    }

    server_.streamFile(file, "text/css");
    file.close();
}

void PumpManager::handleScript() {
    File file = SPIFFS.open("/script.js", "r");
    if (!file) {
        LogManager::getInstance().log(ERROR, "Failed to open files for web /script.js");
        return;
    }

    server_.streamFile(file, "application/javascript");
    file.close();
}

void PumpManager::handleRoot() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        LogManager::getInstance().log(ERROR, "Failed to open files for web /");
        return;
    }

    server_.streamFile(file, "text/html");
    file.close();
}

void PumpManager::handleMaintenance() {
    File file = SPIFFS.open("/maintenance.html", "r");
    if (!file) {
        LogManager::getInstance().log(ERROR, "Failed to open files for web /");
        return;
    }

    server_.streamFile(file, "text/html");
    file.close();
}

void PumpManager::handleLogs() {
    String logs = LogManager::getInstance().getLastLogs(30);
    JsonDocument doc;
    deserializeJson(doc, logs);

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server_.send(200, "application/json", jsonResponse);
}

void PumpManager::handleNotFound() {
    File file = SPIFFS.open("/not-found.html", "r");
    if (!file) {
        LogManager::getInstance().log(ERROR, "Failed to open files for web not-found");
        return;
    }

    server_.streamFile(file, "text/html");
    file.close();
}

void PumpManager::handleUpdate() {
    HTTPUpload& upload = server_.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());

        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            //logDebugMessage("Update Success: Rebooting...");
        } else {
            Update.printError(Serial);
            //logDebugMessage("Update failed!");
        }
    }
}

void PumpManager::handleData() {

    String jsonString = "{"
         "\"controllerUptime\":\"" + String(getUptime()) + "\","
        "\"firmwareVersion\":\""  + String(FIRMWARE_VERSION) + "\","
        "\"enclosureTemp\":\""    + String(enclosureTemp_) + " C\","
        "\"localTime\":\""        + TimeManager::getInstance().getLongDate() + "\","
        "\"pumpStatus\":\""       + pumpStateToString(pumpState) + "\","
        "\"targetTemp\":\""       + String(TARGET_TEMP) + " C\","
        "\"poolTemp\":\""         + String(lastPoolTemp_) + " C\","
        "\"poolTempTime\":\""     + calculatePoolLastTime() + "\","
        "\"inputTemp\":\""        + String(inputTemp_) + " C\","
        "\"outputTemp\":\""       + String(outputTemp_) + " C\","
        "\"flowRate\":\""         + String(flowRate_) + " L/min\","
        "\"energyCapture\":\""    + formatPower(energyCapture_) + "\""
        + "}";

    server_.send(200, "application/json", jsonString);
}

void PumpManager::setup() {

    if (!SPIFFS.begin(true)) {
        LogManager::getInstance().log(ERROR, "Failed to mount SPIFFS");
    }

    sensors_.begin();

    // Pump GPIO setup
    pinMode(PUMP_CONTROL_PIN, OUTPUT);
    digitalWrite(PUMP_CONTROL_PIN, LOW); // Pump OFF initially
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), PumpManager::pulseCounter, RISING);

    // Web setup
    httpUpdater_.setup(&server_);
    server_.on("/style.css", [this](){ handleStyle(); });
    server_.on("/script.js", [this](){ handleScript(); });
    server_.on("/", [this](){ handleRoot(); });
    server_.on("/maintenance", [this](){ handleMaintenance(); });
    server_.on("/api/logs", [this](){ handleLogs(); });
    server_.on("/update", HTTP_POST, [this](){ server_.send(200); }, [this](){ handleUpdate(); });
    server_.on("/api/data", HTTP_GET, [this](){ handleData(); });
    server_.onNotFound([this](){ handleNotFound(); });
    server_.begin();
    LogManager::getInstance().log(INFO, "HTTP server started");

}

void PumpManager::update() {

    unsigned long currentMillis = millis();

    // Temp sensor updates
    if (lastTempPoll_ == 0 || currentMillis - lastTempPoll_ >= TEMP_POLL_INTERVAL) {
        sensors_.requestTemperatures();

        if (sensors_.getTempC(inputTempAddr_) != -127) { inputTemp_ = sensors_.getTempC(inputTempAddr_); }
        if (sensors_.getTempC(outputTempAddr_) != -127) { outputTemp_ = sensors_.getTempC(outputTempAddr_); }
        if (sensors_.getTempC(enclosureTempAddr_) != -127) { enclosureTemp_ = sensors_.getTempC(enclosureTempAddr_); }

        lastTempPoll_ = currentMillis;
    }

    // Flow meter update
    currentFlowMillis_ = millis();
    if (currentFlowMillis_ - previousFlowMillis_ > flowInterval_) {

        pulse1Sec_ = pulseCount;
        pulseCount = 0;

        // Because this loop may not complete in exactly 1 second intervals, calculate
        // the number of milliseconds that have passed since the last execution and use
        // that to scale the output. Also, apply the calibrationFactor to scale the output
        // based on the number of pulses per second per units of measure (litres/minute in
        // this case) coming from the sensor.
        flowRate_ = ((1000.0 / (millis() - previousFlowMillis_)) * pulse1Sec_) / calibrationFactor_;
        previousFlowMillis_ = millis();

        // Divide the flow rate in litres/minute by 60 to determine how many litres have
        // passed through the sensor in this 1 second interval, then multiply by 1000 to
        // convert to millilitres.
        flowMilliLitres_ = (flowRate_ / 60) * 1000;

        // Add the millilitres passed in this second to the cumulative total
        totalMilliLitres_ += flowMilliLitres_;
    }

    // Pump updater
    if (lastPumpUpdate_ == 0 || currentMillis - lastPumpUpdate_ >= PUMP_UPDATE_INTERVAL) {
        pumpControlUpdater();
        lastPumpUpdate_ = currentMillis;
    }

    server_.handleClient(); // Handle webserver
}
