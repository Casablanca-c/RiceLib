#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H


#include "Pointer.h"



/**
 * 智能指针
 * 1. 指针生命周期结束是自动释放堆空间
 * 2. 一篇堆空间只能由一个指针标识
 * 3. 杜绝指针运算和指针比较
 */

namespace FLLib
{
template
< typename T >
class SmartPointer : public Pointer<T>
{
public:
    SmartPointer(T* p = NULL) : Pointer<T>(p)
    {
        // 直接调用父类的构造函数
    }

    SmartPointer(const SmartPointer<T>& obj)
    {
        this->m_pointer = obj.m_pointer;

        const_cast<SmartPointer<T>&>(obj).m_pointer = NULL;
    }

    SmartPointer<T>& operator = (const SmartPointer<T>& obj)
    {
        if( this != &obj )
        {
            /* 如果删除 m_pointer 所指向的堆空间
             * 就有可能造成异常的抛出。所以不能先删除 m_pointer 所指向的堆空间
             * 否则的话这里的赋值操作符函数就不是异常安全的
             */
            T* p = this->m_pointer;

            this->m_pointer = obj.m_pointer;

            const_cast<SmartPointer<T>&>(obj).m_pointer = NULL;

            delete p;
        }

        return *this;
    }


    ~SmartPointer()
    {
        delete this->m_pointer;
    }

};


}

#endif // SMARTPOINTER_H
