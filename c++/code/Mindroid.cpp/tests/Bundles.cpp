#include <gtest/gtest.h>
#include "mindroid/os/Bundle.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

class Test :
    public Object
{
public:
    Test(uint32_t value) : mValue(value) { }
    virtual ~Test() { }
    uint32_t getValue() { return mValue; }

private:
    uint32_t mValue;
};

TEST(Mindroid, Bundle) {
    sp<Bundle> bundle = new Bundle();
    bundle->putBoolean("1", true);
    ASSERT_EQ(bundle->getBoolean("1", false), true);

    bundle->putInt("2", 12345);
    ASSERT_EQ(bundle->getInt("2", 0), 12345);
    ASSERT_EQ(bundle->getLong("2", 0), 0);

    bundle->putObject("3", new ::Test(17));
    sp<::Test> test = object_cast<::Test>(bundle->getObject("3"));
    ASSERT_EQ(test->getValue(), 17);

    bundle->putString("4", "Test");
    sp<String> string = bundle->getString("4");
    ASSERT_EQ(string->equals("Test"), true);
}
