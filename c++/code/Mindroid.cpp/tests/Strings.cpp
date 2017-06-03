#include <gtest/gtest.h>
#include "mindroid/lang/String.h"
#include "mindroid/lang/Integer.h"

using namespace mindroid;

TEST(Mindroid, Strings) {
    sp<String> s1 = new String("Hello");
    ASSERT_STREQ(s1->c_str(), "Hello");
    sp<String> s2 = s1->append(" World");
    ASSERT_STREQ(s1->c_str(), "Hello");
    ASSERT_STREQ(s2->c_str(), "Hello World");
    sp<String> s3 = s2->append("!");
    ASSERT_STREQ(s1->c_str(), "Hello");
    ASSERT_STREQ(s2->c_str(), "Hello World");
    ASSERT_STREQ(s3->c_str(), "Hello World!");
    s1 = new String("Test");
    ASSERT_STREQ(s1->c_str(), "Test");

    sp<String> s4 = String::format("%d", 123);
    ASSERT_STREQ(s4->c_str(), "123");
    s4 = s4->appendFormatted("%s%d", "456", 789);
    ASSERT_STREQ(s4->c_str(), "123456789");

    sp<String> s5 = s4;
    s4 = nullptr;
    ASSERT_STREQ(s5->c_str(), "123456789");

    sp<String> s6 = new String("    1234    ");
    ASSERT_EQ(s6->length(), 12);
    ASSERT_STREQ(s6->c_str(), "    1234    ");
    sp<String> s7 = s6->trim();
    ASSERT_EQ(s7->length(), 4);
    ASSERT_STREQ(s7->c_str(), "1234");
    ASSERT_STREQ(s6->c_str(), "    1234    ");

    sp<String> s8 = new String("\r\n");
    ASSERT_EQ(s8->length(), 2);
    ASSERT_EQ(s8->equals("\r\n"), true);
    sp<String> s9 = s8->trim();
    ASSERT_EQ(s9->length(), 0);
    ASSERT_EQ(s9->equals(""), true);
    ASSERT_EQ(s9 == String::EMPTY_STRING, true);

    sp<String> s10 = new String();
    ASSERT_EQ(s10->isEmpty(), true);

    sp<String> s11 = new String("Test");
    ASSERT_EQ(s11->startsWith("Te"), true);
    ASSERT_EQ(s11->startsWith("abc"), false);
    ASSERT_EQ(s11->substring(1)->equals("est"), true);
    ASSERT_EQ(s11->substring(1, 3)->equals("es"), true);
    ASSERT_EQ(s11->indexOf("es"), 1);
    ASSERT_EQ(s11->endsWith("st"), true);

    sp<String> s12 = new String("abc\n\n\ndef\r\nxyz");
    sp<ArrayList<sp<String>>> strings = s12->split("\n");
    ASSERT_EQ(strings->size(), 5);
    int32_t i = 0;
    auto itr = strings->iterator();
    while (itr.hasNext()) {
        sp<String> string = itr.next();
        switch (i) {
        case 0:
            ASSERT_STREQ(string->c_str(), "abc");
            break;
        case 1:
            ASSERT_STREQ(string->c_str(), "");
            break;
        case 2:
            ASSERT_STREQ(string->c_str(), "");
            break;
        case 3:
            ASSERT_STREQ(string->c_str(), "def\r");
            break;
        case 4:
            ASSERT_STREQ(string->c_str(), "xyz");
            break;
        default:
            ASSERT_TRUE(false);
            break;
        }
        i++;
        itr.remove();
    }
    ASSERT_EQ(strings->size(), 0);

    strings = s12->split("\r\n");
    ASSERT_EQ(strings->size(), 2);
    i = 0;
    itr = strings->iterator();
    while (itr.hasNext()) {
        sp<String> string = itr.next();
        switch (i) {
        case 0:
            ASSERT_STREQ(string->c_str(), "abc\n\n\ndef");
            break;
        case 1:
            ASSERT_STREQ(string->c_str(), "xyz");
            break;
        default:
            ASSERT_TRUE(false);
            break;
        }
        i++;
        itr.remove();
    }
    ASSERT_EQ(strings->size(), 0);

    sp<String> s13 = new String("Hello");
    sp<String> s14 = new String("Hello");
    ASSERT_EQ(s13->equals(s14), true);
    ASSERT_EQ(s13->equals("Hello"), true);
    sp<String> s15 = new String("World");
    ASSERT_EQ(s13->equals(s15), false);
    ASSERT_EQ(s13->equals(nullptr), false);
    sp<Object> o = new Object();
    ASSERT_EQ(s13->equals(o), false);
    sp<Integer> integer = new Integer(123);
    ASSERT_EQ(s13->equals(object_cast<Object>(integer)), false);
}
