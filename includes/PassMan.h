#pragma once
#include "../src/crypto.hpp"
#include "MyStr.h"
#include "User.h"
#include <cstdint>

class PassMan {
public:
  PassMan(const PassMan &) = default;
  PassMan &operator=(const PassMan &) = default;
  void addUser(MyStr UserName, MyStr PassKey);
  void removeUser(MyStr UserName);
  bool login(MyStr UserName, MyStr PassKey);
  unsigned int userExists(MyStr un);

  bool loadData();
  bool loadPassManDat();
  bool loadUserData(User *u);

  bool saveData();
  bool savePassManDat();
  bool saveUserData(User *u);

  static PassMan *getInstance();
  static User *current_user;
  bool ensureUserDataTreeExists();

private:
  PassMan() = default;
  ~PassMan() = default;
  static PassMan *me;
  Vector<User *> users;
  MyStr MasterPassword;
  static unsigned int user_count;
  std::uint8_t m_key[32] = {};
  bool m_key_set = false;
};
