/*
 * Copyright (C) 2007 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDROID_ENVIRONMENT_H_
#define MINDROID_ENVIRONMENT_H_

#include "mindroid/lang/String.h"
#include "mindroid/content/SharedPreferences.h"
#include "mindroid/io/File.h"
#include <pthread.h>

namespace mindroid {

/**
 * Provides access to environment variables.
 */
class Environment final {
public:
    /**
     * Sets the Mindroid root directory.
     *
     * @hide
     */
    static void setRootDirectory(const char* rootDirectory) {
        setRootDirectory(String::valueOf(rootDirectory));
    }
    static void setRootDirectory(const sp<String>& rootDirectory);

    /**
     * Gets the Mindroid root directory.
     */
    static sp<File> getRootDirectory() {
        return getInstance()->ROOT_DIRECTORY;
    }

    /**
     * Gets the Mindroid apps directory.
     */
    static sp<File> getAppsDirectory() {
        return getInstance()->APPS_DIRECTORY;
    }

    /**
     * Gets the Mindroid data directory.
     */
    static sp<File> getDataDirectory() {
        return getInstance()->DATA_DIRECTORY;
    }

    /**
     * Gets the Mindroid preferences directory.
     */
    static sp<File> getPreferencesDirectory() {
        return getInstance()->PREFERENCES_DIRECTORY;
    }

    /**
     * Gets the Mindroid log directory.
     */
    static sp<File> getLogDirectory() {
        return getInstance()->LOG_DIRECTORY;
    }

    /**
     * Sets the Mindroid log directory.
     */
    static void setLogDirectory(const sp<String>& directory) {
        getInstance()->LOG_DIRECTORY = new File(directory);
    }

    /**
     * Retrieve and hold the contents of the preferences file 'fileName', returning a SharedPreferences
     * through which you can retrieve and modify its values. Only one instance of the
     * SharedPreferences object is returned to any callers for the same name, meaning they will see
     * each other's edits as soon as they are made.
     *
     * @param baseDir The base directory of the preferences file.
     * @param fileName Desired preferences file name. If a preferences file by this name does not exist, it
     * will be created when you retrieve an editor (SharedPreferences.edit()) and then commit
     * changes (Editor.commit()).
     * @param mode Operating mode. Use 0 or {@link #MODE_PRIVATE} for the default operation.
     *
     * @return Returns the single SharedPreferences instance that can be used to retrieve and modify
     * the preference values.
     */
    static sp<SharedPreferences> getSharedPreferences(const sp<File>& baseDir, const sp<String>& fileName, int32_t mode) {
        sp<File> sharedPrefsFile = new File(baseDir, fileName);
        return getSharedPreferences(sharedPrefsFile, mode);
    }

    /**
     * Retrieve and hold the contents of the preferences file 'fileName', returning a SharedPreferences
     * through which you can retrieve and modify its values. Only one instance of the
     * SharedPreferences object is returned to any callers for the same name, meaning they will see
     * each other's edits as soon as they are made.
     *
     * @param sharedPrefsFile Desired preferences file. If a preferences file by this name does not exist, it
     * will be created when you retrieve an editor (SharedPreferences.edit()) and then commit
     * changes (Editor.commit()).
     * @param mode Operating mode. Use 0 or {@link #MODE_PRIVATE} for the default operation.
     *
     * @return Returns the single SharedPreferences instance that can be used to retrieve and modify
     * the preference values.
     */
    static sp<SharedPreferences> getSharedPreferences(const sp<File>& sharedPrefsFile, int32_t mode);

    /**
     * Clear shared preferences cache.
     */
    static void clearSharedPreferences();

private:
    Environment();

    static Environment* getInstance();

    sp<File> ROOT_DIRECTORY;
    sp<File> APPS_DIRECTORY;
    sp<File> DATA_DIRECTORY;
    sp<File> PREFERENCES_DIRECTORY;
    sp<File> LOG_DIRECTORY;
    sp<ReentrantLock> mLock = new ReentrantLock();
    sp<HashMap<sp<String>, sp<SharedPreferences>>> mSharedPrefs = new HashMap<sp<String>, sp<SharedPreferences>>();

    static pthread_mutex_t sMutex;
    static Environment* sInstance;
};

} /* namespace mindroid */

#endif /* MINDROID_ENVIRONMENT_H_ */
