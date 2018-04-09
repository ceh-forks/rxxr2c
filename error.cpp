#include "error.hpp"
#include<iostream>
#include<cstdlib>
using namespace std;

void raise(int err) {
  cout << "Error no " << err;
  exit(err);
}
