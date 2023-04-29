#include<iostream>
#include<mosquitto.h>
#include<ctime>
#include "gnuplot_i.hpp"
using namespace std;

int main() {
  /*Creating a MQTT client*/
  mosquitto_client *client = mosquitto_new("client_id", true, NULL);
  // Connecting to the MQTT broker
  if (mosquitto_connect(client, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
    cout << "Error connecting to the MQTT broker" << endl;
    return -1;
  }
  /*Subscribing to topics "/cpu_frequency" and "/battery_status".*/
  mosquitto_subscribe(client, "/cpu_frequency", 0);
  mosquitto_subscribe(client, "/battery_status", 0);
  int cpu_freq = 0;
  int battery_stat = 0;
  time_t current_time;
  double timestamp = 0;
  /*Setting up the plot*/
  Gnuplot plot;
  plot.set_title("CPU Frequency and Battery Status vs Time");
  plot.set_xlabel("Time");
  plot.set_ylabel("Value");
  plot.cmd("set style data lines");
  while (true) {
    /*Receive a message*/
    mosquitto_message *msg = mosquitto_receive(client, 1000);
    if (msg != NULL) {
      if (strcmp(msg->topic, "/cpu_frequency") == 0) {
        /*Get the CPU frequency*/
        cpu_freq = *((int *)msg->payload);
        cout << "CPU frequency: " << cpu_freq << endl;
        current_time = time(nullptr);
        timestamp = static_cast<double>(current_time);
        plot.reset_plot();
        plot.plot_xy(timestamp, cpu_freq, "CPU Frequency");
        plot.plot_xy(timestamp, battery_stat, "Battery Status");
      } else if (strcmp(msg->topic, "/battery_status") == 0) {
        /*Get the battery status*/
        battery_stat = *((int *)msg->payload);
        cout << "Battery status: " << battery_stat << endl;
        current_time = time(nullptr);
        timestamp = static_cast<double>(current_time);
        plot.reset_plot();
        plot.plot_xy(timestamp, cpu_freq, "CPU Frequency");
        plot.plot_xy(timestamp, battery_stat, "Battery Status");
      }
      mosquitto_free_message(msg);
    }}
  /*Disconnect from the MQTT broker and destroy the MQTT client*/
  mosquitto_disconnect(client);
  mosquitto_destroy(client);
  mosquitto_lib_cleanup();
  return 0;
}
