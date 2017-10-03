
#include <bits/stdc++.h>
using namespace std;
class IntList {
private:
  const int * head;
  const IntList * tail;
  IntList(const int * head) {
    this->head = head;
    tail = NULL;}
  IntList(const int * X, const IntList * Xs) {
    head = X;
    tail = Xs;
  }
  };
