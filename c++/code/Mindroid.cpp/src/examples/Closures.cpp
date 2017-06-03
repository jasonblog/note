#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

const char* const TAG = "Closures";

int main() {
    int32_t value1 = 123;
    int32_t value2 = 1234567;

    auto func1 = [] () { Log::d(TAG, "Hello World"); };
    func1();

    auto func2 = [] { Log::d(TAG, "Hello World"); };
    func2();

    auto func3 = [&] () { Log::d(TAG, "Hello World: %d", value1); };
    func3();

    auto func4 = [&] () { return value2; };
    Log::d(TAG, "Func4: %d", func4());

    std::function<void (void)> func5 = func3;
    func5();

    Looper::prepare();
    sp<Handler> handler = new Handler();
    handler->postDelayed([value1] { Log::d(TAG, "Hello Lambda: %d", value1); }, 100);
    sp<Closure> c = handler->postDelayed(func1, 500);
    c->cancel();
    c = handler->postDelayed(func3, 500);
    handler->postDelayed([] { Looper::myLooper()->quit(); }, 1000);
    Looper::loop();

    return 0;
}
