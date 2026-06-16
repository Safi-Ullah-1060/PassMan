#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/MyStr.hpp>
#include <ftxui/screen/screen.hpp>
using namespace ftxui;
using namespace std;

#include "includes/PassMan.h"

int main2() {
  PassMan *master = PassMan::makeNew();
  bool success = 0;
  while (!success)
    success = master->login();
  User *currUser = master->current_user;

  currUser->addService();
  currUser->addService();
  currUser->addService();
  currUser->listServices();
  for (int i = 0; i < 3; i++) {
    currUser->viewService(i);
  }
  return 0;
}

int main3() {
  auto screen = ScreenInteractive::Fullscreen();
  std::MyStr user, pass;
  InputOption pw_opt;
  pw_opt.password = true;

  auto i_user = Input(&user, "username");
  auto i_pass = Input(&pass, "password", pw_opt);
  auto i_btn = Button("Login", [&] { /* ... */ });

  // Container::Vertical = Tab moves DOWN the list
  auto container = Container::Vertical({i_user, i_pass, i_btn});

  auto ui = Renderer(container, [&] {
    return vbox({
        i_user->Render() | border,
        i_pass->Render() | border,
        i_btn->Render() | center,
    });
  });

  screen.Loop(ui);
  return 0;
}

// Each page function takes a reference to `page` so it can navigate
Component make_login_page(int &page, std::MyStr &username) {
  auto input = Input(&username, "username");
  auto btn_login = Button("Login", [&] {
    if (!username.empty())
      page = 1;
  });
  auto container = Container::Vertical({input, btn_login});

  return Renderer(container, [=, &page, &username] {
    return vbox({
               text("Login") | bold | center,
               separator(),
               input->Render() | border,
               btn_login->Render() | center,
           }) |
           border;
  });
}

Component make_dashboard_page(int &page, const std::MyStr &username) {
  auto btn_logout = Button("Logout", [&] { page = 0; });

  return Renderer(btn_logout, [=, &page, &username] {
    return vbox({
               text("Welcome, " + username) | bold | center,
               separator(),
               btn_logout->Render() | center,
           }) |
           border;
  });
}

int main() {
  auto screen = ScreenInteractive::Fullscreen();
  int page = 0;
  std::MyStr username;

  auto p0 = make_login_page(page, username);
  auto p1 = make_dashboard_page(page, username);

  auto root = Container::Tab({p0, p1}, &page);
  screen.Loop(root);
}
