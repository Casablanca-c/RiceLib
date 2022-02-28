#ifndef POINTER_H
#define POINTER_H

#include "Object.h"

namespace FLLib {

template < typename T >
class Pointer : public Object
{
protected:
    T* m_pointer;

public:
    Pointer(T* p = NULL)
    {
        m_pointer = p;
    }

    T* operator-> ()
    {
        return m_pointer;
    }

    T& operator* ()
    {
        return *m_pointer;
    }

    const T* operator-> () const
    {
        return m_pointer;
    }

    const T& operator* () const
    {
        return *m_pointer;
    }


    bool isNull() const
    {
        return (m_pointer == NULL);
    }

    T* get() const
    {
        return m_pointer;
    }

    /*
     *  不需要实现析构函数，因为继承自顶层父类
     *  只要在此类中不实现具体的析构函数，机会默认使用父类的纯虚函数
     *  Pointer 这个类就是抽象类
     */
};

}

#endif // POINTER_H
