#pragma once
#include "../includes/PassMan.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

inline Component LoginScreen(AppState &state) {
  auto username_str = std::make_shared<std::string>();
  auto password_str = std::make_shared<std::string>();

  InputOption pass_opt;
  pass_opt.password = true;

  auto username_input = Input(username_str.get(), "username");
  auto password_input = Input(password_str.get(), "password", pass_opt);

  auto login_btn = Button("Login", [&state, username_str, password_str] {
    PassMan *pm = PassMan::getInstance();
    if (pm->login(MyStr(username_str->c_str()), MyStr(password_str->c_str()))) {
      state.statusMessage = "";
      state.screen = AppScreen::Dashboard;
    } else {
      state.statusMessage = "Incorrect username or password.";
    }
  });

  auto add_user_btn = Button("Add User", [&state] {
    state.statusMessage = "";
    state.screen = AppScreen::AddUser;
  });

  auto layout = Container::Vertical({
      username_input,
      password_input,
      Container::Horizontal({login_btn, add_user_btn}),
  });

  auto renderer = Renderer(layout, [&state, layout, username_input,
                                    password_input, login_btn, add_user_btn] {
    return vbox({
        filler(),
        hbox({filler(),
              vbox({
                  text("PassMan") | bold | hcenter,
                  separator(),
                  text("Username") | dim,
                  username_input->Render(),
                  text("Password") | dim,
                  password_input->Render(),
                  separator(),
                  hbox({
                      login_btn->Render() | flex,
                      add_user_btn->Render() | flex,
                  }),
                  text(state.statusMessage) | color(Color::Red) | hcenter,
              }) | border |
                  size(WIDTH, EQUAL, 40),
              filler()}),
        filler(),
    });
  });

  return renderer;
}
