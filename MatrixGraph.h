#ifndef MATRIXGRAPH_H
#define MATRIXGRAPH_H

#include "Graph.h"
#include "Exception.h"
#include "DynamicArray.h"

namespace FLLib
{

template < int N, typename V, typename E >
class MatrixGraph : public Graph<V, E>
{
protected:
    V* m_vertexes[N];
    E* m_edges[N][N];
    int m_eCount;

public:
    MatrixGraph()
    {
        for(int i=0; i<vCount(); i++)
        {
            m_vertexes[i] = NULL;
            for(int j=0; j<vCount(); j++)
            {
                m_edges[i][j] = NULL;
            }
        }

        m_eCount = 0;
    }

    // 获取与顶点 i 相关联的数据元素的值
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
        bool ret = ( (0 <= i) && i < vCount() );

        if( ret )
        {
            // 判断当前的顶点是否有数据元素与之相关联
            if( m_vertexes[i] != NULL )
            {
                value = *(m_vertexes[i]);
            }
            else
            {
                THROW_EXCEPTION(InvalidOperationException, "No value assigned to this vertex ...");
            }
        }

        return ret;
    }

    bool setVertex(int i, const V& value)
    {
        bool ret = ( (0 <= i) && (i < vCount()) );

        if( ret )
        {
            V* data = m_vertexes[i];  // 为了异常安全

            // 判断当前的顶点是否有数据元素与之相关联
            if( data == NULL )
            {
                data = new V();
            }

            if( data != NULL )
            {
                *data = value;

                m_vertexes[i] = data;
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to store new vertex value ...");
            }
        }

        return ret;
    }

    // 获取顶点 i 相邻接的其他顶点
    SharedPointer< Array<int> > getAdjacent(int i)
    {
        DynamicArray<int>* ret = NULL;

        if( (0 <= i) && (i < vCount()) )
        {
            int n = 0;  // 计数变量，与目标定点邻接的顶点个数

            for(int j=0; j<vCount(); j++)
            {
                if( m_edges[i][j] != NULL )
                {
                    n++;
                }
            }

            ret = new DynamicArray<int>(n);

            if( ret != NULL )
            {
                for(int j=0, k=0; j<vCount(); j++)
                {
                    if( m_edges[i][j] != NULL )
                    {
                        ret->set(k++, j); // 记录与目标定点邻接的顶点编号
                    }
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
        return (0 <= i) && (i < vCount()) && (0 <= j) && (j < vCount()) && (m_edges[i][j] != NULL);
    }

    // 获取顶点 i 到顶点j 之接边上的权值
    E getEdge(int i, int j)
    {
        E ret;

        if( !getEdge(i, j, ret))
        {
            THROW_EXCEPTION(InvalidParameterException, "Index <i, j> is invalid ...");
        }

        return ret;
    }

    bool getEdge(int i, int j, E& value)
    {
        bool ret = ( (0 <= i) && (i < vCount()) &&
                     (0 <= j) && (j < vCount()) );

        if( ret )
        {
            if( m_edges[i][j] != NULL ) // 判断权值存在否
            {
                value = *(m_edges[i][j]);
            }
            else
            {
                THROW_EXCEPTION(InvalidOperationException, "No value assigend to this edge ...");
            }
        }

        return ret;
    }

    bool setEdge(int i, int j, const E& value)
    {
        bool ret = ( (0 <= i) && (i < vCount()) &&
                     (0 <= j) && (j < vCount()) );

        if( ret )
        {
            E* ne = m_edges[i][j];

            if( ne == NULL )
            {
                ne = new E();

                if( ne != NULL )
                {
                    *ne = value;

                    m_edges[i][j] = ne;

                    m_eCount++;
                }
                else
                {
                    THROW_EXCEPTION(NoEnoughMemoryException, "No memory to store new edge value ...");
                }
            }
            else
            {
                *ne = value;
            }
        }

        return ret;
    }

    bool removeEdge(int i, int j)
    {
        bool ret = ( (0 <= i) && (i < vCount()) &&
                     (0 <= j) && (j < vCount()) );

        if( ret )
        {
            E* toDel = m_edges[i][j];

            m_edges[i][j] = NULL;

            if( toDel != NULL )
            {
                m_eCount--;

                delete toDel;
            }
        }

        return ret;
    }

    int vCount()
    {
        return N;
    }

    int eCount()
    {
        return m_eCount;
    }

    // 获取顶点 i 的出度
    int OD(int i)
    {
        int ret = 0;

        if( (0 <= i) && (i < vCount()) )
        {
            for(int j=0; j<vCount(); j++)
            {
                if( m_edges[i][j] != NULL )
                {
                    ret++;
                }
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    // 获取顶点 i 的入度
    int ID(int i)
    {
        int ret = 0;

        if( (0 <= i) && (i < vCount()) )
        {
            for(int j=0; j<vCount(); j++)
            {
                if( m_edges[j][i] != NULL )
                {
                    ret++;
                }
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    ~MatrixGraph()
    {
        for(int i=0; i<vCount(); i++)
        {
            for(int j=0; j<vCount(); j++)
            {
                delete m_edges[i][j];
            }
            delete m_vertexes[i];
        }
    }

};

}

#endif // MATRIXGRAPH_H
