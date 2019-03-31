#include <iostream>
#include "Manager.h"

Manager::Manager():
  myClients() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::~Manager() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::Token Manager::addClient() {
  std::cout << "ADDING CLIENT..." << std::endl;
  static Token t = 0;
  myClients.emplace_back(StreamClient(t++));
  return t;
}

void Manager::removeClient(Token t) {
  std::cout << "REMOVING CLIENT..." << std::endl;
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients.erase(myClients.begin() + i);
    }
  }
}
