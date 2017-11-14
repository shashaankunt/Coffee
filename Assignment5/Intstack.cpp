#include<vector>
#include<iostream>
using namespace std;
class IntStack{
public: 
	vector<int> stack;
	int top;
 int SIZE=16;
	int minsize =1<<(SIZE-1);
IntStack(){
	IntStack(SIZE);
}
IntStack(int SIZE){
	stack.reserve(SIZE);
	clear();
}
int getTop() {
	return top;
}
int setTop(int top) {
        return this->top = top;
}
void clear() {
        top = -1;
}
bool isEmpty() {
        return top < 0;
}
void push(int i){
	if(++top >=stack.size())
		expand();
	stack[top]=i;
}
int pop(){
	int r=stack[top--];
	shrink();
	return r;
}
int get(int i){
	return stack[i];
}
void set(int i,int val){
	stack[i]=val;
}
int size(){
	return top+1;
}
void expand(){
	int l=stack.size();
	stack.resize(l*4);
}
void shrink(){
	int l=stack.size();
	if(l<=minsize || top<<2 >=1)
		return;
	l=1+(top<< 1);
	if(top <minsize)
		l=minsize;
	stack.resize(l);
}
vector<int> toArray(){
	vector<int> array;
	if(size() > 0)
		array=stack;
	return array;
}
void reverse(){

	int l=size();
	int h=l>>1;
	for(int i=0;i<h;i++){
		int temp=stack[i];
		stack[i]=stack[l-i-1];
		stack[l-i-1]=temp;
	}
}
};
int main()
{
IntStack s(10);
s.push(100);
s.push(200);
s.push(300);
s.push(400);
s.push(500);
cout<<"size of stack is:";
cout<<s.getTop()+1<<endl;
}
