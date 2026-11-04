#include <iostream>
#include <string>
// Deliberately broken experiments; never linked into ember.
int main(int argc,char** argv) {
 if(argc!=2)return 2;
 const std::string mode=argv[1];
 if(mode=="leak"){new int{42};return 0;}
 int* p=new int{42};
 if(mode=="fixed"){std::cout<<*p<<'\n';delete p;p=nullptr;return 0;}
 delete p;
 if(mode=="uaf")std::cout<<*p<<'\n';
 else if(mode=="double")delete p;
 else return 2;
}
