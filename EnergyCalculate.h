#pragma once
#include <iostream>
#include <pthread.h>
#include <nvml.h>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

// Function to start querying GPU power
void startEnergyCalculate(std::string msg);
// Function to stop querying GPU power
void stopEnergyCalculate();

