#include <gtest/gtest.h>
#include "mindroid/app/SharedPreferencesImpl.h"
#include "mindroid/io/File.h"
#include "mindroid/lang/String.h"
#include "mindroid/os/Environment.h"

using namespace mindroid;

TEST(Mindroid, SharedPreferences) {
    Environment::setRootDirectory(".");
    sp<SharedPreferences> sharedPreferences = new SharedPreferencesImpl(new File(Environment::getPreferencesDirectory(), "Test.xml"), 0);
    sp<SharedPreferences::Editor> editor = sharedPreferences->edit();
    editor->clear()->commit();

    ASSERT_EQ(sharedPreferences->getBoolean("Bool", false), false);
    ASSERT_STREQ(sharedPreferences->getString("String", "abc")->c_str(), "abc");
    ASSERT_EQ(sharedPreferences->getStringSet("Set", nullptr), nullptr);

    sp<Set<sp<String>>> set = new Set<sp<String>>();
    set->add(String::valueOf("1"));
    set->add(String::valueOf("2"));
    set->add(String::valueOf("3"));

    sharedPreferences->edit()
            ->putBoolean("Bool", true)
            ->putString("String", "xyz")
            ->putStringSet("Set", set)
            ->commit();

    ASSERT_EQ(sharedPreferences->getBoolean("Bool", false), true);
    ASSERT_STREQ(sharedPreferences->getString("String", "abc")->c_str(), "xyz");
    sp<Set<sp<String>>> s = sharedPreferences->getStringSet("Set", nullptr);
    ASSERT_EQ(set->contains(String::valueOf("1")), true);
    ASSERT_EQ(set->contains(String::valueOf("2")), true);
    ASSERT_EQ(set->contains(String::valueOf("3")), true);

    sharedPreferences->edit()
            ->remove("String")
            ->putStringSet("Set", nullptr)
            ->commit();

    ASSERT_EQ(sharedPreferences->getString("String", nullptr), nullptr);
    ASSERT_EQ(sharedPreferences->getString("Set", nullptr), nullptr);
}
