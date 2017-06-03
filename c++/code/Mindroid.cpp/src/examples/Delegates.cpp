#include "mindroid/util/Delegate.h"
#include "mindroid/util/Event.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

class Test {
public:
    void test1(int32_t value) {
        Log::d("Test", "test1 with value %d", value);
    }

    void test2(int32_t value) {
        Log::d("Test", "test2 with value %d", value);
    }

    Event<void (int32_t)> testIt;
};

int main() {
    Test test;

    Delegate<void (int32_t)> delegate1 = Delegate<void (int32_t)>::create<Test, &Test::test1>(test);
    Delegate<void (int32_t)> delegate2 = Delegate<void (int32_t)>::create<Test, &Test::test2>(test);

    test.testIt += delegate1;
    test.testIt(1234567);

    test.testIt -= delegate1;
    test.testIt(1234);

    test.testIt += delegate1;
    test.testIt(1234);

    test.testIt += delegate2;
    test.testIt(1234);

    test.testIt -= delegate1;
    test.testIt(1234);

    test.testIt -= delegate1;
    test.testIt(1234);

    test.testIt -= delegate2;
    test.testIt(1234);

    return 0;
}
