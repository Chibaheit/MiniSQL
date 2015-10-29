#include "../src/common.h"
#include <cassert>
int main(){
    Value *x, *y, *z;
    x = new Int(9);
    y = new Int(-10);
    z = new Int(9);

    assert(*x == *z);
    assert(*z == *x);
    assert(*y < *x);
    assert(!(*x < *y));
    cout<<*x<<' '<<*y<<' '<<*z<<endl;

    int X;
    x->memoryCopy(&X);
    assert(X==9);
    cout<<X<<endl;

    assert(x->size()==4);

    delete x;
    delete y;
    delete z;
    cout<<"Int passed all test cases!"<<endl;
}
