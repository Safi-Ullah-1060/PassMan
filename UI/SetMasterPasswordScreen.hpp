#pragma once
#include "../includes/PassMan.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

inline Component SetMasterPasswordScreen(AppState &state) {
  auto pass_str = std::make_shared<std::string>();
  auto confirm_str = std::make_shared<std::string>();

  InputOption opt;
  opt.password = true;

  auto pass_input = Input(pass_str.get(), "master password", opt);
  auto confirm_input = Input(confirm_str.get(), "confirm password", opt);

  auto submit_btn =
      Button("Set Master Password", [&state, pass_str, confirm_str] {
        if (pass_str->empty()) {
          state.statusMessage = "Password cannot be empty.";
          return;
        }
        if (*pass_str != *confirm_str) {
          state.statusMessage = "Passwords do not match.";
          return;
        }
        // Set master password and save PassMan.dat
        PassMan *pm = PassMan::getInstance();
        pm->setMasterPassword(MyStr(pass_str->c_str()));
        pm->saveData();
        state.statusMessage = "";
        state.screen = AppScreen::Login;
      });

  auto layout = Container::Vertical({
      pass_input,
      confirm_input,
      submit_btn,
  });

  auto renderer =
      Renderer(layout, [&state, layout, pass_input, confirm_input, submit_btn] {
        return vbox({
            filler(),
            hbox({filler(),
                  vbox({
                      text("PassMan — First Launch") | bold | hcenter,
                      separator(),
                      text("Set a master password to encrypt your data.") |
                          dim | hcenter,
                      separator(),
                      text("Master Password") | dim,
                      pass_input->Render(),
                      text("Confirm Password") | dim,
                      confirm_input->Render(),
                      separator(),
                      submit_btn->Render() | hcenter,
                      text(state.statusMessage) | color(Color::Red) | hcenter,
                  }) | border |
                      size(WIDTH, EQUAL, 50),
                  filler()}),
            filler(),
        });
      });

  return renderer;
}
