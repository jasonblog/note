#include <gtest/gtest.h>
#include "mindroid/io/File.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

TEST(Mindroid, Files) {
    sp<File> file = new File("/mindroid/test/");
    ASSERT_STREQ(file->getName()->c_str(), "test");
    ASSERT_STREQ(file->getPath()->c_str(), "/mindroid/test");
    ASSERT_STREQ(file->getParent()->c_str(), "/mindroid");
}

TEST(Mindroid, Root) {
    sp<File> file = new File("/");
    ASSERT_STREQ(file->getName()->c_str(), "");
    ASSERT_STREQ(file->getPath()->c_str(), "/");
    ASSERT_EQ(file->getParent(), nullptr);
}
