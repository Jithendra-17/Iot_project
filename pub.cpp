#include <iostream>
#include <mosquitto.h>
#include<cpufreq.h>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

using namespace std;

//Funtion to get the current cpu frequency
int get_cpu_frequency() {
    cpufreq_init();
    cpufreq_policy *policy = cpufreq_get_policy(0);

    if (policy) {
        int current_frequency = policy->cur / 1000.0; // convert from Hz to kHz
        cpufreq_put_policy(policy);
        cpufreq_exit();
        return current_frequency;
    }

    cpufreq_exit();
    return -1; // return -1 if current frequency couldn't be obtained
}

//Function to get battery status
enum BatteryStatus {
    BATTERY_STATUS_UNKNOWN = 0,
    BATTERY_STATUS_CHARGING = 1,
    BATTERY_STATUS_DISCHARGING = 2,
    BATTERY_STATUS_FULL = 3,
};

BatteryStatus getBatteryStatus() {
    BatteryStatus status = BATTERY_STATUS_UNKNOWN;

#ifdef _WIN32
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus)) {
        if (powerStatus.ACLineStatus == 1) {
            status = BATTERY_STATUS_CHARGING;
        } else {
            switch (powerStatus.BatteryFlag) {
                case 8:
                    status = BATTERY_STATUS_CHARGING;
                    break;
                case 128:
                    status = BATTERY_STATUS_FULL;
                    break;
                default:
                    status = BATTERY_STATUS_DISCHARGING;
                    break;
            }
        }
    }
#elif __linux__
    int fd = open("/sys/class/power_supply/BAT0/status", O_RDONLY);
    if (fd != -1) {
        char buffer[16];
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n-1] = '\0'; // Remove trailing newline
            if (strcmp(buffer, "Charging") == 0) {
                status = BATTERY_STATUS_CHARGING;
            } else if (strcmp(buffer, "Full") == 0) {
                status = BATTERY_STATUS_FULL;
            } else {
                status = BATTERY_STATUS_DISCHARGING;
            }
        }
        close(fd);
    }
#endif

    return status;
}

void get_battery_status() {
    BatteryStatus status = getBatteryStatus();
    switch (status) {
        case BATTERY_STATUS_UNKNOWN:
            std::cout << "Unknown" << std::endl;
            break;
        case BATTERY_STATUS_CHARGING:
            std::cout << "Charging" << std::endl;
            break;
        case BATTERY_STATUS_DISCHARGING:
            std::cout << "Discharging" << std::endl;
            break;
        case BATTERY_STATUS_FULL:
            std::cout << "Full" << std::endl;
            break;
    }
}



int main() {
  // Create a MQTT client
  mosquitto_client *client = mosquitto_new("client_id", true, NULL);

  // Connect to the MQTT broker
  if (mosquitto_connect(client, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
    cout << "Error connecting to the MQTT broker" << endl;
    return -1;
  }

  // Start publishing messages
  while (true) {
    // Get the current CPU frequency,Battery status
    int cpu_frequency = get_cpu_frequency();
    int battery_status = get_battery_status();

    // Publish a message with the UUID, CPU frequency, and battery status
    mosquitto_publish(client, "/cpu_frequency", strlen("/cpu_frequency"), &cpu_frequency, 1, true);
    mosquitto_publish(client, "/battery_status", strlen("/battery_status"), &battery_status, 1, true);

    // Sleep for 1sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Disconnect from the MQTT broker and Destroy the MQTT client
  mosquitto_disconnect(client);
  mosquitto_destroy(client);

  return 0;
}
