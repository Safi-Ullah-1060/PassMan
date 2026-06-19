#pragma once
#include "MyStr.h"
#include <fstream>
#include <iostream>
using namespace std;

class Data {
public:
  Data() = default;
  Data(const Data &) = default;
  Data &operator=(const Data &) = default;
  MyStr getEmail();
  MyStr getUserName();
  MyStr getPassword();
  void setEmail(MyStr EMail);
  void setUserName(MyStr UserName);
  void setPassword(MyStr Password);
  ~Data() = default;

  friend ostream &operator<<(ostream &out, Data d);
  friend istream &operator>>(istream &out, Data d);
  friend ofstream &operator<<(ofstream &out, Data d);
  friend ifstream &operator>>(ifstream &out, Data d);

private:
  MyStr username, email, password;
};
