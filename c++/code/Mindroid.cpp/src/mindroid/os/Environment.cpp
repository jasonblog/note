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

#include "mindroid/os/Environment.h"
#include "mindroid/app/SharedPreferencesImpl.h"

namespace mindroid {

pthread_mutex_t Environment::sMutex = PTHREAD_MUTEX_INITIALIZER;
Environment* Environment::sInstance = nullptr;

Environment::Environment() :
        ROOT_DIRECTORY(new File(".")),
        APPS_DIRECTORY(new File(ROOT_DIRECTORY, "apps")),
        DATA_DIRECTORY(new File(ROOT_DIRECTORY, "data")),
        PREFERENCES_DIRECTORY(new File(ROOT_DIRECTORY, "prefs")),
        LOG_DIRECTORY(new File(ROOT_DIRECTORY, "logs")) {
}

void Environment::setRootDirectory(const sp<String>& rootDirectory) {
    Environment* self = getInstance();
    self->ROOT_DIRECTORY = new File(rootDirectory);
    self->APPS_DIRECTORY = new File(self->ROOT_DIRECTORY, "apps");
    self->DATA_DIRECTORY = new File(self->ROOT_DIRECTORY, "data");
    self->PREFERENCES_DIRECTORY = new File(self->ROOT_DIRECTORY, "prefs");
    self->LOG_DIRECTORY = new File(self->ROOT_DIRECTORY, "logs");
}

sp<SharedPreferences> Environment::getSharedPreferences(const sp<File>& sharedPrefsFile, int32_t mode) {
    Environment* self = getInstance();
    sp<SharedPreferences> sp;
    AutoLock autoLock(self->mLock);
    sp = self->mSharedPrefs->get(sharedPrefsFile->getAbsolutePath());
    if (sp == nullptr) {
        sp = new SharedPreferencesImpl(sharedPrefsFile, mode);
        self->mSharedPrefs->put(sharedPrefsFile->getAbsolutePath(), sp);
        return sp;
    }
    return sp;
}

void Environment::clearSharedPreferences() {
    Environment* self = getInstance();
    AutoLock autoLock(self->mLock);
    self->mSharedPrefs->clear();
}

Environment* Environment::getInstance() {
    pthread_mutex_lock(&sMutex);
    if (sInstance == nullptr) {
        sInstance = new Environment();
    }
    pthread_mutex_unlock(&sMutex);
    return sInstance;
}

} /* namespace mindroid */
