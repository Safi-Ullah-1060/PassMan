#include "../includes/User.h"

unsigned int User::service_count = 0;

void User::setUsername(MyStr un) { username = un; }
void User::setPasskey(MyStr pk) { passkey = pk; }
MyStr User::getUsername() { return username; }
MyStr User::getPasskey() { return passkey; }
bool User::addService(MyStr ServuceName, MyStr ServiceUserName, MyStr EMail,
                      MyStr Password) {
  unsigned int limit = 0 - 1;
  if (service_count < limit) {
    services.push_back(Service(ServuceName, ServiceUserName, EMail, Password));
    service_count += 1;
    indexes.insert({string(services[service_count - 1].getName().getData()),
                    service_count});
  } else
    return 0;
  return 1;
}
unsigned int User::findService(MyStr name) {
  return indexes.at(string(name.getData()));
}
Vector<Service> *User::getServices() { return &services; }
void User::removeService(MyStr name) {
  if (service_count > 0) {
    unsigned int target = indexes.at(string(name.getData()));
    services.remove_at(target);
    service_count -= 1;
  }
}
Service *User::getService(unsigned int i) {
  if (i > 0 and i < services.size())
    return &services[i];
  return nullptr;
}
