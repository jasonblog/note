# shared_ptr 向下轉型


```cpp
#include <iostream>
#include <vector>
#include <memory>
using namespace std;

class RealVector {

};

class ArrayRealVector : public RealVector {
    public:
        ArrayRealVector();
        std::shared_ptr<std::vector<double> > data;
};

ArrayRealVector::ArrayRealVector() {
    data = shared_ptr<std::vector<double> >(new std::vector<double>(10));
}


int main(int argc, char *argv[])
{
    std::shared_ptr<RealVector> p = std::shared_ptr<RealVector>(new ArrayRealVector());
    // static_cast<std::shared_ptr<ArrayRealVector> >(p)->data;
    cout << std::static_pointer_cast<ArrayRealVector>(p)->data->at(0) << endl;
    
    return 0;
}
```