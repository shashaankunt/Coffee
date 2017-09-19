
#include <iostream>
#include <bits/stdc++.h>
using namespace std;
class Clause {
  Clause(final int len, final int[] hgs, final int base, final int neck, final int[] xs) {
    this.hgs = hgs; // head+goals pointing to cells in cs
    this.base = base; // heap where this starts
    this.len = len; // length of heap slice
    this.neck = neck; // first after the end of the head
    this.xs = xs; // indexables in head
  }

  final int len;
  final int[] hgs;
  final int base;
  final int neck;
  final int[] xs;
};
int main() {
  Clause noob(10,[1,2],2,4,[20,14,5,6]);
  cout<<"noob";
  /* code */
  return 0;
}
