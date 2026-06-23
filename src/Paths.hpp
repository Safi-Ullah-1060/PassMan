#include <filesystem>
namespace fs = std::filesystem;

inline std::string getBaseDir() {
#ifdef _WIN32
  const char *home = getenv("USERPROFILE");
  if (!home)
    home = getenv("APPDATA");
  return (fs::path(home) / "AppData" / "Local" / "PassMan").string() + "/";
#else
  return (fs::path(getenv("HOME")) / ".local" / "share" / "PassMan").string() +
         "/";
#endif
}

inline std::string getUserDataDir(const std::string &baseDir) {
  return (fs::path(baseDir) / "UserData").string() + "/";
}

inline std::string getUserDir(const std::string &userDataDir,
                              const std::string &username) {
  return (fs::path(userDataDir) / username).string() + "/";
}

inline std::string getUserServicesDir(const std::string &userDir) {
  return (fs::path(userDir) / "Services").string() + "/";
}
