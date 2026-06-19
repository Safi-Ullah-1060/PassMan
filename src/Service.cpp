#include "../includes/Service.h"

ofstream &operator<<(ofstream &out, Service s) {
  if (!s.name.is_empty() and out.is_open())
    out << s.name << endl << s.data;
  return out;
}
ifstream &operator>>(ifstream &in, Service s) {
  if (!s.name.is_empty()) {
    getline(in, s.name, '\n');
    in >> s.data;
  }
  return in;
}

Data *Service::getData() { return &data; }
Vector<Token *> *Service::getTokensList() { return &tokens; }
void Service::setName(MyStr n) { name = n; }
MyStr Service::getName() { return name; }
