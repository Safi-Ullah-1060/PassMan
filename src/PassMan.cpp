#include "../includes/PassMan.h"

unsigned int PassMan::user_count = 0;
PassMan *PassMan::me = nullptr;

PassMan *PassMan::getInstance() {
  return me = me != nullptr ? me : new PassMan();
}

void PassMan::addUser(MyStr un, MyStr pk) {
  users.push_back(new User());
  users[user_count++]->setUsername(un);
  users[user_count - 1]->setPasskey(pk);
}
void PassMan::removeUser(MyStr un) {
  unsigned int UE = userExists(un);
  users.remove_at(--UE);
}
unsigned int PassMan::userExists(MyStr un) {
  for (unsigned int i = 0; i < user_count; i++)
    if (users[i]->getUsername().is_equal(un))
      return i + 1;
  return 0;
}
bool PassMan::login(MyStr un, MyStr pk) {
  unsigned int i = userExists(un) != -1;
  if (i != 0 and users[i - 1]->getPasskey().is_equal(pk)) {
    current_user = users[i - 1];
    return 1;
  }
  current_user = nullptr;
  return 0;
}
