#pragma once
#include "../includes/PassMan.h"
#include "../includes/User.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

using namespace ftxui;

inline Component AddEditScreen(AppState &state) {
  auto name_str = std::make_shared<std::string>();
  auto user_str = std::make_shared<std::string>();
  auto email_str = std::make_shared<std::string>();
  auto pass_str = std::make_shared<std::string>();

  // Pre-fill fields if editing an existing service
  auto prefill = [&state, name_str, user_str, email_str, pass_str] {
    if (state.editIndex < 0) {
      // Adding — clear everything
      *name_str = "";
      *user_str = "";
      *email_str = "";
      *pass_str = "";
      return;
    }
    User *u = PassMan::current_user;
    if (!u)
      return;
    Service *svc = u->getService(static_cast<unsigned int>(state.editIndex));
    if (!svc)
      return;
    *name_str = svc->getName().toStr();
    Data *d = svc->getData();
    *user_str = d->getUserName().toStr();
    *email_str = d->getEmail().toStr();
    *pass_str = d->getPassword().toStr();
  };

  InputOption pass_opt;
  pass_opt.password = true;

  auto name_input = Input(name_str.get(), "e.g. GitHub");
  auto user_input = Input(user_str.get(), "e.g. username");
  auto email_input = Input(email_str.get(), "e.g. user@example.com");
  auto pass_input = Input(pass_str.get(), "userpswd", pass_opt);

  auto save_btn =
      Button("Save", [&state, name_str, user_str, email_str, pass_str] {
        User *u = PassMan::current_user;
        if (!u)
          return;

        if (state.editIndex < 0) {
          // ── ADD ──────────────────────────────────────────────────────────
          u->addService(MyStr(name_str->c_str()), MyStr(user_str->c_str()),
                        MyStr(email_str->c_str()), MyStr(pass_str->c_str()));
          // Select the new service in the dashboard
          Vector<Service> *svcs = u->getServices();
          state.selectedIndex = static_cast<int>(svcs->size()) - 1;
        } else {
          // ── EDIT ─────────────────────────────────────────────────────────
          Service *svc =
              u->getService(static_cast<unsigned int>(state.editIndex));
          if (!svc)
            return;
          svc->setName(MyStr(name_str->c_str()));
          Data *d = svc->getData();
          d->setUserName(MyStr(user_str->c_str()));
          d->setEmail(MyStr(email_str->c_str()));
          d->setPassword(MyStr(pass_str->c_str()));
          state.selectedIndex = state.editIndex;
        }

        PassMan::getInstance()->saveData();
        state.statusMessage = "Saved!";
        state.screen = AppScreen::Dashboard;
      });

  auto cancel_btn = Button("Cancel", [&state] {
    state.statusMessage = "";
    state.screen = AppScreen::Dashboard;
  });

  auto layout = Container::Vertical({
      name_input,
      user_input,
      email_input,
      pass_input,
      Container::Horizontal({save_btn, cancel_btn}),
  });

  auto renderer =
      Renderer(layout, [&state, layout, name_str, user_str, email_str, pass_str,
                        prefill, name_input, user_input, email_input,
                        pass_input, save_btn, cancel_btn] {
        // Prefill every render so edits are loaded when screen first opens
        static AppScreen last_screen = AppScreen::Login;
        if (last_screen != state.screen) {
          prefill();
          last_screen = state.screen;
        }

        std::string title = (state.editIndex < 0) ? "Add Entry" : "Edit Entry";

        return vbox({
            filler(),
            hbox({filler(),
                  vbox({
                      text(title) | bold | hcenter,
                      separator(),
                      text("Service name") | dim,
                      name_input->Render(),
                      separator(),
                      text("Username") | dim,
                      user_input->Render(),
                      text("Email") | dim,
                      email_input->Render(),
                      text("Password") | dim,
                      pass_input->Render(),
                      separator(),
                      hbox({
                          save_btn->Render() | flex,
                          cancel_btn->Render() | flex,
                      }),
                  }) | border |
                      size(WIDTH, EQUAL, 50),
                  filler()}),
            filler(),
        });
      });

  return renderer;
}
