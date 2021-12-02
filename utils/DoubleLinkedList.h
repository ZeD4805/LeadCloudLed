//
// Created by zed4805 on 08/11/21.
//

#ifndef LEAD_CLOUD_DOUBLELINKEDLIST_H
#define LEAD_CLOUD_DOUBLELINKEDLIST_H

template <typename T> class DoubleLinkedList {
    T *head, *tail;

public:

    DoubleLinkedList(){
        head = nullptr;
        tail = nullptr;
    }

    T* getHead(){
        return head;
    }
    T* getTail(){
        return tail;
    }

    T* popHead(){
        T* oldHead = head;
        if(head != nullptr)
            head = head->next;

        oldHead->next = nullptr;
        return oldHead;
    }
    T* popTail(){
        T* oldTail = tail;
        if(head != nullptr)
            tail = tail->prev;

        oldTail->prev = nullptr;
        return oldTail;
    }

    void insertHead(T* toInsert){
        toInsert->prev = nullptr;
        toInsert->next = head;
        head->prev = toInsert;

        head = toInsert;
    }

    void insertTail(T* toInsert){
        toInsert->prev = tail;
        toInsert->next = nullptr;
        tail->next = toInsert;

        tail = toInsert;
    }

    T* getThroughComparable(T filter){
        T* curr = head;

        while (curr != nullptr && filter.comparableTo(*curr)){
            curr = curr->next;
        }

        return curr;
    }

    T* removeThroughComparable (T filter){
        T* toRemove = getThroughComparable(T);
        if(toRemove != nullptr){
            if(toRemove->prev != nullptr)
                toRemove->prev->next = toRemove->next;
            if(toRemove->next != nullptr)
                toRemove->next->prev = toRemove->prev;
        }

        toRemove->prev = nullptr;
        toRemove->next = nullptr;

        return toRemove;
    }

    bool isEmpty(){
        return head == nullptr;
    }
};


#endif //LEAD_CLOUD_DOUBLELINKEDLIST_H
