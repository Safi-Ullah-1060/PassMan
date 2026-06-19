#include "../includes/Token.h"

Token::Token() : tokenName(), tokenValue() {}
Token::Token(const Token &t)
    : tokenName(t.tokenName), tokenValue(t.tokenName) {}
Token &Token::operator=(const Token &t) {
  if (this == &t)
    return *this;
  tokenName = t.tokenName;
  tokenValue = t.tokenValue;
  return *this;
}
MyStr Token::getName() { return tokenName; }
MyStr Token::getValue() { return tokenValue; }
void Token::setName(MyStr name) { tokenName = name; }
void Token::setValue(MyStr val) { tokenValue = val; }
Token::~Token() {}
