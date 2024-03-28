#include "EnergyProfiling.h"


bool isQuerying = false;
static bool isSignalHandlerRegistered = false;
void signalHandler(int signal) {
  isQuerying = false;
  exit(EXIT_SUCCESS);
}

void* t_startPowerQuery(void* arg) {
  nvmlDevice_t device;
  auto t = nvmlDeviceGetHandleByIndex(
      0, &device);  // Assuming there's at least one GPU, so index 0
  if (t != NVML_SUCCESS) {
    std::cerr << "Failed to get device handle: " << nvmlErrorString(t)
              << std::endl;
    nvmlShutdown();
  }
  isQuerying = true;
  nvmlReturn_t result;

  // calculate energy consumption during isQuerying is true
  unsigned int energyConsumed = 0;
  auto start = std::chrono::high_resolution_clock::now();
  while (isQuerying) {
    unsigned int power;
    result = nvmlDeviceGetPowerUsage(device, &power);
    if (result != NVML_SUCCESS) {
      std::cerr << "Failed to get power usage: " << nvmlErrorString(result)
                << std::endl;
      break;
    }
    std::cout << "Power consumption: " << power / 1000.0 << " W"
              << std::endl;  // Convert to watts
    std::this_thread::sleep_for(
        std::chrono::milliseconds(200));     // Sleep for 200 milliseconds
    energyConsumed += power / 1000.0 * 0.2;  // Convert to joules
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::string msg = *(std::string*)arg;
  // open file and write msg
  std::ofstream f;
  f.open("power.txt");
  f << msg << ": " << energyConsumed << " J"
       << " in "
       << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
       << " seconds\n";
  pthread_exit(NULL);
}
void startEnergyCalculate(std::string msg) {
  pthread_t powerThread;
  if (isSignalHandlerRegistered == false) {
    signal(SIGINT, signalHandler);
    isSignalHandlerRegistered = true;
  }
  pthread_create(&powerThread, NULL, t_startPowerQuery, (void*)&msg);
}
void stopEnergyCalculate() {
  isQuerying = false;
  nvmlShutdown();
}