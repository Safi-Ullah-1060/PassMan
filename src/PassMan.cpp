#include "../includes/PassMan.h"
#include "../src/crypto.hpp"
#include "crypto.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
using namespace std;

unsigned int PassMan::user_count = 0;
PassMan *PassMan::me = nullptr;
User *PassMan::current_user = nullptr;

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
bool PassMan::login(MyStr UserName, MyStr PassKey) {
  // Derive key once — Argon2 runs exactly here, never again
  uint8_t salt[SALT_LEN];
  string datFile = getenv("HOME");
  datFile.append("/.local/share/PassMan/PassMan.dat");
  ifstream dat(datFile, std::ios::binary);
  if (dat.good()) {
    // Existing install: read salt from first 16 bytes of PassMan.dat
    dat.read((char *)salt, SALT_LEN);
    dat.close();
    if (!m_key_set) {
      derive_key(m_key, PassKey.toStr().c_str(), salt);
      m_key_set = true;
    }
    bool dataLoaded = loadData();
    if (dataLoaded) {
      crypto_wipe(m_key, 32);
      m_key_set = false;
      return false; // wrong password
    }
  }
  auto i = userExists(UserName);
  if (i != 0 and loadData() and users[i - 1]->getPasskey().is_equal(PassKey)) {
    current_user = users[i - 1];
    return 1;
  }
  return 0;
}

void PassMan::setMasterPassword(MyStr MPass) {
  MasterPassword = MPass;
  uint8_t salt[SALT_LEN];
  ensureUserDataTreeExists();
  secure_random(salt, SALT_LEN);
  derive_key(m_key, MPass.toStr().c_str(), salt);
  m_key_set = true;
  std::string base = getenv("HOME");
  base += "/.local/share/PassMan/";

  // ── PassMan.dat: salt(16) + encrypted(master password + user_count) ──
  {
    std::ostringstream oss;
    oss << MasterPassword.toStr();
    // write MasterPassword and user_count char by char as before
    std::string plaintext = oss.str();
    // Write: raw salt first (unencrypted), then encrypted blob
    std::ofstream f(base + "PassMan.dat", std::ios::binary | std::ios::trunc);
    f.write((const char *)salt, SALT_LEN); // salt in plaintext
    auto blob = encrypt_with_key(plaintext.c_str(), m_key);
    f.write((const char *)blob.data(), blob.size());
  }
}

bool PassMan::saveData() {
  if (!m_key_set)
    return false;

  uint8_t salt[SALT_LEN];
  ensureUserDataTreeExists();
  secure_random(salt, SALT_LEN);
  std::string base = getenv("HOME");
  base += "/.local/share/PassMan/";
  {
    std::ostringstream oss;
    oss << MasterPassword.toStr() << "\n";
    for (int i = 0; i < user_count; i++)
      oss << users[i]->getUsername() << "\n";
    // write MasterPassword and user_count char by char as before
    std::string plaintext = oss.str();
    // Write: raw salt first (unencrypted), then encrypted blob
    std::ofstream f(base + "PassMan.dat", std::ios::binary | std::ios::trunc);
    f.write((const char *)salt, SALT_LEN); // salt in plaintext
    auto blob = encrypt_with_key(plaintext.c_str(), m_key);
    f.write((const char *)blob.data(), blob.size());
  }
  // ── Per-user and per-service files ───────────────────────────────────
  for (unsigned int i = 0; i < user_count; i++) {
    std::string udir =
        base + "UserData/" + users[i]->getUsername().toStr() + "/";
    {
      std::ostringstream oss;
      oss << current_user->getUsername() << "," << current_user->getPasskey()
          << "\n";
      save_encrypted_with_key(udir + users[i]->getUsername().toStr() + ".dat",
                              oss.str().c_str(), m_key);
    }

    // Services/
    Vector<Service> *svcs = users[i]->getServices();
    for (unsigned int i = 0; i < svcs->size(); ++i) {
      Service &svc = (*svcs)[i];
      Data *sdata = svc.getData();
      std::ostringstream oss;
      oss << svc.getName() << "," << sdata->getUserName() << ","
          << sdata->getPassword() << "\n";
      save_encrypted_with_key(udir + "Services/" + svc.getName().toStr() +
                                  ".dat",
                              oss.str().c_str(), m_key);
      sdata = nullptr;
      delete sdata;
    }
    svcs = nullptr;
    delete svcs;
  }
  return true;
}

bool PassMan::loadData() {
  if (!m_key_set)
    return false;
  std::string base = "~/.local/share/PassMan/";

  // First launch check
  std::ifstream readFile(base + "PassMan.dat", std::ios::binary);
  if (!readFile.good())
    return true;

  // Skip the 16-byte salt header, decrypt the rest
  readFile.seekg(SALT_LEN);
  std::vector<uint8_t> blob((std::istreambuf_iterator<char>(readFile)),
                            std::istreambuf_iterator<char>());
  readFile.close();

  std::string plaintext;
  try {
    plaintext = decrypt_with_key(blob, m_key);
  } catch (...) {
    return false; // wrong master password
  }

  std::istringstream iss(plaintext);
  // read MasterPmassword and user_count char by char from iss
  char *c = new char[100];
  iss.getline(c, 100, '\n');
  MasterPassword = MyStr(c);
  while (iss.getline(c, 100, '\n')) {
    users[user_count++] = new User();
    users[user_count - 1]->setUsername(MyStr(c));
  }
  delete[] c;
  // Then load each user and service file the same way
  for (unsigned int i = 0; i < user_count; i++) {
    MyStr uname = users[i]->getUsername();
    string ufile =
        base + "UserData/" + uname.getData() + "/" + uname.getData() + ".dat";
    readFile.open(ufile, std::ios::binary);
    readFile.seekg(SALT_LEN);
    std::vector<uint8_t> blob((std::istreambuf_iterator<char>(readFile)),
                              std::istreambuf_iterator<char>());
    readFile.close();
    try {
      plaintext = decrypt_with_key(blob, m_key);
    } catch (...) {
      return false; // wrong master password
    }
    try {
      plaintext = load_decrypted_with_key(ufile, m_key);
    } catch (...) {
      return false;
    }
    plaintext.clear();
    std::istringstream iss(plaintext);
    char *c = new char[100];
    while (iss.getline(c, 100, ',')) {
      users[i]->setUsername(MyStr(c));
      delete[] c;
      c = new char[100];
      iss.getline(c, 100, '\n');
      users[i]->setPasskey(MyStr(c));
    }
    delete[] c;
    unsigned int services_size = users[i]->getServices()->size();
    Vector<Service> *userv = users[i]->getServices();
    for (unsigned int j = 0; j < services_size; j++) {
      plaintext.clear();
      string sfile =
          ufile + "Services/" + (*userv)[j].getName().getData() + ".dat";
      readFile.open(sfile, std::ios::binary);
      readFile.seekg(SALT_LEN);
      std::vector<uint8_t> blob((std::istreambuf_iterator<char>(readFile)),
                                std::istreambuf_iterator<char>());
      readFile.close();
      try {
        plaintext = decrypt_with_key(blob, m_key);
      } catch (...) {
        return false; // wrong master password
      }
      try {
        plaintext = load_decrypted_with_key(sfile, m_key);
      } catch (...) {
        return false;
      }
      plaintext.clear();
      std::istringstream iss(plaintext);
      char *c = new char[100];
      while (iss.getline(c, 100, ',')) {
        Data *sData = (*userv)[j].getData();
        (*userv)[j].setName(MyStr(c));
        delete[] c;
        c = new char[100];
        iss.getline(c, 100, ',');
        sData->setUserName(c);
        delete[] c;
        c = new char[100];
        iss.getline(c, 100, ',');
        sData->setEmail(c);
        delete[] c;
        c = new char[100];
        iss.getline(c, 100, '\n');
        sData->setPassword(c);
        sData = nullptr;
        delete sData;
      }
      delete[] c;
    }
    userv = nullptr;
    delete userv;
  }

  return true;
}

// ---------------- HELPER METHODS ----------------------

bool PassMan::ensureUserDataTreeExists() {
  MyStr path = getenv("HOME");
  path += "/.local/share/PassMan/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  path += "UserData/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  if (current_user) {
    MyStr un = PassMan::current_user->getUsername();
    path += un;
    if (!filesystem::exists(path.getData()))
      filesystem::create_directory(path.getData());
    path += "Services/";
    if (!filesystem::exists(path.getData()))
      filesystem::create_directory(path.getData());
  }
  return 1;
}
