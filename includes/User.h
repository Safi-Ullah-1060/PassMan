#pragma once
#include "Service.h"
#include "Vector.h"
#include <string>
#include <unordered_map>

class User {
public:
  User() = default;
  User(const User &) = default;
  User &operator=(const User &) = default;
  ~User() = default;
  void setUsername(MyStr _username);
  void setPasskey(MyStr _passkey);
  MyStr getUsername();
  MyStr getPasskey();
  bool addService(MyStr ServiceName);
  Vector<Service> *getServices();
  void removeService(MyStr ServiceName);
  unsigned int findService(MyStr ServiceName);
  Service *getService(unsigned int);

private:
  unsigned int uID;
  MyStr username, passkey;
  Vector<Service> services;
  unordered_map<string, unsigned int> indexes;
  static unsigned int service_count;
};
