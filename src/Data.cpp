#include "../includes/Data.h"

Data::Data(MyStr UserName, MyStr EMail, MyStr Password)
    : username(UserName), email(EMail), password(Password) {}
ostream &operator<<(ostream &out, Data d) {
  out << d.username << ',' << d.email << ',' << d.password << endl;
  return out;
}
ofstream &operator<<(ofstream &out, Data d) {
  out << d.username << ',' << d.email << ',' << d.password << endl;
  return out;
}
ifstream &operator>>(ifstream &in, Data d) {
  in.ignore();
  getline(in, d.username, ',');
  getline(in, d.email, ',');
  getline(in, d.password, '\n');
  return in;
}
void Data::setEmail(MyStr em) { email = em; }
void Data::setUserName(MyStr un) { username = un; }
void Data::setPassword(MyStr pw) { password = pw; }
MyStr Data::getEmail() { return email; }
MyStr Data::getUserName() { return username; }
MyStr Data::getPassword() { return password; }
