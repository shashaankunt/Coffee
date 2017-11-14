#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <stdexcept>

using namespace std;
template <class T>
class ObStack {
   private:
      vector<T> elems;

   public:
      void push(T const&);
            void pop();
                 T top() const;
      bool empty() const {
         return elems.empty();
      }
};
template <class T>
T ObStack<T>::top () const {
   if (elems.empty()) {
      throw out_of_range("Stack<>::top(): empty stack");
   }
      return elems.back();
}

template <class T>
void ObStack<T>::push (T const& elem) {

   elems.push_back(elem);
}

template <class T>
void ObStack<T>::pop () {
   if (elems.empty()) {
      throw out_of_range("Stack<>::pop(): empty stack");
   }

 elems.pop_back();
}


int main() {
   try {

      ObStack<string> Objectinst;
     Objectinst.push("1");
	  Objectinst.push("2");
	  Objectinst.push("3");
	  Objectinst.push("4");
      cout << "Top Item: "<<Objectinst.top() << std::endl;
      Objectinst.pop();
       cout <<"Top After pop "<< Objectinst.top() << std::endl;

   } catch (exception const& ex) {
      cerr << "Exception: " << ex.what() <<endl;
      return -1;
   }
}
//}
