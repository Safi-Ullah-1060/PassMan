#include "../includes/Data.h"
#include <string>

ostream &operator<<(ostream &out, Data d) {
  out << "Username: " << d.username << endl
      << "Email: " << d.email << endl
      << "Password: " << d.password << endl;
  return out;
}
istream &operator>>(istream &in, Data d) {
  cout << "Enter Username: ";
  in >> d.username;
  cout << "Enter Email: ";
  in >> d.email;
  cout << "Enter Password: ";
  in >> d.password;
  return in;
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
void Data::view() {
  if (!username.empty())
    cout << *this;
}
void Data::setEmail() {
  cout << "Enter New Username: ";
  cin >> username;
}
void Data::setUserName() {
  cout << "Enter New Email: ";
  cin >> email;
}
void Data::setPassword() {
  cout << "Enter New Password: ";
  cin >> password;
}
