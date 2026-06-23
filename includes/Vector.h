#pragma once
#include <iostream>
using namespace std;

template <class T> class Vector {
private:
  T *data;
  int sz;
  int cap;

  void grow() {
    int newCap = (cap == 0) ? 1 : cap * 2;
    T *newData = new T[newCap];
    for (int i = 0; i < sz; i++)
      newData[i] = data[i];
    delete[] data;
    data = newData;
    cap = newCap;
  }

public:
  Vector() {
    data = nullptr;
    sz = 0;
    cap = 0;
  }

  Vector(const Vector &other) {
    sz = other.sz;
    cap = other.cap;
    data = new T[cap];
    for (int i = 0; i < sz; i++)
      data[i] = other.data[i];
  }

  Vector &operator=(const Vector &other) {
    if (this == &other)
      return *this;
    delete[] data;
    sz = other.sz;
    cap = other.cap;
    data = new T[cap];
    for (int i = 0; i < sz; i++)
      data[i] = other.data[i];
    return *this;
  }
  ~Vector() { delete[] data; }

  void push_back(const T &val) {
    if (sz == cap)
      grow();
    data[sz++] = val;
  }

  void pop_back() {
    if (sz > 0)
      sz--;
  }

  void insert(int index, const T &val) {
    if (index < 0 || index > sz)
      return;
    if (sz == cap)
      grow();
    for (int i = sz; i > index; i--)
      data[i] = data[i - 1];
    data[index] = val;
    sz++;
  }

  void clear() { sz = 0; }

  void remove_at(int index) {
    if (index < 0 || index >= sz)
      return;
    for (int i = index; i < sz - 1; i++)
      data[i] = data[i + 1];
    sz--;
  }

  void remove(T val) {
    for (int i = 0; i < sz; i++) {
      if (data[i] == val) {
        remove_at(i);
        break;
      }
    }
  }

  T &operator[](int index) { return data[index]; }

  const int &operator[](int index) const { return data[index]; }

  int size() const { return sz; }

  int capacity() const { return cap; }

  friend ostream &operator<<(ostream &out, const Vector &v) {
    out << "[";
    for (int i = 0; i < v.sz; i++) {
      out << v.data[i];
      if (i < v.sz - 1)
        out << ", ";
    }
    out << "]";
    return out;
  }
  friend istream &operator>>(istream &in, Vector &v) {
    delete[] v.data;
    v.sz = 0;
    v.cap = 0;
    v.data = nullptr;

    int n;
    in >> n;
    for (int i = 0; i < n; i++) {
      T val;
      in >> val;
      v.push_back(val);
    }
    return in;
  }
};
