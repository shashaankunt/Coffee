
#include <bits/stdc++.h>
using namespace std;
class IntList {
private:
   int head=0;
   int tail=0;
   int abc=0;
  IntList(int head) ;
  IntList( int X,  int Xs) ;


  static IntList cons(int X,int Xs);

  static IntList app(int y ,int Ys);

  static int toInts(int Xs);
  static int len(int Xs);
};
int main()
{
   return 0;
}
