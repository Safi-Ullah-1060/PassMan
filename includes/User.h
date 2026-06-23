#pragma once
#include "MyStr.h"
#include "Organization.h"
#include "Service.h"
#include "Vector.h"
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
  bool addService(MyStr ServiceName, MyStr ServiceUserName = MyStr(),
                  MyStr EMail = MyStr(), MyStr Password = MyStr());
  Vector<Service> *getServices();
  void resetServices();
  bool removeService(MyStr ServiceName);
  unsigned int findService(MyStr ServiceName);
  Service *getService(unsigned int);

protected:
  unsigned int uID;
  MyStr username, passkey;
  Vector<Service> services;
  unordered_map<string, unsigned int> indexes;
  static unsigned int service_count;
};

class Admin {
public:
  Admin();
  Admin(Admin &&) = default;
  Admin(const Admin &) = default;
  Admin &operator=(Admin &&) = default;
  Admin &operator=(const Admin &) = default;
  ~Admin();

private:
  Vector<Organization> orgs;
  Vector<MyStr> MasterPasswords;
  static unsigned int orgCount;
};
