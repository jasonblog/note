/*
 * Copyright (C) 2016 E.S.R.Labs
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

#include "mindroid/io/File.h"
#include "mindroid/io/FilenameFilter.h"
#include "mindroid/lang/System.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

namespace mindroid {

const sp<String> File::pathSeparator = new String(":");
const sp<String> File::separator = new String("/");

File::File(const sp<File>& dir, const char* name) {
    sp<String> pathName = dir->getPath();
    if (!pathName->endsWith(separator)) {
        pathName = pathName->append(separator);
    }
    mPath = pathName->append(name);
}

File::File(const sp<File>& dir, const sp<String>& name) {
    sp<String> pathName = dir->getPath();
    if (!pathName->endsWith(separator)) {
        pathName = pathName->append(separator);
    }
    mPath = pathName->append(name);
}

File::File(const char* path) : mPath(String::valueOf(path)) {
    if (!mPath->equals(separator) && mPath->endsWith(separator)) {
        mPath = mPath->substring(0, mPath->length() - 1);
    }
}

File::File(const sp<String>& path) : mPath(path) {
    if (!mPath->equals(separator) && mPath->endsWith(separator)) {
        mPath = mPath->substring(0, mPath->length() - 1);
    }
}

File::File(const char* dirPath, const char* name) {
    sp<String> pathName = String::valueOf(dirPath);
    if (!pathName->endsWith(separator)) {
        pathName = pathName->append(separator);
    }
    mPath = pathName->append(name);
}

File::File(const sp<String>& dirPath, const sp<String>& name) {
    sp<String> pathName = dirPath;
    if (!pathName->endsWith(separator)) {
        pathName = pathName->append(separator);
    }
    mPath = pathName->append(name);
}

bool File::canExecute() const {
    return (access(mPath->c_str(), X_OK) == 0);
}

bool File::canRead() const {
    return (access(mPath->c_str(), R_OK) == 0);
}

bool File::canWrite() const {
    return (access(mPath->c_str(), W_OK) == 0);
}

bool File::remove() {
    return (unlink(mPath->c_str()) == 0);
}

bool File::exists() const {
    struct stat status;
    return (stat(mPath->c_str(), &status) == 0);
}

sp<String> File::getAbsolutePath() {
    if (isAbsolute()) {
        return mPath;
    }
    sp<String> userDir = System::getProperty("user.dir");
    return mPath->isEmpty() ? userDir : join(userDir, mPath);
}

sp<String> File::getName() {
    if (mName == nullptr) {
        ssize_t i = mPath->lastIndexOf(separatorChar);
        if (i >= 0) {
            mName = mPath->substring(i + 1);
        } else {
            mName = mPath;
        }
    }
    return mName;
}

sp<String> File::getParent() {
    if (mParent == nullptr) {
        if (!mPath->equals(separator)) {
            ssize_t i = mPath->lastIndexOf(separatorChar);
            if (i >= 0) {
                mParent = mPath->substring(0, i);
            } else {
                mParent = nullptr;
            }
        } else {
            mParent = nullptr;
        }
    }
    return mParent;
}

sp<File> File::getParentFile() {
    sp<String> parent = getParent();
    return new File(parent);
}

sp<String> File::getPath() {
    return mPath;
}

bool File::isAbsolute() const {
    return mPath->length() > 0 && mPath->charAt(0) == separatorChar;
}

bool File::isDirectory() const {
    struct stat status;
    if (stat(mPath->c_str(), &status) == 0) {
        return S_ISDIR(status.st_mode);
    } else {
        return false;
    }
}

bool File::isFile() const {
    struct stat status;
    if (stat(mPath->c_str(), &status) == 0) {
        return S_ISREG(status.st_mode);
    } else {
        return false;
    }
}

size_t File::length() const {
    struct stat status;
    if (stat(mPath->c_str(), &status) == 0) {
        return status.st_size;
    } else {
        return 0;
    }
}

sp<ArrayList<sp<File>>> File::listFiles() {
    DIR* direcrory = opendir(mPath->c_str());
    if (direcrory != nullptr) {
        sp<ArrayList<sp<File>>> list = new ArrayList<sp<File>>();
        struct dirent* file;
        while ((file = readdir(direcrory)) != nullptr) {
            if (!strncmp(file->d_name, ".", strlen(".") + 1)) continue;
            if (!strncmp(file->d_name, "..", strlen("..") + 1)) continue;

            list->add(new File(this, file->d_name));
        }
        closedir(direcrory);
        return list;
    } else {
        return nullptr;
    }
}

sp<ArrayList<sp<File>>> File::listFiles(const sp<FilenameFilter>& filter) {
    DIR* direcrory = opendir(mPath->c_str());
    if (direcrory != nullptr) {
        sp<ArrayList<sp<File>>> list = new ArrayList<sp<File>>();
        struct dirent* file;
        while ((file = readdir(direcrory)) != nullptr) {
            if (!strncmp(file->d_name, ".", strlen(".") + 1)) continue;
            if (!strncmp(file->d_name, "..", strlen("..") + 1)) continue;

            if (filter->accept(this, String::valueOf(file->d_name))) {
                list->add(new File(this, file->d_name));
            }
        }
        closedir(direcrory);
        return list;
    } else {
        return nullptr;
    }
}

bool File::mkdir() {
    return ::mkdir(mPath->c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool File::createNewFile() {
    int fd = open(mPath->c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd != -1) {
        close(fd);
        return true;
    } else {
        return false;
    }
}

bool File::renameTo(const sp<File>& newPath) {
    return (::rename(mPath->c_str(), newPath->mPath->c_str()) == 0);
}

sp<String> File::join(sp<String>& prefix, sp<String>& suffix) {
    if (prefix != nullptr) {
        if (suffix != nullptr) {
            if (suffix->length() > 0) {
                if (prefix->endsWith(separator) && suffix->startsWith(separator)) {
                    suffix = suffix->substring(1);
                }
                if (!prefix->endsWith(separator) && !suffix->startsWith(separator)) {
                    prefix = String::format("%s%s", prefix->c_str(), separator->c_str());
                }
            }
            return String::format("%s%s", prefix->c_str(), suffix->c_str());
        } else {
            return prefix;
        }
    } else {
        return suffix;
    }
}

} /* namespace mindroid */
