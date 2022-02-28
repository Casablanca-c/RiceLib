#ifndef CIRCLELIST_H
#define CIRCLELIST_H

#include "LinkList.h"

namespace FLLib {

template < typename T >
class CircleList : public LinkList<T>
{
protected:
    typedef typename LinkList<T>::Node Node;

    int mod(int i) const   //O(1)
    {
        return (this->m_length == 0) ? 0 : (i % this->m_length);
    }
    // 获得指向尾节点的指针
    Node* last() const     // O(n)
    {
        return this->position(this->m_length-1)->next;
    }

    // 将链表首尾相连
    void last_to_first() const  // O(n)
    {
        last()->next = this->m_header.next;
    }

public:
    bool insert(const T& e)    // O(n)
    {
        return insert(this->m_length,e);
    }

    bool insert(int i, const T& e)         // O(1) + O(n) + O(n) ==> O(2n+1) ==> O(n)
    {
        bool ret = true;

        i = i % (this->m_length + 1);      // O(1)

        ret = LinkList<T>::insert(i, e);   // O(n)

        // 如果插入位置为0，首尾相连
        if( ret && (i == 0) )
        {
            last_to_first();               // O(n)
        }

        return ret;
    }

    bool remove(int i)  // (n)
    {
        bool ret = true;

        i = mod(i);

        // 特殊处理删除位置为0的情况
        if( i == 0 )   // O(n)
        {
            Node* toDel = this->m_header.next;

            if( toDel != NULL )
            {
                this->m_header.next = toDel->next;
                this->m_length--;

                if(this->m_length > 0)
                {
                    last_to_first();    // O(n)

                    if( this->m_current == toDel )
                    {
                        this->m_current = toDel->next;
                    }
                }
                else // 删除之后没有节点了
                {
                    this->m_header.next = NULL;
                    this->m_current = NULL;
                }

                this->destroy(toDel);
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = LinkList<T>::remove(i);  // O(n)
        }

        return ret;
    }

    bool set(int i, const T& e)   // O(n)
    {
        return LinkList<T>::set(mod(i), e);
    }

    T get(int i) const        // O(n)
    {
        return LinkList<T>::get(mod(i));
    }

    bool get(int i, T& e) const
    {
        return LinkList<T>::get(mod(i), e);
    }

    int find(const T& e) const   // O(n)
    {
        int ret = -1;

        Node* slider = this->m_header.next;

        for(int i = 0; i < this->m_length; i++)
        {
            if( slider->value == e )
            {
                ret = i;
                break;
            }
            slider = slider->next;
        }

        return ret;
    }

    void clear()  // O(n+1) ==> O(n)
    {
        while( this->m_length > 1 )   // O(n)
        {
            remove(1);     // O(1)
        }

        if( this->m_length == 1 )  // O(1)
        {
            Node* toDel = this->m_header.next;

            this->m_header.next = NULL;
            this->m_length = 0;
            this->m_current = NULL;

            this->destroy(toDel);
        }
    }

    bool move(int i, int step)  // O(n)
    {
        return LinkList<T>::move(mod(i), step);
    }

    bool end()  // O(1)
    {
        return (this->m_length == 0) || (this->m_current == NULL);
    }

    ~CircleList()   // O(n)
    {
        clear();
    }
};

}
#endif // CIRCLELIST_H
