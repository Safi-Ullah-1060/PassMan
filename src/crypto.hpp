#pragma once

// clang-format off
#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <bcrypt.h>
#endif
// clang-format on

#include "../third-party/monocypher/monocypher.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

// ── Blob layout (all fields concatenated, written to disk as-is)
// ──────────────
//   salt     16 bytes   Argon2 salt (random per encrypt)
//   nonce    24 bytes   XChaCha20 nonce (random per encrypt)
//   mac      16 bytes   Poly1305 authentication tag
//   ctext    N  bytes   ciphertext (same length as plaintext)
// ─────────────────────────────────────────────────────────────────────────────

static constexpr size_t SALT_LEN = 16;
static constexpr size_t NONCE_LEN = 24;
static constexpr size_t MAC_LEN = 16;
static constexpr size_t HEADER_LEN = SALT_LEN + NONCE_LEN + MAC_LEN;

// Argon2 work parameters — tweak nb_blocks/nb_passes to taste.
// 65536 blocks = 64 MB RAM, 3 passes is OWASP-recommended minimum.
static constexpr uint32_t ARGON2_BLOCKS = 65536;
static constexpr uint32_t ARGON2_PASSES = 3;

// ── Cross-platform CSPRNG
// ─────────────────────────────────────────────────────
inline void secure_random(uint8_t *buf, size_t len) {
#ifdef _WIN32
  BCryptGenRandom(nullptr, buf, static_cast<ULONG>(len),
                  BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
  FILE *f = fopen("/dev/urandom", "rb");
  if (!f)
    throw std::runtime_error("Could not open /dev/urandom");
  if (fread(buf, 1, len, f) != len) {
    fclose(f);
    throw std::runtime_error("Short read from /dev/urandom");
  }
  fclose(f);
#endif
}

// ── Key derivation (Argon2i)
// ──────────────────────────────────────────────────
inline void derive_key(uint8_t key[32], const char *password,
                       const uint8_t salt[SALT_LEN]) {
  crypto_argon2_config cfg{};
  cfg.algorithm = CRYPTO_ARGON2_I;
  cfg.nb_blocks = ARGON2_BLOCKS;
  cfg.nb_passes = ARGON2_PASSES;
  cfg.nb_lanes = 1;

  crypto_argon2_inputs inputs{};
  inputs.pass = reinterpret_cast<const uint8_t *>(password);
  inputs.pass_size = static_cast<uint32_t>(strlen(password));
  inputs.salt = salt;
  inputs.salt_size = static_cast<uint32_t>(SALT_LEN);

  void *work = malloc(static_cast<size_t>(ARGON2_BLOCKS) * 1024);
  if (!work)
    throw std::runtime_error("Argon2 work area allocation failed");

  crypto_argon2(key, 32, work, cfg, inputs, crypto_argon2_no_extras);
  free(work);
}

// ── Encrypt
// ─────────────────────────────────────────────────────────────────── plaintext
// : null-terminated C string (your char* array) password  : master password
// (null-terminated) returns   : blob ready to write to disk
inline std::vector<uint8_t> encrypt(const char *plaintext,
                                    const char *password) {
  const size_t pt_len = strlen(plaintext);

  uint8_t salt[SALT_LEN];
  uint8_t nonce[NONCE_LEN];
  secure_random(salt, SALT_LEN);
  secure_random(nonce, NONCE_LEN);

  uint8_t key[32];
  derive_key(key, password, salt);

  std::vector<uint8_t> blob(HEADER_LEN + pt_len);
  uint8_t *p = blob.data();

  // Write salt and nonce into header
  memcpy(p, salt, SALT_LEN);
  p += SALT_LEN;
  memcpy(p, nonce, NONCE_LEN);
  p += NONCE_LEN;

  uint8_t *mac_slot = p;
  p += MAC_LEN;         // reserved for MAC
  uint8_t *ct_slot = p; // ciphertext goes here

  // Note: crypto_aead_lock writes ciphertext first, then mac
  crypto_aead_lock(ct_slot, mac_slot, key, nonce, nullptr, 0,
                   reinterpret_cast<const uint8_t *>(plaintext), pt_len);

  crypto_wipe(key, sizeof(key));
  return blob;
}

// ── Decrypt
// ─────────────────────────────────────────────────────────────────── blob :
// bytes read from disk password : master password (null-terminated) returns  :
// decrypted plaintext as std::string throws   : on wrong password or tampered
// data
inline std::string decrypt(const std::vector<uint8_t> &blob,
                           const char *password) {
  if (blob.size() < HEADER_LEN)
    throw std::runtime_error("Blob too short — file may be corrupted");

  const uint8_t *p = blob.data();
  const uint8_t *salt = p;
  p += SALT_LEN;
  const uint8_t *nonce = p;
  p += NONCE_LEN;
  const uint8_t *mac = p;
  p += MAC_LEN;
  const uint8_t *ct = p;
  const size_t ct_len = blob.size() - HEADER_LEN;

  uint8_t key[32];
  derive_key(key, password, salt);

  std::vector<uint8_t> pt(ct_len);

  int ok =
      crypto_aead_unlock(pt.data(), mac, key, nonce, nullptr, 0, ct, ct_len);

  crypto_wipe(key, sizeof(key));

  if (ok != 0)
    throw std::runtime_error(
        "Decryption failed — wrong password or tampered file");

  return std::string(pt.begin(), pt.end());
}

// ── File helpers
// ──────────────────────────────────────────────────────────────
#include <fstream>

inline void save_encrypted(const std::string &path, const char *plaintext,
                           const char *password) {
  auto blob = encrypt(plaintext, password);
  std::ofstream f(path, std::ios::binary | std::ios::trunc);
  if (!f)
    throw std::runtime_error("Cannot open file for writing: " + path);
  f.write(reinterpret_cast<const char *>(blob.data()),
          static_cast<std::streamsize>(blob.size()));
}

inline std::string load_decrypted(const std::string &path,
                                  const char *password) {
  std::ifstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("Cannot open file for reading: " + path);
  std::vector<uint8_t> blob((std::istreambuf_iterator<char>(f)),
                            std::istreambuf_iterator<char>());
  return decrypt(blob, password);
}

// Derive and cache key from password + fixed salt stored in PassMan.dat header
// Call once after master password is confirmed
inline void derive_key_once(uint8_t key[32], const char *password) {
  // Use a fixed salt stored separately (first 16 bytes of PassMan.dat)
  // For now use a constant salt — we'll store it in PassMan.dat header
  uint8_t salt[SALT_LEN];
  // salt is loaded/generated externally and passed in
  derive_key(key, password, salt);
}

// Encrypt a string using a pre-derived key (no Argon2, just AEAD)
inline std::vector<uint8_t> encrypt_with_key(const char *plaintext,
                                             const uint8_t key[32]) {
  const size_t pt_len = strlen(plaintext);
  uint8_t nonce[NONCE_LEN];
  secure_random(nonce, NONCE_LEN);

  std::vector<uint8_t> blob(NONCE_LEN + MAC_LEN + pt_len);
  uint8_t *p = blob.data();
  memcpy(p, nonce, NONCE_LEN);
  p += NONCE_LEN;
  uint8_t *mac_slot = p;
  p += MAC_LEN;

  crypto_aead_lock(p, mac_slot, key, nonce, nullptr, 0,
                   (const uint8_t *)plaintext, pt_len);
  return blob;
}

// Decrypt using pre-derived key
inline std::string decrypt_with_key(const std::vector<uint8_t> &blob,
                                    const uint8_t key[32]) {
  if (blob.size() < NONCE_LEN + MAC_LEN)
    throw std::runtime_error("Blob too short");

  const uint8_t *p = blob.data();
  const uint8_t *nonce = p;
  p += NONCE_LEN;
  const uint8_t *mac = p;
  p += MAC_LEN;
  const uint8_t *ct = p;
  const size_t ct_len = blob.size() - NONCE_LEN - MAC_LEN;

  std::vector<uint8_t> pt(ct_len);
  if (crypto_aead_unlock(pt.data(), mac, key, nonce, nullptr, 0, ct, ct_len) !=
      0)
    throw std::runtime_error("Decryption failed");
  return std::string(pt.begin(), pt.end());
}

// File helpers using pre-derived key
inline void save_encrypted_with_key(const std::string &path,
                                    const char *plaintext,
                                    const uint8_t key[32]) {
  auto blob = encrypt_with_key(plaintext, key);
  std::ofstream f(path, std::ios::binary | std::ios::trunc);
  if (!f)
    throw std::runtime_error("Cannot open: " + path);
  f.write((const char *)blob.data(), blob.size());
}

inline std::string load_decrypted_with_key(const std::string &path,
                                           const uint8_t key[32]) {
  std::ifstream f(path, std::ios::binary);
  if (!f)
    throw std::runtime_error("Cannot open: " + path);
  std::vector<uint8_t> blob((std::istreambuf_iterator<char>(f)),
                            std::istreambuf_iterator<char>());
  return decrypt_with_key(blob, key);
}
