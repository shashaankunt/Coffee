#include <iostream>
#include <bits/stdc++.h>
using namespace std;
class Clause 
{
public:
  int len;
  int * hgs;
  int base;
  int neck;
  int * xs;
  Clause( int len, int *hgs, int base, int neck,int *xs)
  {
    this->hgs=hgs; // head+goals pointing to cells in cs
    this->base=base; // heap where this starts
    this->len=len; // length of heap slice
    this->neck=neck; // first after the end of the head
    this->xs= xs; // indexables in head
  }
};
int main()
{
int str1[2]={1,2};
int str3[2]={1,2};
int x;
Clause b(10,&str1[0],1,1,&str3[0]);4
  cout<<"Length of the clause";
  cout<<b.len;
  /* code */
  cin>>x;
  return 0;
}

