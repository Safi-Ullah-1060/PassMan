#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "AddEditScreen.hpp"
#include "AppState.hpp"
#include "Dashboard.hpp"
#include "LoginScreen.hpp"

using namespace ftxui;

inline void RunApp() {
  auto screen = ScreenInteractive::Fullscreen();
  AppState state;

  // Build all screens once — they hold their own input state
  // and read/write the shared AppState by reference
  auto login_screen = LoginScreen(state);
  auto dashboard = Dashboard(state);
  auto add_edit_screen = AddEditScreen(state);

  // The root component switches between screens based on state.screen.
  // CatchEvent lets us handle global keys (Esc to go back, etc.)
  auto root = Make<ComponentBase>();

  auto switcher =
      Renderer(Container::Tab(
                   {
                       login_screen,
                       dashboard,
                       add_edit_screen,
                       add_edit_screen, // EditEntry reuses the same component
                   },
                   reinterpret_cast<int *>(&state.screen)),
               [&state, login_screen, dashboard, add_edit_screen]() -> Element {
                 switch (state.screen) {
                 case AppScreen::Login:
                   return login_screen->Render();
                 case AppScreen::Dashboard:
                   return dashboard->Render();
                 case AppScreen::AddEntry:
                 case AppScreen::EditEntry:
                   return add_edit_screen->Render();
                 }
                 return text(""); // unreachable
               });

  // Global key handler
  auto app = CatchEvent(switcher, [&](Event e) -> bool {
    // Esc from add/edit → back to dashboard
    if (e == Event::Escape && (state.screen == AppScreen::AddEntry ||
                               state.screen == AppScreen::EditEntry)) {
      state.screen = AppScreen::Dashboard;
      return true;
    }
    // Esc from dashboard → logout
    if (e == Event::Escape && state.screen == AppScreen::Dashboard) {
      PassMan::current_user = nullptr;
      state.screen = AppScreen::Login;
      state.showPassword = false;
      state.statusMessage = "";
      return true;
    }
    return false;
  });

  screen.Loop(app);
}
