#ifdef __linux__
#include <signal.h>
#endif
#include <chrono>
#include <iostream>
#include "Console.h"
#include "Profiler.h"
#include "StreamServer.h"
#include "prettyprint.h"

bool running = false;
void signalHandler(int signum) {
  Console::log("Receiving termination signal (" + str(signum) + ")...");
  ::running = false;
}

int main(int argc, const char* argv[]) {
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  try {
    Console::start();
    StreamServer s;
    s.configure(50000, 50001);
    s.start();
    ::running = true;
    while (::running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Exiting..." << std::endl;
    Console::stop();
    s.stop();
    Profiler::print_statistics();
    Profiler::export_records(EXPORT_RECORDS_FOLDER);
  } catch (std::exception& e) {
    std::cerr << RED << e.what() << RESET << '\n';
  }
  return 0;
}
