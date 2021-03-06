/*************************************************************************
	> File Name: testTypeId.cpp
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月23日 星期六 11时49分31秒
 ************************************************************************/

#include<iostream>
#include<algorithm>
#include<vector>
#include <typeinfo>  
using namespace std;  

class Class1{};
class Class2:public Class1{};
void fn0();
int fn1(int n);

int main(void)  {  
    int a = 10;
    int* b = &a;
    float c;
    double d;

    cout << typeid(a).name() << endl;
    cout << typeid(b).name() << endl;
    cout << typeid(c).name() << endl;
    cout << typeid(d).name() << endl;
    cout << typeid(Class1).name() << endl;
    cout << typeid(Class2).name() << endl;
    cout << typeid(fn0).name() << endl;
    cout << typeid(fn1).name() << endl;
    cout << typeid(typeid(a).name()).name() << endl;
}  

