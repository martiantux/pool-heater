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
 * For inquiries, please contact hello@distractedlabs.cc.
 */

#include <Arduino.h>
#include "util/LogManager/LogManager.h"
#include "util/LEDStatusManager/LEDStatusManager.h"
#include "util/WiFiManager/WiFiManager.h"
#include "util/TimeManager/TimeManager.h"
#include "PumpManager/PumpManager.h"

void setup() {
    Serial.begin(115200);

    LEDStatusManager::getInstance().setup();
    LogManager::getInstance().setup();
    WiFiManager::getInstance().setup();
    TimeManager::getInstance().setup();
    PumpManager::getInstance().setup();

    LEDStatusManager::getInstance().setStatus(0);
    LogManager::getInstance().log(INFO, "Setup finished");
}

void loop() {
    LEDStatusManager::getInstance().update();
    LogManager::getInstance().update();
    WiFiManager::getInstance().update();
    TimeManager::getInstance().update();
    PumpManager::getInstance().update();
}
