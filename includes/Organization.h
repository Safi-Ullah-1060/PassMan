#pragma once
#include "User.h"
#include "Vector.h"

class Organization {
public:
  Organization();
  Organization(Organization &&) = default;
  Organization(const Organization &) = default;
  Organization &operator=(Organization &&) = default;
  Organization &operator=(const Organization &) = default;
  ~Organization();

private:
  MyStr name, masterKey;
  User *admin;
  Vector<User *> members;
};
