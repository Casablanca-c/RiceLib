#ifndef LISTGRAPH_H
#define LISTGRAPH_H

#include "Graph.h"
#include "LinkList.h"
#include "Exception.h"
#include "DynamicArray.h"

namespace FLLib
{

template < typename V, typename E >
class ListGraph : public Graph<V, E>
{
protected:
    struct Vertex : public Object
    {
        V* data;
        LinkList< Edge<E> > edge;

        Vertex()
        {
            data = NULL;
        }
    };

    LinkList<Vertex*> m_list; // 邻接链表

public:
    ListGraph(unsigned int n = 0)
    {
        for(unsigned int i=0; i<n; i++)
        {
            addVertex();
        }
    }

    int addVertex()
    {
        int ret = -1;

        Vertex* v = new Vertex();

        if( v != NULL )
        {
            m_list.insert(v);

            ret = m_list.length() - 1;  // 返回值就是当前顶点加入到链表中后的编号
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new vertex object ...");
        }

        return ret;
    }

    int addVertex(const V& value)
    {
        int ret = addVertex();

        if( ret >= 0 )
        {
            setVertex(ret, value);  // 设置当前顶点数据元素的值
        }

        return ret;
    }

    bool setVertex(int i, const V& value)
    {
        int ret = ( (0 <= i) && (i < vCount()) );

        if( ret )
        {
            Vertex* vertex = m_list.get(i);
            V* data = vertex->data;

            if( data == NULL )
            {
                data = new V();
            }

            if( data != NULL )
            {
                *data = value;

                vertex->data = data;
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new vertex value ...");
            }
        }

        return ret;
    }

    // 将与位置 i 相关联的数据元素值返回
    V getVertex(int i)
    {
        V ret;

        if( !getVertex(i, ret) )
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    bool getVertex(int i, V& value)
    {
        int ret = ( (0 <= i) && (i < vCount()) );

        if( ret )
        {
            Vertex* v = m_list.get(i);

            if( v->data != NULL )
            {
                value = *(v->data);
            }
            else
            {
                THROW_EXCEPTION(InvalidOperationException, "No value assigned to this vertex ...");
            }
        }

        return ret;
    }

    // 删除最近添加的顶点
    void removeVertex()
    {
        if( m_list.length() > 0 )
        {
            int index = m_list.length() - 1;
            Vertex* v = m_list.get(index);

            if( m_list.remove(index) )
            {
                //int i = 0;

                // 检查其他顶点的邻接链表里面有没有与被删除顶点相关的边，有的话删除该边
                for(int i = (m_list.move(0), 0); !m_list.end(); i++, m_list.next())
                {
                    int pos = m_list.current()->edge.find(Edge<E>(i, index)); // 查找以i为起点， index 为终点的边是否在当前的邻接链表里面

                    if( pos >= 0 )
                    {
                        m_list.current()->edge.remove(pos);
                    }
                    //i++;

                }
                delete v->data; // 释放与顶点相关联的数据元素值的空间
                delete v;       // 释放顶点自身所占用的空间
            }

        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No vertex in current graph ...");
        }
    }

    // 获取从顶点 i 出发可以抵达的顶点编号(终点编号)
    SharedPointer< Array<int> > getAdjacent(int i)
    {
        DynamicArray<int>* ret = NULL;

        if( (0 <= i) && (i < vCount()) )
        {
            Vertex* vertex = m_list.get(i);

            ret = new DynamicArray<int>(vertex->edge.length());

            if( ret != NULL )
            {
                for(int k = (vertex->edge.move(0), 0); !vertex->edge.end(); k++, vertex->edge.next())
                {
                    ret->set(k, vertex->edge.current().e);
                }
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create ret object ...");
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    bool isAdjacent(int i, int j)
    {
        return (0 <= i) && (i < vCount()) && (0 <= j) && (j < vCount()) && (m_list.get(i)->edge.find(Edge<E>(i, j)) >= 0);
    }

    E getEdge(int i, int j)
    {
        E ret;

        if( !getEdge(i, j, ret) )
        {
            THROW_EXCEPTION(InvalidParameterException, "Edge <i, j> is invalid ...");
        }

        return ret;
    }

    bool getEdge(int i, int j, E& value)
    {
        int ret = ( (0 <= i) && (i < vCount()) &&
                     (0 <= j) && (j < vCount()) );

        if( ret )
        {
            // 取出边的权值
            Vertex* vertex = m_list.get(i);
            int pos = vertex->edge.find(Edge<E>(i, j));

            if( pos >= 0 )
            {
                value = vertex->edge.get(pos).data;
            }
            else
            {
                THROW_EXCEPTION(InvalidOperationException, "No value assigned to this edge ...");
            }
        }

        return ret;
    }

    bool setEdge(int i, int j, const E& value)
    {
        int ret = ( (0 <= i) && (i < vCount()) &&
                    (0 <= j) && (j < vCount()) );

        if( ret )
        {
            Vertex* vertex = m_list.get(i);
            int pos = vertex->edge.find(Edge<E>(i, j));

            if( pos >= 0 )
            {
                ret = vertex->edge.set(pos, Edge<E>(i, j, value));
            }
            else
            {
                // 不存在 <i, j> 这条边，则增加一条边
                ret = vertex->edge.insert(0, Edge<E>(i, j, value));
            }
        }

        return ret;
    }

    bool removeEdge(int i, int j)
    {
        int ret = ( (0 <= i) && (i < vCount()) &&
                    (0 <= j) && (j < vCount()) );

        if( ret )
        {
            Vertex* vertex = m_list.get(i);
            int pos = vertex->edge.find(Edge<E>(i, j));

            if( pos >= 0 )
            {
                ret = vertex->edge.remove(pos);
            }
        }

        return ret;
    }

    int vCount()
    {
        return m_list.length();
    }

    int eCount()
    {
        int ret = 0;

        for(m_list.move(0); !m_list.end(); m_list.next())
        {
            ret += m_list.current()->edge.length();
        }

        return ret;
    }

    // 获取顶点 i 的入度
    int ID(int i)
    {
        int ret = 0;

        if( (0 <= i) && (i < vCount()) )
        {
            for(m_list.move(0); !m_list.end(); m_list.next())
            {
                LinkList< Edge<E> >& edge = m_list.current()->edge;

                // 遍历当前顶点的邻接链表
                for(edge.move(0); !edge.end(); edge.next())
                {
                    if( edge.current().e == i )
                    {
                        ret++;
                        break;
                    }
                }
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    // 获取顶点 i 的出度
    int OD(int i)
    {
        int ret = 0;

        if( (0 <= i) && (i < vCount()) )
        {
            ret = m_list.get(i)->edge.length();
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    ~ListGraph()
    {
        while( m_list.length() > 0 )
        {
            Vertex* toDel = m_list.get(0);

            m_list.remove(0);

            delete toDel->data;
            delete toDel;
        }
    }

};

}

#endif // LISTGRAPH_H
