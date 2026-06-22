#pragma once
#include "../includes/PassMan.h"
#include "AddEditScreen.hpp"
#include "AddUserScreen.hpp"
#include "AppState.hpp"
#include "Dashboard.hpp"
#include "LoginScreen.hpp"
#include "SetMasterPasswordScreen.hpp"
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

inline void RunApp() {
  auto screen = ScreenInteractive::Fullscreen();
  AppState state;

  // Determine initial screen
  PassMan *pm = PassMan::getInstance();
  std::string home = std::getenv("HOME");
  std::ifstream test(home + "/.local/share/PassMan/PassMan.dat");
  state.screen = test.good() ? AppScreen::Login : AppScreen::SetMasterPassword;

  auto set_master_screen = SetMasterPasswordScreen(state);
  auto login_screen = LoginScreen(state);
  auto add_user_screen = AddUserScreen(state);
  auto dashboard = Dashboard(state);
  auto add_edit_screen = AddEditScreen(state);

  auto switcher = Renderer(Container::Tab(
                               {
                                   set_master_screen,
                                   login_screen,
                                   add_user_screen,
                                   dashboard,
                                   add_edit_screen,
                                   add_edit_screen,
                               },
                               reinterpret_cast<int *>(&state.screen)),
                           [&]() -> Element {
                             switch (state.screen) {
                             case AppScreen::SetMasterPassword:
                               return set_master_screen->Render();
                             case AppScreen::Login:
                               return login_screen->Render();
                             case AppScreen::AddUser:
                               return add_user_screen->Render();
                             case AppScreen::Dashboard:
                               return dashboard->Render();
                             case AppScreen::AddEntry:
                             case AppScreen::EditEntry:
                               return add_edit_screen->Render();
                             }
                             return text("");
                           });

  auto app = CatchEvent(switcher, [&](Event e) -> bool {
    if (e == Event::Escape) {
      switch (state.screen) {
      case AppScreen::AddEntry:
      case AppScreen::EditEntry:
        state.screen = AppScreen::Dashboard;
        return true;
      case AppScreen::AddUser:
        state.screen = AppScreen::Login;
        return true;
      case AppScreen::Dashboard:
        PassMan::current_user = nullptr;
        state.screen = AppScreen::Login;
        state.showPassword = false;
        state.statusMessage = "";
        return true;
      default:
        break;
      }
    }
    return false;
  });

  screen.Loop(app);
}
