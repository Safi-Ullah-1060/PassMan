#include "../includes/Service.h"
#include <MyStr>

ostream &operator<<(ostream &out, Service s) {
  if (!s.name.empty())
    out << "Serive: " << s.name << endl << s.data;
  else
    cout << "Service not defined\n";
  return out;
}
istream &operator>>(istream &in, Service s) {
  cout << "Enter Service Name: ";
  in >> s.name >> s.data;
  return in;
}
ofstream &operator<<(ofstream &out, Service s) {
  if (!s.name.empty() and out.is_open())
    out << s.name << endl << s.data;
  return out;
}
ifstream &operator>>(ifstream &in, Service s) {
  if (!s.name.empty()) {
    getline(in, s.name, '\n');
    in >> s.data;
  }
  return in;
}

void Service::view() { cout << *this; }
Data *Service::getData() { return &data; }
void Service::setName() {
  cout << "Enter Service Name: ";
  cin >> name;
}
MyStr Service::getName() { return name; }
