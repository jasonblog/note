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

#ifndef MINDROID_PAIR_H_
#define MINDROID_PAIR_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

template<typename F, typename S>
class Pair :
        public Object {
public:
    Pair(F first, S second) :
        first(first),
        second(second) {
    }

    static sp<Pair> create(F first, S second) {
        return new Pair(first, second);
    }

    F first;
    S second;
};

} /* namespace mindroid */

#endif /* MINDROID_PAIR_H_ */
