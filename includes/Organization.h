#pragma once
#include "MyStr.h"
#include "Vector.h"

class User;

class Organization {
public:
  Organization();
  Organization(const Organization &) = default;
  Organization &operator=(const Organization &) = default;
  ~Organization();

private:
  MyStr name;
  User *admin;
  Vector<User *> members;
};
