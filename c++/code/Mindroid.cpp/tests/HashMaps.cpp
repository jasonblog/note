#include <gtest/gtest.h>
#include "mindroid/util/HashMap.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

TEST(Mindroid, HashMapOfIntegerInteger) {
    sp<HashMap<int, int>> map = new HashMap<int, int>();
    map->put(1, 1);
    map->put(2, 2);
    map->put(3, 3);
    map->put(4, 17);
    ASSERT_EQ(map->get(1), 1);
    ASSERT_EQ(map->get(2), 2);
    ASSERT_EQ(map->get(3), 3);
    ASSERT_EQ(map->get(4), 17);

    size_t i = 0;
    auto itr = map->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        switch (entry.getKey()) {
        case 1:
            ASSERT_EQ(entry.getValue(), 1);
            i |= 1;
            break;
        case 2:
            ASSERT_EQ(entry.getValue(), 2);
            i |= 2;
            break;
        case 3:
            ASSERT_EQ(entry.getValue(), 3);
            i |= 4;
            break;
        case 4:
            ASSERT_EQ(entry.getValue(), 17);
            i |= 8;
            break;
        }
        itr.remove();
    }
    ASSERT_EQ(map->size(), 0);
    ASSERT_EQ(i, 15);
}

TEST(Mindroid, HashMapOfStringString) {
    sp<HashMap<sp<String>, sp<String>>> map1 = new HashMap<sp<String>, sp<String>>();
    sp<String> abc = new String("abc");
    map1->put(abc, new String("123"));
    map1->put(sp<String>(new String("def")), new String("123456789"));
    ASSERT_STREQ(map1->get(sp<String>(new String("abc")))->c_str(), "123");
    ASSERT_STREQ(map1->get(abc)->c_str(), "123");

    ASSERT_STREQ(map1->remove(new String("abc"))->c_str(), "123");
    ASSERT_EQ(map1->get(sp<String>(new String("abc"))), nullptr);

    map1->put(abc, new String("123"));

    size_t i = 0;
    auto itr = map1->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        if (entry.getKey()->equals("abc")) {
            i |= 1;
            ASSERT_EQ(entry.getValue()->equals("123"), true);
        } else if (entry.getKey()->equals("def")) {
            i |= 2;
            ASSERT_EQ(entry.getValue()->equals("123456789"), true);
        }
    }
    ASSERT_EQ(map1->size(), 2);
    ASSERT_EQ(i, 3);

    sp<HashMap<sp<String>, sp<String>>> map2 = new HashMap<sp<String>, sp<String>>();
    map2->put(new String("Test1"), new String("7654321"));
    map2->put(new String("Test2"), new String("321"));
    map2->put(new String("Test2"), new String("0000"));
    map2->putAll(map1);
    i = 0;
    itr = map2->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        if (entry.getKey()->equals("abc")) {
            i |= 1;
            ASSERT_EQ(entry.getValue()->equals("123"), true);
        } else if (entry.getKey()->equals("def")) {
            i |= 2;
            ASSERT_EQ(entry.getValue()->equals("123456789"), true);
        } else if (entry.getKey()->equals("Test1")) {
            i |= 4;
            ASSERT_EQ(entry.getValue()->equals("7654321"), true);
        } else if (entry.getKey()->equals("Test2")) {
            i |= 8;
            ASSERT_EQ(entry.getValue()->equals("0000"), true);
        }
        itr.remove();
    }
    ASSERT_EQ(map2->size(), 0);
    ASSERT_EQ(i, 15);
}

TEST(Mindroid, HashMapOfIntegerString) {
    sp<HashMap<int, sp<String>>> map = new HashMap<int, sp<String>>();
    map->put(1, new String("123"));
    map->put(2, new String("456"));
    map->put(17, new String("789"));
    map->put(17, new String("Test"));
    map->put(42, new String("42"));
    int i = 0;
    auto itr = map->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        switch (entry.getKey()) {
        case 1:
            i |= 1;
            ASSERT_EQ(entry.getValue()->equals("123"), true);
            break;
        case 2:
            i |= 2;
            ASSERT_EQ(entry.getValue()->equals("456"), true);
            break;
        case 17:
            i |= 4;
            ASSERT_EQ(entry.getValue()->equals("Test"), true);
            break;
        case 42:
            i |= 8;
            ASSERT_EQ(entry.getValue()->equals("42"), true);
            break;
        }
    }
    ASSERT_EQ(map->size(), 4);
    ASSERT_EQ(i, 15);

    ASSERT_EQ(map->containsKey(42), true);
    ASSERT_EQ(map->containsKey(100), false);
    ASSERT_EQ(map->containsValue(new String("42")), true);
    ASSERT_EQ(map->containsValue(new String("789")), false);
    ASSERT_STREQ(map->get(17)->c_str(), "Test");
    ASSERT_EQ(map->isEmpty(), false);
    ASSERT_STREQ(map->put(17, new String("abc"))->c_str(), "Test");
    ASSERT_STREQ(map->get(17)->c_str(), "abc");
    ASSERT_STREQ(map->remove(17)->c_str(), "abc");

    itr = map->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        itr.remove();
    }
    ASSERT_EQ(map->size(), 0);
}

TEST(Mindroid, HashMapOfStringInteger) {
    sp<HashMap<sp<String>, int>> map = new HashMap<sp<String>, int>();
    map->put(new String("123"), 1);
    map->put(new String("456"), 2);
    map->put(new String("42"), 3);
    map->put(new String("Test"), 17);
    map->put(new String("42"), 42);
    int i = 0;
    auto itr = map->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        if (entry.getKey()->equals("123")) {
            i |= 1;
            ASSERT_EQ(entry.getValue(), 1);
        } else if (entry.getKey()->equals("456")) {
            i |= 2;
            ASSERT_EQ(entry.getValue(), 2);
        } else if (entry.getKey()->equals("42")) {
            i |= 4;
            ASSERT_EQ(entry.getValue(), 42);
        } else if (entry.getKey()->equals("Test")) {
            i |= 8;
            ASSERT_EQ(entry.getValue(), 17);
        }
    }
    ASSERT_EQ(map->size(), 4);
    ASSERT_EQ(i, 15);

    ASSERT_EQ(map->containsKey(new String("42")), true);
    ASSERT_EQ(map->containsKey(new String("100")), false);
    ASSERT_EQ(map->containsValue(42), true);
    ASSERT_EQ(map->containsValue(3), false);
    ASSERT_EQ(map->get(new String("42")), 42);
    ASSERT_EQ(map->isEmpty(), false);
    ASSERT_EQ(map->put(new String("123"), 123), 1);
    ASSERT_EQ(map->get(new String("123")), 123);
    ASSERT_EQ(map->remove(new String("123")), 123);

    itr = map->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        itr.remove();
    }
    ASSERT_EQ(map->size(), 0);
}
