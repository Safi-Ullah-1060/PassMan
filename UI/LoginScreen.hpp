#pragma once
#include "../includes/PassMan.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

using namespace ftxui;

inline Component LoginScreen(AppState &state) {
  // Input fields
  auto username_str = std::make_shared<std::string>();
  auto password_str = std::make_shared<std::string>();

  InputOption pass_opt;
  pass_opt.password = true;

  auto username_input = Input(username_str.get(), "username");
  auto password_input = Input(password_str.get(), "master password", pass_opt);

  auto login_btn = Button("Unlock", [&state, username_str, password_str] {
    PassMan *pm = PassMan::getInstance();
    if (pm->login(MyStr(username_str->c_str()), MyStr(password_str->c_str()))) {
      state.screen = AppScreen::Dashboard;
      state.statusMessage = "";
    } else {
      state.statusMessage = "Wrong username or password.";
    }
  });

  auto layout = Container::Vertical({
      username_input,
      password_input,
      login_btn,
  });

  auto renderer = Renderer(layout, [&state, layout, username_str, password_str,
                                    username_input, password_input, login_btn] {
    return vbox({
        filler(),
        hbox({filler(),
              vbox({
                  text("PassMan") | bold | hcenter,
                  separator(),
                  text("Username") | dim,
                  username_input->Render(),
                  text("Master Password") | dim,
                  password_input->Render(),
                  separator(),
                  login_btn->Render() | hcenter,
                  // Status message (empty string renders as blank line)
                  text(state.statusMessage) | color(Color::Red) | hcenter,
              }) | border |
                  size(WIDTH, EQUAL, 40),
              filler()}),
        filler(),
    });
  });

  return renderer;
}
