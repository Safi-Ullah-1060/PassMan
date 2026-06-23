#pragma once
#include "../includes/PassMan.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

inline Component AddUserScreen(AppState &state) {
  auto user_str = std::make_shared<std::string>();
  auto pass_str = std::make_shared<std::string>();
  auto confirm_str = std::make_shared<std::string>();

  InputOption opt;
  opt.password = true;

  auto user_input = Input(user_str.get(), "username");
  auto pass_input = Input(pass_str.get(), "password", opt);
  auto confirm_input = Input(confirm_str.get(), "confirm password", opt);

  auto submit_btn =
      Button("Add User", [&state, user_str, pass_str, confirm_str] {
        if (user_str->empty() || pass_str->empty()) {
          state.statusMessage = "Fields cannot be empty.";
          return;
        }
        if (*pass_str != *confirm_str) {
          state.statusMessage = "Passwords do not match.";
          return;
        }
        PassMan *pm = PassMan::getInstance();
        if (pm->userExists(MyStr(user_str->c_str())) != 0) {
          state.statusMessage = "Username already exists.";
          return;
        }
        pm->addUser(MyStr(user_str->c_str()), MyStr(pass_str->c_str()));
        pm->savePassManDat();
        state.statusMessage = "User added. Please log in.";
        state.screen = AppScreen::Login;
      });

  auto cancel_btn = Button("Cancel", [&state] {
    state.statusMessage = "";
    state.screen = AppScreen::Login;
  });

  auto layout = Container::Vertical({
      user_input,
      pass_input,
      confirm_input,
      Container::Horizontal({submit_btn, cancel_btn}),
  });

  auto renderer = Renderer(layout, [&state, layout, user_input, pass_input,
                                    confirm_input, submit_btn, cancel_btn] {
    return vbox({
        filler(),
        hbox({filler(),
              vbox({
                  text("Add User") | bold | hcenter,
                  separator(),
                  text("Username") | dim,
                  user_input->Render(),
                  text("Password") | dim,
                  pass_input->Render(),
                  text("Confirm Password") | dim,
                  confirm_input->Render(),
                  separator(),
                  hbox({
                      submit_btn->Render() | flex,
                      cancel_btn->Render() | flex,
                  }),
                  text(state.statusMessage) | color(Color::Red) | hcenter,
              }) | border |
                  size(WIDTH, EQUAL, 44),
              filler()}),
        filler(),
    });
  });

  return renderer;
}
