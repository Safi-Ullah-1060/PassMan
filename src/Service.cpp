#include "../includes/Service.h"

Service::Service() : name(), data() {}
Service::Service(MyStr ServiceName, MyStr ServiceUserName, MyStr EMail,
                 MyStr Password)
    : name(ServiceName), data(Data(ServiceUserName, EMail, Password)) {}
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

void Service::setData(MyStr n, MyStr e, MyStr p) {
  data.setUserName(n), data.setEmail(e), data.setPassword(p);
}
Data *Service::getData() { return &data; }
Vector<Token *> *Service::getTokensList() { return &tokens; }
void Service::setName(MyStr n) { name = n; }
MyStr Service::getName() { return name; }
