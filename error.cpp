#include "error.hpp"
#include<iostream>
using namespace std;

void raise(int err) {
  cout << "Error no " << err;
  exit(err);
}
