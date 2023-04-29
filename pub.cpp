#include <iostream>
#include <mosquitto.h>
#include<cpufreq.h>
#include <sys/sysinfo.h>
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
int get_battery_status() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        std::cout << "Error getting system information" << std::endl;
        return -1;
    }

    return info.battery_level;
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