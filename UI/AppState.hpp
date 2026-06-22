#pragma once
#include <string>

enum class AppScreen {
  SetMasterPassword, // first launch
  Login,
  AddUser, // from login screen
  Dashboard,
  AddEntry,
  EditEntry,
};

struct AppState {
  AppScreen screen = AppScreen::Login;
  int selectedIndex = 0;
  int editIndex = -1;
  bool showPassword = false;
  std::string statusMessage = "";
};
