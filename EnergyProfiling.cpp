#include "EnergyProfiling.h"
bool isQuerying = false;
static bool isSignalHandlerRegistered = false;
void signalHandler(int signal) {
  isQuerying = false;
  exit(EXIT_SUCCESS);
}

void* t_startPowerQuery(void* arg) {
  nvmlInit();
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
  constexpr int polling_interval = 10;
  // calculate energy consumption while isQuerying is true
  auto start = std::chrono::high_resolution_clock::now();
  int query_count = 0;
  int total_power = 0;
  while (isQuerying) {
    query_count++;
    unsigned int power;
    result = nvmlDeviceGetPowerUsage(device, &power);
    total_power += power;
    if (result != NVML_SUCCESS) {
      std::cerr << "Failed to get power usage: " << nvmlErrorString(result)
                << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(
        polling_interval));  // Sleep for polling_interval milliseconds
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::string msg = *(std::string*)arg;
  float average_power = total_power / query_count / 1000.0;
  float interval =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();

  // open file and write msg
  std::ofstream f;
  std::cout << average_power << " // " << query_count << std::endl;
  f.open("power.txt");
  f << msg << ": " << average_power * interval / 1000 / 1000 << " J"
    << " in " << interval << " us\n";
  pthread_exit(NULL);
}
pthread_t powerThread;
void startPowerQuery(std::string msg) {
  if (isSignalHandlerRegistered == false) {
    signal(SIGINT, signalHandler);
    isSignalHandlerRegistered = true;
  }
  pthread_create(&powerThread, NULL, t_startPowerQuery, (void*)&msg);
}
void stopPowerQuery() {
  isQuerying = false;
  pthread_join(powerThread, NULL);
  nvmlShutdown();

  // pthread join
}