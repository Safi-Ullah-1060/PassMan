#include "../includes/PassMan.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
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
  MasterPassword = PassKey;

  // Derive key once — Argon2 runs exactly here, never again
  uint8_t salt[SALT_LEN];

  std::ifstream dat("~/.local/share/PassMan/PassMan.dat", std::ios::binary);
  if (dat.good()) {
    // Existing install: read salt from first 16 bytes of PassMan.dat
    dat.read((char *)salt, SALT_LEN);
    dat.close();
    derive_key(m_key, PassKey.toStr().c_str(), salt);
    m_key_set = true;
    if (!loadData()) {
      crypto_wipe(m_key, 32);
      m_key_set = false;
      return false; // wrong password
    }
  } else {
    // First launch: generate and store salt, no loadData needed
    secure_random(salt, SALT_LEN);
    derive_key(m_key, PassKey.toStr().c_str(), salt);
    m_key_set = true;
    // PassMan.dat will be written on first saveData()
  }

  // find and set current_user ...
  return true;
}

bool PassMan::saveData() {
  if (!m_key_set)
    return false;
  std::string base = "~/.local/share/PassMan/";

  // ── PassMan.dat: salt(16) + encrypted(master password + user_count) ──
  {
    std::ostringstream oss;
    // write MasterPassword and user_count char by char as before
    std::string plaintext = oss.str();

    // Read existing salt from file or generate new one
    uint8_t salt[SALT_LEN];
    std::ifstream sf(base + "PassMan.dat", std::ios::binary);
    if (sf.good())
      sf.read((char *)salt, SALT_LEN);
    else
      secure_random(salt, SALT_LEN);
    sf.close();

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

    // <username>.dat
    {
      std::ostringstream oss;
      // write username, passkey char by char
      save_encrypted_with_key(udir + users[i]->getUsername().toStr() + ".dat",
                              oss.str().c_str(), m_key);
    }

    // Services/
    Vector<Service> *svcs = users[i]->getServices();
    for (unsigned int i = 0; i < svcs->size(); ++i) {
      Service &svc = (*svcs)[i];
      std::ostringstream oss;
      // write service data char by char
      save_encrypted_with_key(udir + "Services/" + svc.getName().toStr() +
                                  ".dat",
                              oss.str().c_str(), m_key);
    }
  }
  return true;
}

bool PassMan::loadData() {
  if (!m_key_set)
    return false;
  std::string base = "~/.local/share/PassMan/";

  // First launch check
  std::ifstream test(base + "PassMan.dat", std::ios::binary);
  if (!test.good())
    return true;

  // Skip the 16-byte salt header, decrypt the rest
  test.seekg(SALT_LEN);
  std::vector<uint8_t> blob((std::istreambuf_iterator<char>(test)),
                            std::istreambuf_iterator<char>());
  test.close();

  std::string plaintext;
  try {
    plaintext = decrypt_with_key(blob, m_key);
  } catch (...) {
    return false; // wrong master password
  }

  std::istringstream iss(plaintext);
  // read MasterPassword and user_count char by char from iss

  // Then load each user and service file the same way
  // load_decrypted_with_key(udir + username + ".dat", m_key) → istringstream

  return true;
}

// ---------------- HELPER METHODS ----------------------

bool PassMan::ensureUserDataTreeExists() {
  MyStr un = PassMan::current_user->getUsername();
  MyStr path = getenv("HOME");
  path += "/.local/share/PassMan/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  path += "UserData/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  path += un + "/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  path += "Services/";
  if (!filesystem::exists(path.getData()))
    filesystem::create_directory(path.getData());
  return 1;
}
