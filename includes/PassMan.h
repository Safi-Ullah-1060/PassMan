#pragma once
#include "MyStr.h"
#include "User.h"

class PassMan {
public:
  PassMan(const PassMan &) = default;
  PassMan &operator=(const PassMan &) = default;
  void addUser(MyStr UserName, MyStr PassKey);
  void removeUser(MyStr UserName);
  bool login(MyStr UserName, MyStr PassKey);
  unsigned int userExists(MyStr un);
  static PassMan *getInstance();
  User *current_user;

private:
  PassMan() = default;
  ~PassMan() = default;
  static PassMan *me;
  Vector<User *> users;
  MyStr MasterPassword;
  static unsigned int user_count;
};
