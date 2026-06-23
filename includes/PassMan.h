#pragma once
#include "MyStr.h"
#include "User.h"
#include <cstdint>

class PassMan {
public:
  PassMan(const PassMan &) = default;
  PassMan &operator=(const PassMan &) = default;
  void addUser(MyStr UserName, MyStr PassKey);
  void removeUser(MyStr UserName);
  void setMasterPassword(MyStr _MasterPassword);
  bool login(MyStr UserName, MyStr PassKey);
  unsigned int userExists(MyStr un);

  bool loadData();
  bool loadPassManDat();

  bool saveData();
  bool savePassManDat();

  static PassMan *getInstance();
  static User *current_user;
  bool ensureUserDataTreeExists(User *user);

private:
  PassMan() = default;
  ~PassMan() = default;
  static PassMan *me;
  Vector<User *> users;
  MyStr MasterPassword;
  static unsigned int user_count;
  uint8_t m_master_key[32] = {};
  uint8_t m_user_key[32] = {};
  bool m_master_key_set = false;
  bool m_user_key_set = false;
  bool saveUserData(User *u);
  bool loadUserData(User *u);
};
