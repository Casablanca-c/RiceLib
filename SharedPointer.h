#ifndef SHAREDPOINTER_H
#define SHAREDPOINTER_H

#include <cstdlib>
#include "Pointer.h"
#include "Exception.h"

namespace FLLib {

template <typename T>
class SharedPointer : public Pointer<T>
{
protected:
    int* m_ref;

    void assign(const SharedPointer<T>& obj)
    {
        this->m_ref = obj.m_ref;
        this->m_pointer = obj.m_pointer;

        if( this->m_ref )
        {
            (*this->m_ref)++;
        }
    }
public:
    SharedPointer(T* p = NULL) : m_ref(NULL)
    {
        if( p )
        {
            this->m_ref = static_cast<int*>(std::malloc(sizeof(int)));

            if( this->m_ref )
            {
                *(this->m_ref) = 1;
                this->m_pointer = p;
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create SharedPointer object ...");
            }
        }
    }

    SharedPointer(const SharedPointer<T>& obj): Pointer<T>(NULL)
    {
        assign(obj);
    }

    SharedPointer<T>& operator = (const SharedPointer<T>& obj)
    {
        if( this != &obj )
        {
            // 在赋值之前将当前对象置空
            clear();

            assign(obj);

        }

        return *this;
    }

    // 将当前智能指针指置空
    void clear()
    {
        T* toDel = this->m_pointer;
        int* ref = this->m_ref;

        this->m_pointer = NULL;
        this->m_ref = NULL;

        // 堆空间对象计数变量减1
        if( ref )
        {
            (*ref)--;

            if( *ref == 0 )
            {
                free(ref);

                delete toDel;
            }
        }
    }

    ~SharedPointer()
    {
        clear();
    }


};

template < typename T >
bool operator == (const T* l, const SharedPointer<T>& r)
{
    return (l == r.get());
}

template < typename T >
bool operator == (const SharedPointer<T>& l, const T* r)
{
    return (l.get() == r.get());
}

template < typename T >
bool operator == (const SharedPointer<T>& l, const SharedPointer<T>& r)
{
    return (l.get() == r.get());
}

template < typename T >
bool operator != (const SharedPointer<T>& l, const SharedPointer<T>& r)
{
    return !(l == r);
}


}

#endif // SHAREDPOINTER_H
