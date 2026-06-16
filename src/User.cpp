#include "../includes/User.h"

unsigned int User::service_count = 0;

void User::setUsername() {
  cout << "Enter new Username for [" << username << "] : ";
  cin >> username;
}
void User::setPasskey() {
  cout << "Enter new setPasskey for [" << username << "] : ";
  cin >> passkey;
}
MyStr User::getUsername() { return username; }
MyStr User::getPasskey() { return passkey; }
void User::addService() {
  cout << "\033[2J\033[1;1H";
  unsigned int limit = 0;
  if (service_count < limit - 1) {
    services.push_back(Service());
    services[services.size() - 1].setName();
    service_count += 1;
    indexes.insert({string(services[service_count - 1].getName().getData()),
                    service_count});
  } else
    cout << "Service Limit reached\n";
}
void User::listServices() {
  cout << "\033[2J\033[1;1H";
  cout << "Services List:\n";
  for (int i = 0; i < services.size(); i++)
    cout << i + 1 << " : " << services[i].getName() << endl;
}
void User::removeService(MyStr name) {
  if (service_count > 0) {
    unsigned int target = indexes.at(string(name.getData()));
    services.remove_at(target);
    service_count -= 1;
  }
}
void User::viewService(unsigned int i) { services[i].view(); }
