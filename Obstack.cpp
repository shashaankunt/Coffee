#include<iostream>
using namespace std;
class Obstack{
public:
  void pop()
  {
    cout<<"sad";
    cout<<"Pop operations";
  }
  int push();
   int peek();
};
int main()
{
  Obstack b;
  b.pop();
  return 0;
}
