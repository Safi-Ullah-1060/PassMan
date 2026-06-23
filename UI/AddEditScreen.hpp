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
  auto error_msg = std::make_shared<std::string>();
  auto tab_index = std::make_shared<int>(0); // 0=add, 1=edit

  auto prefill = [&state, name_str, user_str, email_str, pass_str, error_msg,
                  tab_index] {
    *error_msg = "";
    if (state.editIndex < 0) {
      *tab_index = 0;
      *name_str = *user_str = *email_str = *pass_str = "";
      return;
    }
    *tab_index = 1;
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

  // Save/cancel lambdas defined once, reused by both layouts
  auto save_action = [&state, name_str, user_str, email_str, pass_str,
                      error_msg]() {
    if (state.editIndex < 0 && name_str->empty()) {
      *error_msg = "Service name cannot be empty.";
      return;
    }
    if (user_str->empty()) {
      *error_msg = "Username cannot be empty.";
      return;
    }
    if (email_str->empty()) {
      *error_msg = "Email cannot be empty.";
      return;
    }
    if (pass_str->empty()) {
      *error_msg = "Password cannot be empty.";
      return;
    }

    User *u = PassMan::current_user;
    if (!u)
      return;

    if (state.editIndex < 0) {
      if (u->findService(MyStr(name_str->c_str())) != 0) {
        *error_msg = "A service with that name already exists.";
        return;
      }
      u->addService(MyStr(name_str->c_str()), MyStr(user_str->c_str()),
                    MyStr(email_str->c_str()), MyStr(pass_str->c_str()));
      state.selectedIndex = static_cast<int>(u->getServices()->size()) - 1;
    } else {
      Service *svc = u->getService(static_cast<unsigned int>(state.editIndex));
      if (!svc)
        return;
      Data *d = svc->getData();
      d->setUserName(MyStr(user_str->c_str()));
      d->setEmail(MyStr(email_str->c_str()));
      d->setPassword(MyStr(pass_str->c_str()));
      state.selectedIndex = state.editIndex;
    }

    *error_msg = "";
    PassMan::getInstance()->saveData();
    state.statusMessage = "Saved!";
    state.screen = AppScreen::Dashboard;
  };

  auto cancel_action = [&state, error_msg]() {
    *error_msg = "";
    state.statusMessage = "";
    state.screen = AppScreen::Dashboard;
  };

  InputOption pass_opt;
  pass_opt.password = true;

  // ── Add layout — owns its own component instances ─────────────────────────
  auto a_name = Input(name_str.get(), "e.g. GitHub");
  auto a_user = Input(user_str.get(), "e.g. username");
  auto a_email = Input(email_str.get(), "e.g. user@example.com");
  auto a_pass = Input(pass_str.get(), "password", pass_opt);
  auto a_save = Button("Save", save_action);
  auto a_cancel = Button("Cancel", cancel_action);

  auto add_layout = Container::Vertical({
      a_name,
      a_user,
      a_email,
      a_pass,
      Container::Horizontal({a_save, a_cancel}),
  });

  // ── Edit layout — separate instances, same string backing ─────────────────
  auto e_user = Input(user_str.get(), "e.g. username");
  auto e_email = Input(email_str.get(), "e.g. user@example.com");
  auto e_pass = Input(pass_str.get(), "password", pass_opt);
  auto e_save = Button("Save", save_action);
  auto e_cancel = Button("Cancel", cancel_action);

  auto edit_layout = Container::Vertical({
      e_user,
      e_email,
      e_pass,
      Container::Horizontal({e_save, e_cancel}),
  });

  auto layout = Container::Tab({add_layout, edit_layout}, tab_index.get());

  auto renderer = Renderer(
      layout, [&state, layout, name_str, user_str, email_str, pass_str,
               error_msg, prefill, tab_index, a_name, a_user, a_email, a_pass,
               a_save, a_cancel, e_user, e_email, e_pass, e_save, e_cancel] {
        static int last_edit_index = -99;
        static AppScreen last_screen = AppScreen::Login;

        bool screen_changed = (last_screen != state.screen);
        bool edit_changed = (state.editIndex != last_edit_index);

        if (screen_changed || edit_changed) {
          prefill();
          last_screen = state.screen;
          last_edit_index = state.editIndex;
          if (state.editIndex < 0)
            a_name->TakeFocus();
          else
            e_user->TakeFocus();
        }

        std::string title = (state.editIndex < 0) ? "Add Entry" : "Edit Entry";

        auto field_color = [](const std::string &val) {
          return val.empty() ? color(Color::Red) : color(Color::Default);
        };

        // ── Body ──────────────────────────────────────────────────────
        Elements body;
        if (state.editIndex < 0) {
          body = {
              text("Service name") | dim | field_color(*name_str),
              a_name->Render(),
              separator(),
              text("Username") | dim | field_color(*user_str),
              a_user->Render(),
              text("Email") | dim | field_color(*email_str),
              a_email->Render(),
              text("Password") | dim | field_color(*pass_str),
              a_pass->Render(),
              separator(),
              hbox({a_save->Render() | flex, a_cancel->Render() | flex}),
          };
        } else {
          body = {
              hbox({
                  text("Service") | dim | size(WIDTH, EQUAL, 14),
                  text(*name_str) | bold,
              }),
              separator(),
              text("Username") | dim | field_color(*user_str),
              e_user->Render(),
              text("Email") | dim | field_color(*email_str),
              e_email->Render(),
              text("Password") | dim | field_color(*pass_str),
              e_pass->Render(),
              separator(),
              hbox({e_save->Render() | flex, e_cancel->Render() | flex}),
          };
        }

        body.push_back(separator());
        body.push_back(text(error_msg->empty() ? " " : *error_msg) |
                       color(Color::Red) | hcenter);

        return vbox({
            filler(),
            hbox({
                filler(),
                vbox({
                    text(title) | bold | hcenter,
                    separator(),
                    vbox(body),
                }) | border |
                    size(WIDTH, EQUAL, 50),
                filler(),
            }),
            filler(),
        });
      });

  return renderer;
}
