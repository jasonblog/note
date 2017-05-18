ListNode* mergeKLists(vector<ListNode*>& lists)
{
    // Begin and end of our range of elements:
    auto it_begin = begin(lists);
    auto it_end = end(lists);

    // Removes empty lists:
    it_end = remove_if(it_begin, it_end, isNull);

    if (it_begin == it_end) {
        return nullptr;    // All lists where empty.
    }

    // Head and tail of the merged list:
    ListNode* head = nullptr;
    ListNode* tail = nullptr;

    // Builds a min-heap over the list of lists:
    make_heap(it_begin, it_end, minHeapPred);

    // The first element in the heap is the head of our merged list:
    head = tail = *it_begin;

    while (distance(it_begin, it_end) > 1) {

        // Moves the heap's front list to its back:
        pop_heap(it_begin, it_end, minHeapPred);

        // And removes one node from it:
        --it_end;
        *it_end = (*it_end)->next;

        // If it is not empty it inserts it back into the heap:
        if (*it_end) {

            ++it_end;
            push_heap(it_begin, it_end, minHeapPred);
        }

        // After  the push we have our next node in front of the heap:
        tail->next = *it_begin;
        tail = tail->next;
    }

    return head;
}
