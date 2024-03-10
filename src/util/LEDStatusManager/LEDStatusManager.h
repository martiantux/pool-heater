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

#ifndef LEDStatusManager_h
#define LEDStatusManager_h

#include <Arduino.h>
#include "util/config.h"

class LEDStatusManager {
public:
    static LEDStatusManager& getInstance() {    // Singleton instance
        static LEDStatusManager instance;
        return instance;
    }

    void setup();
    void update();
    void setStatus(int status);

private:
    LEDStatusManager();                         // Private constructor/destructor for singleton
    ~LEDStatusManager() = default;
    LEDStatusManager(const LEDStatusManager&) = delete;
    LEDStatusManager& operator=(const LEDStatusManager&) = delete;

    int status_;                                // Current LED status
    unsigned long lastBlinkTime_;               // Timestamp of the last LED blink
    const unsigned long blinkInterval_ = 200;   // Duration of each blink in milliseconds
    const unsigned long pauseInterval_ = 2000;  // Pause duration between sets of blinks in milliseconds
    int blinkCount_;                            // Current count of blinks in the ongoing sequence
    int maxBlinks_;                             // Maximum number of blinks in the current pattern
    bool ledState_;                             // Current state of the LED (ON/OFF)

    void toggleLED();                           // Toggles the current state of the LED
    void resetBlinkPattern();                   // Resets the blinking pattern based on the current status
};

#endif // LEDStatusManager_h
