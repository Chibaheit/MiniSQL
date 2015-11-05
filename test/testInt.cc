#include "../src/common.h"
#include <cassert>
int main(){
    debug("Testing Int class...\n");
    PValue x(new Int(9)), y(new Int(-10)), z(new Int(9));

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

    cout<<"Int: OK!\n"<<endl;
}
