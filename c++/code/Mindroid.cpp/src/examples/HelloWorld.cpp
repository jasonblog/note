#include <cstdio>
#include "mindroid/os/Message.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/lang/String.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

static const int SAY_HELLO = 0;
static const int SAY_WORLD = 1;

class HelloHandler : public Handler {
public:
    HelloHandler(const sp<Handler>& worldHandler) :
            mWorldHandler(worldHandler) {
    }

    virtual void handleMessage(const sp<Message>& msg) {
        switch (msg->what) {
        case SAY_HELLO:
            printf("Hello ");
            sp<Message> message = mWorldHandler->obtainMessage(SAY_WORLD);
            message->getData()->putObject("Handler", this);
            message->sendToTarget();
            break;
        }
    }

private:
    sp<Handler> mWorldHandler;
};

class WorldHandler : public Handler {
public:
    WorldHandler() {
    }

    virtual void handleMessage(const sp<Message>& msg) {
        switch (msg->what) {
        case SAY_WORLD:
            printf("World!\n");
            sp<Handler> helloHandler = object_cast<Handler>(msg->getData()->getObject("Handler"));
            sp<Message> message = helloHandler->obtainMessage(SAY_HELLO);
            helloHandler->sendMessageDelayed(message, 1000);
            break;
        }
    }
};

int main() {
    Looper::prepare();
    sp<Handler> worldHandler = new WorldHandler();
    sp<Handler> helloHandler = new HelloHandler(worldHandler);
    helloHandler->obtainMessage(SAY_HELLO)->sendToTarget();
    sp<Handler> handler = new Handler();
    Looper::loop();

    return 0;
}
