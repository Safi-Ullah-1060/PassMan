#pragma once
#include "User.h"

class PassMan {
public:
  PassMan(const PassMan &) = default;
  PassMan &operator=(const PassMan &) = default;
  void addUser();
  void removeUser(MyStr un);
  bool login();
  unsigned int userExists(MyStr un);
  static PassMan *makeNew();
  User *current_user;

private:
  PassMan() = default;
  ~PassMan() = default;
  static PassMan *me;
  Vector<User *> users;
  MyStr MasterPassword;
  static unsigned int user_count;
};
