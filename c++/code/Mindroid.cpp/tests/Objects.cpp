#include <gtest/gtest.h>
#include "mindroid/lang/String.h"
#include "mindroid/lang/Integer.h"

using namespace mindroid;

TEST(Mindroid, Objects) {
    sp<Object> o1 = new Object();
    sp<Object> o2 = new Object();
    ASSERT_EQ(o1->equals(o2), false);
    sp<Object> o3 = o1;
    ASSERT_EQ(o1->equals(o3), true);
}
