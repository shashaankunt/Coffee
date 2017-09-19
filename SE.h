#include <iostream>
#include <bits/stdc++.h>
using namespace std;
class Clause {
public:
  int len;
  int * hgs;
  int base;
  int neck;
  int * xs;
  Clause( int len, int *hgs, int base, int neck,int *xs){
    this->hgs=hgs; // head+goals pointing to cells in cs
    this->base=base; // heap where this starts
    this->len=len; // length of heap slice
    this->neck=neck; // first after the end of the head
    this->xs= xs; // indexables in head
  }
};

