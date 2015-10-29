#include "../src/common.h"
#include <cassert>
const float a=2.3, b=-4.5;
int main(){
    Value *x, *y, *z;
    x = new Float(a);
    y = new Float(b);
    z = new Float(a);

    assert(*x == *z);
    assert(*z == *x);
    assert(*y < *x);
    assert(!(*x < *y));
    cout<<*x<<' '<<*y<<' '<<*z<<endl;

    float X;
    x->memoryCopy(&X);
    assert(X==a);
    cout<<X<<endl;

    assert(x->size()==4);

    delete x;
    delete y;
    delete z;
    cout<<"Float passed all test cases!"<<endl;
}
