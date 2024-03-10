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

#include "util/LEDStatusManager/LEDStatusManager.h"

LEDStatusManager::LEDStatusManager()
    : status_(0),
    lastBlinkTime_(0),
    blinkCount_(0),
    maxBlinks_(0),
    ledState_(false) {}


void LEDStatusManager::setup() {
    pinMode(INDICATOR_LED_PIN, OUTPUT);
}

void LEDStatusManager::update() {
    unsigned long currentMillis = millis();

    // Check for pause between sets of blinks
    if (status_ != 0 && blinkCount_ >= maxBlinks_ * 2 && currentMillis - lastBlinkTime_ < pauseInterval_) {
        // In pause, do nothing until pause is over
        return;
    }

    // Reset pattern after pause or initially
    if (status_ != 0 && blinkCount_ >= maxBlinks_ * 2 && currentMillis - lastBlinkTime_ >= pauseInterval_) {
        resetBlinkPattern();
        return; // Start new set of blinks after pause
    }

    // Continue with blink logic if not pausing and not resetting
    if (status_ != 0 && (currentMillis - lastBlinkTime_ >= blinkInterval_) && blinkCount_ < maxBlinks_ * 2) {
        toggleLED();
        lastBlinkTime_ = currentMillis;
    }
}

void LEDStatusManager::setStatus(int status) {
    status_ = status;
    resetBlinkPattern();

    switch (status_) {
        case 1: maxBlinks_ = 1; break; // SD
        case 2: maxBlinks_ = 2; break; // WiFi
        case 3: maxBlinks_ = 3; break; // WAN
        case 4: maxBlinks_ = 4; break; // Config Error
        default: maxBlinks_ = 0; break; // LED off for OK status
    }
}

void LEDStatusManager::toggleLED() {
    ledState_ = !ledState_;
    digitalWrite(INDICATOR_LED_PIN, ledState_ ? HIGH : LOW);
    blinkCount_++;
}

void LEDStatusManager::resetBlinkPattern() {
    blinkCount_ = 0;
    lastBlinkTime_ = millis();
    digitalWrite(INDICATOR_LED_PIN, LOW); // Ensure LED is off during pause
    ledState_ = false;
}
