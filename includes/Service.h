#pragma once
#include "Data.h"
#include <vector>

class Service {
public:
  Service() = default;
  Service(const Service &) = default;
  Service &operator=(const Service &) = default;
  void view();
  Data *getData();
  void setName();
  MyStr getName();
  ~Service() = default;

  friend ostream &operator<<(ostream &out, Service s);
  friend istream &operator>>(istream &in, Service s);
  friend ofstream &operator<<(ofstream &out, Service s);
  friend ifstream &operator>>(ifstream &in, Service s);

private:
  MyStr name;
  Data data;
};
