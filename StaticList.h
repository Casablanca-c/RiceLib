#ifndef STATICLIST_H
#define STATICLIST_H

#include "SeqList.h"

namespace FLLib
{

template < typename T, int N >
class StaticList : public SeqList<T>
{
protected:
    T m_sapce[N];   // 顺序存储空间，N 为模板参数
public:
    StaticList()
    {
        this->m_array = m_sapce;
        this->m_length = 0;
    }

    int capacity() const
    {
        return N;
    }
};


}


#endif // STATICLIST_H
