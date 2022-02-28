#ifndef DUALLINKLIST_H
#define DUALLINKLIST_H

#include "List.h"
#include "Exception.h"


namespace FLLib {

// 构造头节点的时候，不去调用泛指类型的构造函数
template < typename T >
class DualLinkList : public List<T>
{
protected:
    struct Node : public Object
    {
        T value;
        Node* next;
        Node* pre;
    };

    // 匿名的类型=>没有类型名字的， 仅仅是为了头节点
    // 避开泛指类型 T 的构造函数的调用
    mutable struct : public Object
    {
        char reserved[sizeof(T)];  // 仅仅是为了占空间
        Node* next;
        Node* pre;
    } m_header;

    int m_length;
    int m_step;       // 游标每次移动节点的数目
    Node* m_current;  // 游标节点

    // 获得指向i号位置的指针
    Node* position(int i) const
    {
        Node* ret = reinterpret_cast<Node*>(&m_header);
        for(int p = 0; p < i; p++)
        {
            ret = ret->next;
        }

        return ret;
    }

    virtual Node* create()
    {
        return new Node();
    }

    virtual void destroy(Node* pn)
    {
        delete pn;
    }

public:
    DualLinkList()
    {
        m_header.next = NULL;
        m_header.pre = NULL;
        m_length = 0;
        m_step = 1;
        m_current = NULL;
    }

    bool insert(const T& e)  // 往线性表尾部插入元素
    {
        return insert(m_length, e);
    }

    bool insert(int i, const T& e)
    {
        bool ret = ( (0 <= i) && (i <= m_length) );

        if( ret )
        {
            Node* node =create();

            if( node != NULL )
            {
                Node* current = position(i);
                Node* next = current->next;   // 新插入节点的下一个节点

                node->value = e;

                node->next = next;
                current->next = node;

                if( current != reinterpret_cast<Node*>(&m_header) )
                {
                    node->pre = current;
                }
                else
                {
                    node->pre = NULL;
                }

                if( next != NULL )
                {
                    next->pre = node;
                }

                m_length++;
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to insert new element ...");
            }
        }

        return ret;
    }

    bool remove(int i )
    {
        bool ret = ( (0 <= i) && (i < m_length) );

        if( ret )
        {
            Node* current = position(i);
            Node* toDel = current->next;
            Node* next = toDel->next;

            // 如果 m_current 等于即将被删除的节点，将游标m_current指向下一个节点
            if(m_current == toDel)
            {
                m_current = next;
            }

            current->next = next;

            if(next != NULL)
            {
                next->pre = toDel->pre;
            }

            m_length--;

            destroy(toDel);

            // m_length--; /* 考虑到 destroy函数可能会抛出异常，这句不能放在 destroy 函数之后 */
        }

        return ret;
    }

    bool set(int i, const T& e)
    {
        bool ret = ( (0 <= i) && (i < m_length) );

        if ( ret )
        {
            position(i)->next->value = e;
        }

        return ret;
    }
    virtual T get(int i) const
    {
        T ret;

        if( get(i, ret) )
        {
            return ret;
        }
        else
        {
            THROW_EXCEPTION(IndexOutOfBoundsException, "Invalid parameter i to get element ...");
        }
    }
    bool get(int i, T& e) const
    {
        bool ret = ( (0 <= i) && (i < m_length) );

        if ( ret )
        {
            e = position(i)->next->value;
        }

        return ret;
    }

    int find(const T& e) const
    {
        int ret = -1;
        int i = 0;
        Node* node = m_header.next;

        while(node)
        {
            if(node->value == e)
            {
                ret = i;
                break;
            }
            else
            {
                node = node->next;
                i++;
            }
        }

        return ret;
    }

    int length() const
    {
        return m_length;
    }

    void clear()
    {
        while( m_length > 0)
        {
            remove(0);
        }

    }

    virtual bool move(int i, int step = 1)
    {
        bool ret = ( (0 <= i) && (i <m_length) && (step > 0) );

        if( ret )
        {
            m_current = position(i)->next; // 定位目标位置的节点
            m_step = step;
        }

        return ret;
    }

    virtual bool end()
    {
        return (m_current == NULL );
    }

    virtual T current()
    {
        if( !end() )
        {
            return m_current->value;
        }
        else
        {
            // 遍历结束
            THROW_EXCEPTION(InvalidOperationException, "No value at currnet position ...");
        }
    }

    virtual bool next()
    {
        int i = 0;
        while( (i < m_step) && !end() )
        {
            m_current = m_current->next;
            i++;
        }

        return ( i == m_step );
    }

    virtual bool pre()
    {
        int i = 0;
        while( (i < m_step) && !end() )
        {
            m_current = m_current->pre;
            i++;
        }

        return ( i == m_step );
    }

    ~DualLinkList()
    {
        clear();  // 析构函数里面调用虚函数，多态失效
    }
};

}


#endif // DUALLINKLIST_H
