#include <gtest/gtest.h>
#include "mindroid/lang/Object.h"

using namespace mindroid;

class Test : public Object {
public:
    Test(int32_t id) : mId(id) {
    }

    virtual ~Test() {
    }

    int32_t id() const {
        return mId;
    }

private:
    int32_t mId;
};

TEST(Mindroid, ReferenceCounting) {
    sp<::Test> ref1 = new ::Test(1);

    {
        sp<::Test> ref2 = new ::Test(2);
    }

    wp<::Test> ref3 = ref1;
    sp<::Test> ref31 = ref3.lock();
    ASSERT_NE(ref31, nullptr);
    ref31 = nullptr;

    ref1 = nullptr;
    ref31 = ref3.lock();
    ASSERT_EQ(ref31, nullptr);

    wp<::Test> ref4 = new ::Test(4);
    ASSERT_NE(ref4, nullptr);
    wp<::Test> ref5 = ref4;
    ASSERT_NE(ref5, nullptr);
    ref4 = nullptr;
    ASSERT_EQ(ref4, nullptr);
    ASSERT_NE(ref5.lock(), nullptr);
    sp<::Test> ref6 = ref5.lock();
    ref6 = nullptr;
    ASSERT_NE(ref5, nullptr);
    ASSERT_EQ(ref5.lock(), nullptr);
}
