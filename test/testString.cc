#include "../src/common.h"
#include <cassert>
const string a="Hello", b="World";
int main(){
    PValue x(new String(a)), y(new String(b)), z(new String("Hello"));

    assert(*x == *z);
    assert(*z == *x);
    assert(!(*y < *x));
    assert(*x < *y);
    cout<<*x<<' '<<*y<<' '<<*z<<endl;

    char X[100];
    x->memoryCopy(&X);
    assert(strcmp(X,a.data())==0);
    cout<<X<<endl;

    assert(x->size()==a.size());

    cout<<"String: OK!\n"<<endl;
}
