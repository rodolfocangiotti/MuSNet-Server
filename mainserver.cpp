#ifdef __linux__
#include <signal.h>
#endif
#include <chrono>
#include <iostream>
#include "StreamServer.h"
#include "prettyprint.h"

bool running = false;
void signalHandler(int signum) {
  std::cout << "Receiving termination signal (" << signum << ")..." << '\n';
  ::running = false;
}

int main(int argc, const char* argv[]) {
  signal(SIGTERM, signalHandler);
  try {
    StreamServer s;
    s.configure(50000, 50001);
    s.start();
    ::running = true;
    while (::running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    s.stop(); // THE EXECUTION BLOCKS HERE WHEN TERMINATION SIGNAL IS RECEIVED!
  } catch (std::exception& e) {
    std::cerr << RED << e.what() << RESET << '\n';
  }
  return 0;
}
