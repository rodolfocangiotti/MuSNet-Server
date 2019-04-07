#ifdef __linux__
#include <signal.h>
#endif
#include <unistd.h>
#include <iostream>
#include "StreamServer.h"

bool running = false;
void signalHandler(int signum) {
   std::cout << "Receiving termination signal (" << signum << ")..." << std::endl;
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
      sleep(1);
    }
    s.stop(); // THE EXECUTION BLOCKS HERE WHEN TERMINATION SIGNAL IS RECEIVED!
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
