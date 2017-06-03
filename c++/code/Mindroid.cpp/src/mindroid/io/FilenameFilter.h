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

#ifndef MINDROID_FILENAMEFILTER_H_
#define MINDROID_FILENAMEFILTER_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"

namespace mindroid {

/**
 * An interface for filtering {@link File} objects based on their names
 * or the directory they reside in.
 *
 * @see File
 * @see File#list(FilenameFilter)
 */
class FilenameFilter : public Object {
public:
    /**
     * Indicates if a specific filename matches this filter.
     *
     * @param dir
     *            the directory in which the {@code filename} was found.
     * @param filename
     *            the name of the file in {@code dir} to test.
     * @return  {@code true} if the filename matches the filter
     *            and can be included in the list, {@code false}
     *            otherwise.
     */
    virtual bool accept(const sp<File>& dir, const sp<String>& filename) = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_FILENAMEFILTER_H_ */
