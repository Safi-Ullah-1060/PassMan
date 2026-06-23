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
  std::string key(name.getData());
  auto it = indexes.find(key);
  if (it == indexes.end())
    return 0;
  return it->second;
}
Vector<Service> *User::getServices() { return &services; }
bool User::removeService(MyStr name) {
  if (service_count == 0)
    return false;

  std::string key(name.getData());
  auto it = indexes.find(key);
  if (it == indexes.end())
    return false;

  unsigned int target = it->second;

  // Remove from vector
  services.remove_at(target - 1);
  service_count--;

  // Remove from map
  indexes.erase(it);

  // Shift down all indexes that were after the removed element
  for (auto &pair : indexes)
    if (pair.second > target)
      pair.second--;

  return true;
}
void User::resetServices() {
  services.clear();
  indexes.clear();
  service_count = 0;
}
Service *User::getService(unsigned int i) {
  if (i >= 0 and i < services.size())
    return &services[i];
  return nullptr;
}
