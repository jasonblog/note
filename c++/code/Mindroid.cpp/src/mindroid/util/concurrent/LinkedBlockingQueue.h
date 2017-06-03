/*
 * Copyright (C) 2011 Daniel Himmelein
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

#ifndef MINDROID_LINKEDBLOCKINGQUEUE_H_
#define MINDROID_LINKEDBLOCKINGQUEUE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"

namespace mindroid {

template<typename T>
class LinkedBlockingQueue :
        public Object {
public:
    LinkedBlockingQueue() :
            mHeadNode(nullptr),
            mLock(new ReentrantLock()),
            mCondition(mLock->newCondition()) {
    }

    ~LinkedBlockingQueue() {
        Node* node = mHeadNode;
        while (node != nullptr) {
            Node* nextNode = node->nextNode;
            delete node;
            node = nextNode;
        }
    }

    LinkedBlockingQueue(const LinkedBlockingQueue&) = delete;
    LinkedBlockingQueue& operator=(const LinkedBlockingQueue&) = delete;

    bool put(T item) {
        AutoLock autoLock(mLock);
        Node* node = new Node(item);
        Node* curNode = mHeadNode;
        if (curNode == nullptr) {
            node->nextNode = curNode;
            mHeadNode = node;
            mCondition->signal();
        } else {
            Node* prevNode = nullptr;
            while (curNode != nullptr) {
                prevNode = curNode;
                curNode = curNode->nextNode;
            }
            node->nextNode = prevNode->nextNode;
            prevNode->nextNode = node;
            mCondition->signal();
        }
        return true;
    }

    T take() {
        while (true) {
            AutoLock autoLock(mLock);
            Node* node = getNextNode();
            if (node != nullptr) {
                T item = node->item;
                delete node;
                return item;
            } else {
                mCondition->await();
            }
        }
    }

    bool remove(T item) {
        bool foundItem = false;
        AutoLock autoLock(mLock);
        Node* curNode = mHeadNode;
        // Remove all matching messages at the front of the message queue.
        while (curNode != nullptr && curNode->item == item) {
            foundItem = true;
            Node* nextNode = curNode->nextNode;
            mHeadNode = nextNode;
            delete curNode;
            curNode = nextNode;
        }

        // Remove all matching messages after the front of the message queue.
        while (curNode != nullptr) {
            Node* nextNode = curNode->nextNode;
            if (nextNode != nullptr) {
                if (nextNode->item == item) {
                    foundItem = true;
                    Node* nextButOneNode = nextNode->nextNode;
                    delete nextNode;
                    curNode->nextNode = nextButOneNode;
                    continue;
                }
            }
            curNode = nextNode;
        }
        return foundItem;
    }

private:
    struct Node {
        T item;

        Node* nextNode;

        Node(T t) : item(t), nextNode(nullptr) { }
    };

    Node* getNextNode() {
        Node* node = mHeadNode;
        if (node != nullptr) {
            mHeadNode = node->nextNode;
            return node;
        }
        return nullptr;
    }

    Node* mHeadNode;
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
};

} /* namespace mindroid */

#endif /* MINDROID_LINKEDBLOCKINGQUEUE_H_ */
