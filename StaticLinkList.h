#ifndef STATICLINKLIST_H
#define STATICLINKLIST_H

#include "LinkList.h"

namespace FLLib {

template < typename T, int N>
class StaticLinkList : public LinkList<T>
{
protected:
    typedef typename LinkList<T>::Node Node;

    struct SNode : public Node
    {
      void* operator new(unsigned int size, void* loc) // 在指定位置分配内存
      {
          (void)size;
          printf("fanliag\n");
          return loc;
      }
    };
    unsigned char m_space[sizeof(SNode) * N];
    int m_used[N]; // 标记数组

    Node* create()
    {
        SNode* ret = NULL;

        for(int i = 0; i < N; i++)
        {
            if( !m_used[i] )
            {
                ret = reinterpret_cast<SNode*>(m_space) + i;
                ret = new(ret)SNode();
                m_used[i] = 1;
                break;
            }
        }
        return ret;
    }

    void destroy(Node *pn)
    {
        SNode* space = reinterpret_cast<SNode*>(m_space);
        SNode* psn = dynamic_cast<SNode*>(pn);
        for(int i = 0; i < N; i++)
        {
            if(pn == (space + i))
            {
                m_used[i] = 0;
                psn->~SNode();
                break; // 归还了空间即可跳出循环
            }
        }
    }

public:
    StaticLinkList()
    {
        for(int i = 0; i < N; i++)
        {
            m_used[i] = 0;
        }
    }

    int capacity()
    {
        return N;
    }
    ~StaticLinkList()
    {
        this->clear(); // 这里调用的是父类 LinkList 的clear函数，
                       // 但是 clear 里面调用的 destroy 函数却是子类 StaticLinkList 的
        // 构造函数和析构函数是不会发生多态的，在它们内部直接调用或者间接调用虚函数必然是当前类中实现的版本
    }

};

}

#endif // STATICLINKLIST_H
