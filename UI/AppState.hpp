#pragma once
#include <string>

// Which screen is currently active
enum class AppScreen {
  Login,
  Dashboard,
  AddEntry,
  EditEntry,
};

// Passed by reference into every screen so they can read/mutate shared state
struct AppState {
  AppScreen screen = AppScreen::Login;
  int selectedIndex = 0;     // highlighted service in left panel
  int editIndex = -1;        // which service is being edited (-1 = new)
  bool showPassword = false; // reveal toggle in detail panel
  std::string statusMessage =
      ""; // transient feedback ("Saved!", "Wrong password", …)
};
