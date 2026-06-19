#pragma once
#include "MyStr.h"

class Token {
public:
  Token();
  Token(const Token &);
  Token &operator=(const Token &);
  MyStr getName();
  MyStr getValue();
  void setName(MyStr TokenName);
  void setValue(MyStr TokenValue);
  ~Token();

private:
  MyStr tokenName, tokenValue;
};
