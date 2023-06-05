#include <Arduino.h>
#pragma once

template <typename T>
class Atom
{
  public:
    T Data;

    Atom* Previous = nullptr;
    Atom* Next = nullptr;

    Atom(T& d) { Data = d; }
};

template <typename T>
class LList
{
  public:
    uint8_t Length;
    Atom<T>* First = nullptr;
    Atom<T>* Last = nullptr;

    typedef bool (*LListComparer)(const T& a, const T& b);

    LList()
    {
        Length = 0;
    }
    ~LList() {}

    bool GetFirst(T& data) {
        if (First == nullptr)
            return false;

        data = First->Data;

        return true;
    }
    T* GetFirst()
    {
        if (First == nullptr)
            return nullptr;

        return &(First->Data);
    }
    bool GetLast(T &data) {
        if (Last == nullptr)
            return false;

        data = Last->Data;

        return true;
    }
    T* GetLast()
    {
        if (Last == nullptr)
            return nullptr;

        return &(Last->Data);
    }

    bool Pop(T& data) {
        if (Last == nullptr)
            return false;

        data = Last->Data;

        if (Last->Previous == nullptr) { // TODO: Sanity check of ->First ??
            delete Last;

            First = nullptr;
            Last = nullptr;

            Length = 0;
        }
        else if (Last->Previous == First) {
            delete Last;

            Last = First;
            First->Previous = nullptr;
            First->Next = nullptr;

            Length = 1;
        }
        else {
            Last = Last->Previous;

            delete Last->Next;
            Last->Next = nullptr;

            Length--;
        }

        return true;
    }

    T& Push(T& item) {
        Atom<T>* atom = new Atom<T>(item);

        if (Last == nullptr) {
            First = atom;
            Last = atom;
        }
        else {
            Last->Next = atom;
            atom->Previous = Last;
            Last = atom; 
        }

        Length++;

        return item;
    }
    T& Add(T item) { return Push(item); }

    bool Remove(T& item, bool allmatches = false)
    {
        bool res = false;

        Atom<T>* ptr = First;
        Atom<T>* nxt = nullptr;

        while (ptr != nullptr) {
            nxt = ptr->Next;

            if (ptr->Data == item) {
                if (ptr->Previous != nullptr)
                    ptr->Previous->Next = nxt;
                else
                    First = nxt;
                
                if (nxt != nullptr)
                    nxt->Previous = ptr->Previous;
                else
                    Last = ptr->Previous;

                delete ptr;

                Length--;

                if (allmatches)
                    res = true;
                else
                    return true;
            }

            ptr = nxt;
        }

        return res;
    }
    bool RemoveIndex(uint16_t index, T& res)
    {
        Atom<T>* ptr = First;
        Atom<T>* nxt = nullptr;

        for (uint16_t i = 0; ptr != nullptr; ++i) {
            nxt = ptr->Next;

            if (i == index) {
                res = (ptr->Data);

                if (ptr->Previous != nullptr)
                    ptr->Previous->Next = nxt;
                else
                    First = nxt;

                if (nxt != nullptr)
                    nxt->Previous = ptr->Previous;
                else
                    Last = ptr->Previous;

                delete ptr;

                Length--;
                return true;
            }

            ptr = nxt;
        }

        return false;
    }
    int GetIndex(const T& lookFor)
    {
        Atom<T>* ptr = First;

        for (uint16_t i = 0; ptr != nullptr; ++i) {
            if (ptr->Data == lookFor) {
                return i;
            }

            ptr = ptr->Next;
        }

        return -1;
    }
    int GetIndex(const T& lookFor, LListComparer comp)
    {
        Atom<T>* ptr = First;

        for (uint16_t i = 0; ptr != nullptr; ++i) {
            if (comp(ptr->Data, lookFor)) {
                return i;
            }

            ptr = ptr->Next;
        }
    
        return -1;
    }

    T& GetByIndex(int index, T &emptyReturn)
    {
        if (index == -1) {
            if (Last == nullptr)
                return emptyReturn;
            return Last->Data;
        }
        else if ((index < -1) || (index >= Length))
            return emptyReturn;

        Atom<T>* ptr = First;

        for (int i = 0; ptr != nullptr; ++i) {
            if (i == index)
                return ptr->Data;

            ptr = ptr->Next;
        }

        return emptyReturn;
    }
    
    /* !! JUST FOR CONVINIENCE! Does NOT return a reference, returns an "empty" T-object on "OutOfRange" !! */
    T operator[] (int index) 
    {
        T t;
        return GetByIndex(index, t);
    }

    bool Find(const T& lookFor, T& foundObject)
    {
        Atom<T>* ptr = First;

        while (ptr != nullptr) {
            if (ptr->Data == lookFor) {
                foundObject = ptr->Data;
                return true;
            }

            ptr = ptr->Next;
        }

        return false;
    }
    bool Find(const T& lookFor, T& foundObject, LListComparer comp)
    {
        Atom<T>* ptr = First;

        while (ptr != nullptr) {
            if (comp(ptr->Data, lookFor)) {
                foundObject = ptr->Data;
                return true;
            }

            ptr = ptr->Next;
        }

        return false;
    }

    LList<T> FindAll(const T& lookFor, LListComparer comp)
    {
        LList<T> ret;

        Atom<T>* ptr = First;

        while (ptr != nullptr) {
            if (comp(ptr->Data, lookFor)) {
                ret.Push(ptr->Data);
            }

            ptr = ptr->Next;
        }

        return ret;
    }
};