#include <iostream>
#include <mosquitto.h>
#include <ctime>
#include "gnuplot_i.hpp"

int main() {
  // Create a MQTT client
  mosquitto_client *client = mosquitto_new("client_id", true, NULL);

  // Connect to the MQTT broker
  if (mosquitto_connect(client, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
    std::cout << "Error connecting to the MQTT broker" << std::endl;
    return -1;
  }

  // Subscribe to the "/cpu_frequency" and "/battery_status" topics
  mosquitto_subscribe(client, "/cpu_frequency", 0);
  mosquitto_subscribe(client, "/battery_status", 0);

  // Initialize variables for CPU frequency and battery status
  int cpu_frequency = 0;
  int battery_status = 0;

  // Initialize variables for time and timestamp
  std::time_t current_time;
  double timestamp = 0;

  // Initialize Gnuplot
  Gnuplot plot;
  plot.set_title("CPU Frequency and Battery Status vs Time");
  plot.set_xlabel("Time");
  plot.set_ylabel("Value");
  plot.cmd("set style data lines");

  // Start receiving messages
  while (true) {
    // Receive a message
    mosquitto_message *message = mosquitto_receive(client, 1000);
    if (message != NULL) {
      // Check the topic of the message
      if (strcmp(message->topic, "/cpu_frequency") == 0) {
        // Get the CPU frequency
        cpu_frequency = *((int *)message->payload);

        // Print the CPU frequency
        std::cout << "CPU frequency: " << cpu_frequency << std::endl;

        // Get the current time and timestamp
        current_time = std::time(nullptr);
        timestamp = static_cast<double>(current_time);

        // Add the data point to the plot
        plot.reset_plot();
        plot.plot_xy(timestamp, cpu_frequency, "CPU Frequency");
        plot.plot_xy(timestamp, battery_status, "Battery Status");

      } else if (strcmp(message->topic, "/battery_status") == 0) {
        // Get the battery status
        battery_status = *((int *)message->payload);

        // Print the battery status
        std::cout << "Battery status: " << battery_status << std::endl;

        // Get the current time and timestamp
        current_time = std::time(nullptr);
        timestamp = static_cast<double>(current_time);

        // Add the data point to the plot
        plot.reset_plot();
        plot.plot_xy(timestamp, cpu_frequency, "CPU Frequency");
        plot.plot_xy(timestamp, battery_status, "Battery Status");
      }
      mosquitto_free_message(message);
    }
  }

  // Disconnect from the MQTT broker and  Destroy the MQTT client
  mosquitto_disconnect(client);
  mosquitto_destroy(client);

  mosquitto_lib_cleanup();

  return 0;
}
