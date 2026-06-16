#pragma once

class Organization {
public:
  Organization();
  Organization(Organization &&) = default;
  Organization(const Organization &) = default;
  Organization &operator=(Organization &&) = default;
  Organization &operator=(const Organization &) = default;
  ~Organization();

private:
};
