#include "../includes/PassMan.h"
#include "Paths.hpp"
#include "crypto.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
namespace fs = std::filesystem;

unsigned int PassMan::user_count = 0;
PassMan *PassMan::me = nullptr;
User *PassMan::current_user = nullptr;

PassMan *PassMan::getInstance() { return me = me ? me : new PassMan(); }

void PassMan::addUser(MyStr un, MyStr pk) {
  users.push_back(new User());
  users[user_count]->setUsername(un);
  users[user_count]->setPasskey(pk);
  user_count++;
}

string base = getBaseDir();
string userDataDir = getUserDataDir(base);

void PassMan::removeUser(MyStr un) {
  unsigned int idx = userExists(un);
  if (idx != 0)
    users.remove_at(idx - 1);
}

unsigned int PassMan::userExists(MyStr un) {
  for (unsigned int i = 0; i < user_count; i++)
    if (users[i]->getUsername().is_equal(un))
      return i + 1;
  return 0;
}

void PassMan::setMasterPassword(MyStr MPass) {
  MasterPassword = MPass;

  ensureUserDataTreeExists(nullptr);

  // Generate salt and derive master key right here
  uint8_t salt[SALT_LEN];
  secure_random(salt, SALT_LEN);
  derive_key(m_master_key, MPass.toStr().c_str(), salt);
  m_master_key_set = true;

  // Write PassMan.dat with just the master password and zero users
  // (users get added after this via addUser + saveData)
  std::ostringstream oss;
  oss << MasterPassword.toStr() << "\n";
  oss << "0\n";
  std::ofstream f(base + "PassMan.dat", std::ios::binary | std::ios::trunc);
  f.write((const char *)salt, SALT_LEN);
  auto blob = encrypt_with_key(oss.str().c_str(), m_master_key);
  f.write((const char *)blob.data(), blob.size());
}

bool PassMan::login(MyStr UserName, MyStr PassKey) {
  // Step 1: load user list (plaintext)
  if (!loadPassManDat())
    return false;

  // Step 2: verify
  unsigned int idx = userExists(UserName);
  if (idx == 0)
    return false;
  User *u = users[idx - 1];
  if (!u->getPasskey().is_equal(PassKey))
    return false;

  // Step 3: derive user key and load their files
  uint8_t user_salt[SALT_LEN];
  string ufile =
      base + "UserData/" + UserName.toStr() + "/" + UserName.toStr() + ".dat";

  ifstream uf(ufile, std::ios::binary);
  if (uf.good()) {
    uf.read((char *)user_salt, SALT_LEN);
    uf.close();
  } else {
    secure_random(user_salt, SALT_LEN);
  }

  derive_key(m_user_key, PassKey.toStr().c_str(), user_salt);
  m_user_key_set = true;

  u->resetServices();
  if (fs::exists(ufile))
    loadUserData(u);

  current_user = u;

  if (!fs::exists(ufile))
    saveUserData(u);

  return true;
}

// ── Save
// ──────────────────────────────────────────────────────────────────────

bool PassMan::saveData() {
  return savePassManDat() && saveUserData(current_user);
}
bool PassMan::savePassManDat() {
  ensureUserDataTreeExists(nullptr);
  std::ofstream f(base + "PassMan.dat"); // no binary, no encryption
  f << MasterPassword.toStr() << "\n";
  for (unsigned int i = 0; i < user_count; i++)
    f << users[i]->getUsername().toStr() << ","
      << users[i]->getPasskey().toStr() << "\n";
  return true;
}
bool PassMan::saveUserData(User *u) {
  if (!u || !m_user_key_set)
    return false;
  ensureUserDataTreeExists(u);

  string uname = u->getUsername().toStr();
  string UserDir = getUserDir(userDataDir, uname);

  // Generate salt for user file
  uint8_t user_salt[SALT_LEN];
  secure_random(user_salt, SALT_LEN);
  derive_key(m_user_key, u->getPasskey().toStr().c_str(), user_salt);

  // <username>.dat: salt(16) + encrypted(username,passkey)
  {
    std::ostringstream oss;
    oss << uname << "," << u->getPasskey().toStr() << "\n";

    std::ofstream f(UserDir + uname + ".dat",
                    std::ios::binary | std::ios::trunc);
    f.write((const char *)user_salt, SALT_LEN);
    auto blob = encrypt_with_key(oss.str().c_str(), m_user_key);
    f.write((const char *)blob.data(), blob.size());
  }

  // Services/
  string servDir = getUserServicesDir(UserDir);
  Vector<Service> *svcs = u->getServices();
  filesystem::remove_all(servDir);
  filesystem::create_directory(servDir);
  for (unsigned int i = 0; i < svcs->size(); i++) {
    Service &svc = (*svcs)[i];
    Data *data = svc.getData();

    std::ostringstream oss;
    oss << svc.getName().toStr() << "," << data->getUserName().toStr() << ","
        << data->getEmail().toStr() << "," << data->getPassword().toStr()
        << "\n";

    // No per-file salt needed — user key already derived above
    auto blob = encrypt_with_key(oss.str().c_str(), m_user_key);
    string path = servDir + svc.getName().toStr() + ".dat";
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    f.write((const char *)blob.data(), blob.size());
  }
  return true;
}

// ── Load
// ──────────────────────────────────────────────────────────────────────

bool PassMan::loadPassManDat() {
  std::ifstream f(base + "PassMan.dat");
  if (!f.good())
    return true; // first launch

  std::string line;
  std::getline(f, line);
  MasterPassword = MyStr(line.c_str());

  while (std::getline(f, line)) {
    if (line.empty())
      continue;
    size_t comma = line.find(',');
    if (comma == std::string::npos)
      continue;
    addUser(MyStr(line.substr(0, comma).c_str()),
            MyStr(line.substr(comma + 1).c_str()));
  }
  return true;
}
bool PassMan::loadUserData(User *u) {
  if (!u || !m_user_key_set)
    return 0;
  ensureUserDataTreeExists(u);

  string uname = u->getUsername().toStr();
  string UserDir = getUserDir(userDataDir, uname);
  string servDir = getUserServicesDir(UserDir);

  // Load service files by scanning the Services/ directory
  if (!fs::exists(servDir))
    return true; // no services yet

  for (auto &entry : fs::directory_iterator(servDir)) {
    if (entry.path().extension() != ".dat")
      continue;

    std::ifstream sf(entry.path(), std::ios::binary);
    std::vector<uint8_t> blob((std::istreambuf_iterator<char>(sf)),
                              std::istreambuf_iterator<char>());
    sf.close();

    std::string plaintext;
    try {
      plaintext = decrypt_with_key(blob, m_user_key);
    } catch (...) {
      continue; // skip corrupt file
    }

    std::istringstream iss(plaintext);
    std::string name, username, email, password;
    std::getline(iss, name, ',');
    std::getline(iss, username, ',');
    std::getline(iss, email, ',');
    std::getline(iss, password, '\n');

    // Add service and populate its data
    u->addService(MyStr(name.c_str()), MyStr(username.c_str()),
                  MyStr(email.c_str()), MyStr(password.c_str()));
  }
  return true;
}
bool PassMan::ensureUserDataTreeExists(User *u) {
  fs::create_directories(userDataDir);
  if (u) {
    string uname = u->getUsername().toStr();
    string UserDir = getUserDir(userDataDir, uname);
    string servDir = getUserServicesDir(UserDir);
    fs::create_directories(servDir);
  }
  return true;
}
