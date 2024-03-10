
#ifndef CONFIG_H
#define CONFIG_H

#include <IPAddress.h>

struct MDNSService {
    const char* service;
    const char* protocol;
    uint16_t port;
};

#define HOSTNAME "pool-heater"
#define WIFI_SSID "yourSSID"
#define WIFI_PASSWORD "yourPassword"
#define WIFI_IP IPAddress(192, 168, 1, 100)
#define WIFI_GATEWAY IPAddress(192, 168, 1, 1)
#define WIFI_SUBNET IPAddress(255, 255, 255, 0)
#define WIFI_PRIMARY_DNS IPAddress(1, 1, 1, 1)      // Cloudflare DNS
#define WIFI_SECONDARY_DNS IPAddress(8, 8, 8, 8)    // Google DNS

#define FIRMWARE_VERSION "1.1.13"

#define INDICATOR_LED_PIN 2             // Pin to use for indicator LED
#define MAX_BUFFER_SIZE 100             // Max log lines to keep in the buffer
#define NTP_SERVER "pool.ntp.org"       // NTP time sync server
#define TIMEZONE_OFFSET 0               // Timezone offset in seconds

// Define your mDNS services here
const MDNSService mdnsServices[] = {
    {"http", "tcp", 80}  // HTTP service
};

// Pin configurations
#define FLOW_SENSOR_PIN = 35;
#define ONE_WIRE_BUS_PIN = 21;
#define PUMP_CONTROL_PIN = 14;

// Pump control config
#define TARGET_TEMP 30.0;
#define TEMP_POLL_INTERVAL 1000;                        // How often to poll the temp sensors (milliseconds)
#define PUMP_UPDATE_INTERVAL 3000;                      // How often the pump control code will update (milliseconds)
#define SENSOR_STABILITY_DELAY 1000 * 120;              // How long to run the pump before considering sensor readings stable
#define ENERGY_CAPTURE_THRESHOLD 500;                   // Minimum watts before hibernating
#define HIBERNATION_TRIGGER_DELAY 1000 * 30;            // Time before entering hibernation after seeing insufficient energy delta
#define HIBERNATION_PERIOD = 1000 * 60 * 30             // 30 min - time to hibernate between cycles
#define MAINTENANCE_PERIOD = 1000 * 60 * 60             // How long to disarm the system if maintenace mode toggled

#endif // config_h
