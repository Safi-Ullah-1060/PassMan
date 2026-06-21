#pragma once
#include "../includes/PassMan.h"
#include "../includes/User.h"
#include "AppState.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

using namespace ftxui;

inline Component Dashboard(AppState &state) {

  // ── Left panel: service list (Menu) ───────────────────────────────────────
  auto service_names = std::make_shared<std::vector<std::string>>();
  auto selected = std::make_shared<int>(0);

  // Sync the shared vector from PassMan before every render
  auto refresh_names = [service_names]() {
    service_names->clear();
    User *u = PassMan::current_user;
    if (!u)
      return;
    Vector<Service> *svcs = u->getServices();
    for (unsigned int i = 0; i < svcs->size(); ++i)
      service_names->push_back((*svcs)[i].getName().toStr());
  };

  MenuOption menu_opt = MenuOption::Vertical();
  menu_opt.entries_option.transform = [](const EntryState &s) {
    auto e = text(s.label);
    if (s.focused || s.active)
      e = e | bold | color(Color::Cyan);
    return e;
  };

  auto service_menu = Menu(service_names.get(), selected.get(), menu_opt);

  // ── Buttons ───────────────────────────────────────────────────────────────
  auto add_btn = Button("+ Add", [&state] {
    state.editIndex = -1;
    state.statusMessage = "";
    state.screen = AppScreen::AddEntry;
  });

  auto edit_btn = Button("Edit", [&state, selected] {
    state.editIndex = *selected;
    state.statusMessage = "";
    state.screen = AppScreen::EditEntry;
  });

  auto delete_btn = Button("Delete", [&state, selected, service_names] {
    User *u = PassMan::current_user;
    if (!u || service_names->empty())
      return;
    int idx = *selected;
    if (idx < 0 || idx >= static_cast<int>(service_names->size()))
      return;

    u->removeService(MyStr((*service_names)[idx].c_str()));
    PassMan::getInstance()->saveData();

    // Keep selection in bounds
    if (*selected >= static_cast<int>(service_names->size()) - 1)
      *selected = std::max(0, static_cast<int>(service_names->size()) - 2);

    state.statusMessage = "Deleted.";
  });

  auto reveal_btn = Button(
      "Show/Hide", [&state] { state.showPassword = !state.showPassword; });

  auto logout_btn = Button("Logout", [&state] {
    PassMan::current_user = nullptr;
    state.screen = AppScreen::Login;
    state.selectedIndex = 0;
    state.showPassword = false;
    state.statusMessage = "";
  });

  auto layout = Container::Vertical({
      service_menu,
      Container::Horizontal({add_btn, edit_btn, delete_btn}),
      reveal_btn,
      logout_btn,
  });

  // ── Renderer ──────────────────────────────────────────────────────────────
  auto renderer = Renderer(layout, [&state, layout, service_names, selected,
                                    refresh_names, add_btn, edit_btn,
                                    delete_btn, reveal_btn, logout_btn,
                                    service_menu] {
    refresh_names();
    state.selectedIndex = *selected;

    // ── Build detail panel ─────────────────────────────────────────────
    Elements detail_rows;
    User *u = PassMan::current_user;

    if (u && !service_names->empty() && *selected >= 0 &&
        *selected < static_cast<int>(service_names->size())) {
      Service *svc = u->getService(static_cast<unsigned int>(*selected));
      if (svc) {
        Data *d = svc->getData();

        auto row = [](const std::string &label, const std::string &value) {
          return hbox({
              text(label) | dim | size(WIDTH, EQUAL, 12),
              text(value) | flex,
          });
        };

        std::string pass_display =
            state.showPassword
                ? d->getPassword().toStr()
                : std::string(d->getPassword().toStr().size(), '*');

        detail_rows = {
            text(svc->getName().toStr()) | bold,
            separator(),
            row("Username", d->getUserName().toStr()),
            row("Email", d->getEmail().toStr()),
            row("Password", pass_display),
            separator(),
        };

        // Tokens
        Vector<Token *> *toks = svc->getTokensList();
        if (toks && toks->size() > 0) {
          detail_rows.push_back(text("Tokens") | dim);
          for (unsigned int i = 0; i < toks->size(); ++i) {
            Token *t = (*toks)[i];
            if (t)
              detail_rows.push_back(hbox({
                  text(t->getName().toStr()) | dim | size(WIDTH, EQUAL, 12),
                  text(t->getValue().toStr()) | flex,
              }));
          }
        } else {
          detail_rows.push_back(text("No tokens") | dim);
        }

        detail_rows.push_back(separator());
        detail_rows.push_back(hbox({
            edit_btn->Render() | size(WIDTH, EQUAL, 10),
            text("  "),
            delete_btn->Render() | size(WIDTH, EQUAL, 10),
            text("  "),
            reveal_btn->Render(),
        }));
      }
    } else {
      detail_rows = {text("No services yet. Press '+ Add'.") | dim | hcenter};
    }

    // ── Left panel ────────────────────────────────────────────────────
    auto left_panel =
        vbox({
            text("Services") | bold | hcenter,
            separator(),
            service_names->empty() ? text("(empty)") | dim | hcenter
                                   : service_menu->Render() | flex,
            filler(),
            separator(),
            add_btn->Render() | hcenter,
            logout_btn->Render() | hcenter,
        }) |
        size(WIDTH, EQUAL, 24) | border;

    // ── Right panel ───────────────────────────────────────────────────
    auto right_panel = vbox(detail_rows) | flex | border;

    // ── Status bar ────────────────────────────────────────────────────
    auto status_bar = hbox({
        text(state.statusMessage.empty() ? " " : state.statusMessage) |
            color(Color::Green),
        filler(),
        text("PassMan  "),
    });

    return vbox({
        hbox({left_panel, right_panel}) | flex,
        separator(),
        status_bar,
    });
  });

  return renderer;
}
