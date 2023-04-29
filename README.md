# Iot_project

This project collects and displays the current CPU frequency and battery status of a device. It also plots a graph between CPU frequency and time, and battery status and time.

How to Run

Dependencies

This project requires the following dependencies:

-> libmosquitto

-> libcpufreq

-> gnuplot

-> Libraries :: windows.h,unistd.h,fcntl.h,sys/types.h,sys/stat.h


Installation

To install the dependencies download the libraries from the internet and link then to the code

Build

To build the project, run the following command:

g++ *********.cpp -o main -lmosquitto -lcpufreq


Test Cases

This project has the following test cases:


-> Test that the CPU frequency can be retrieved successfully.

-> Test that the battery status can be retrieved successfully.

-> Test that the graph can be plotted successfully.

