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
  void setUsername();
  void setPasskey();
  MyStr getUsername();
  MyStr getPasskey();
  void addService();
  void listServices();
  void removeService(MyStr name);
  void viewService(unsigned int);

private:
  unsigned int uID;
  MyStr username, passkey;
  Vector<Service> services;
  unordered_map<string, unsigned int> indexes;
  static unsigned int service_count;
};
