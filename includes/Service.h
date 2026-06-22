#pragma once
#include "Data.h"
#include "Token.h"
#include "Vector.h"

class Service {
public:
  Service();
  Service(MyStr ServiceName, MyStr ServiceUserName, MyStr EMail,
          MyStr Password);
  Service(const Service &) = default;
  Service &operator=(const Service &) = default;
  Data *getData();
  void setData(MyStr ServiceUserName, MyStr EMail, MyStr Password);
  Vector<Token *> *getTokensList();
  MyStr getName();
  void setName(MyStr);
  ~Service() = default;

  friend ofstream &operator<<(ofstream &out, Service s);
  friend ifstream &operator>>(ifstream &in, Service s);

private:
  MyStr name;
  Data data;
  Vector<Token *> tokens;
};
