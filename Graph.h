#ifndef GRAPH_H
#define GRAPH_H

#include "Object.h"
#include "Array.h"
#include "SharedPointer.h"
#include "DynamicArray.h"
#include "LinkQueue.h"
#include "Linkstack.h"
#include "Sort.h"

namespace FLLib
{

template < typename E >
struct Edge : public Object
{
    int b;
    int e;
    E data;

    Edge(int i=-1, int j=-1)
    {
        b = i;
        e = j;
    }

    Edge(int i, int j, const E& value)
    {
        b = i;
        e = j;
        data = value;
    }

    // 如果两个顶点的起点和终点相同，即可认为两条边相等
    bool operator == (const Edge<E>& obj)
    {
        return (b == obj.b) && (e == obj.e);
    }

    bool operator != (const Edge<E>& obj)
    {
        return !(*this == obj);
    }

     bool operator < (const Edge<E>& obj)
     {
         return (data < obj.data);
     }

     bool operator > (const Edge<E>& obj)
     {
         return (data > obj.data);
     }
};


template < typename V, typename E >
class Graph : public Object
{
protected:
    template < typename T >
    DynamicArray<T>* toArray(LinkQueue<T>& queue)
    {
        DynamicArray<T>* ret = new DynamicArray<T>(queue.length());

        if( ret != NULL )
        {
            for(int i=0; i<ret->length(); i++, queue.remove())
            {
                ret->set(i, queue.front());
            }
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create ret object ...");
        }

        return ret;
    }


    // 获取无向图中的所有边
    SharedPointer< Array< Edge<E> > > getUndiretedEdges()
    {
        DynamicArray< Edge<E> >* ret = NULL;

        if( asUndirected() )
        {
            LinkQueue< Edge<E> > queue;

            for(int i=0; i<vCount(); i++)
            {
                for(int j=0; j<vCount(); j++)
                {
                    if( isAdjacent(i, j) )
                    {
                        queue.add(Edge<E>(i, j, getEdge(i, j)));
                    }
                }
            }

            ret = toArray(queue);
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "This function is for undirected graph only ...");
        }

        return ret;
    }

    int find(Array<int>& p, int v)
    {
        while( p[v] != -1 )
        {
            v = p[v];
        }

        return v;
    }

public:
    virtual V getVertex(int i) = 0;
    virtual bool getVertex(int i, V& value) = 0;
    virtual bool setVertex(int i, const V& value) = 0;
    virtual SharedPointer< Array<int> > getAdjacent(int i) = 0;
    virtual bool isAdjacent(int i, int j) = 0;  // 判断在当前图中的顶点 i 到顶点 j 是否邻接
    virtual E getEdge(int i, int j) = 0;
    virtual bool getEdge(int i, int j, E& value) = 0;
    virtual bool setEdge(int i, int j, const E& value)= 0;
    virtual bool removeEdge(int i, int j) = 0;
    virtual int vCount() = 0;
    virtual int eCount() = 0;
    virtual int OD(int i) = 0;
    virtual int ID(int i) = 0;
    virtual int TD(int i) // 表示某个顶点的度
    {
        return OD(i) + ID(i);
    }

    // 判断当前的有向图是否可以看成无向图
    bool asUndirected()
    {
        bool ret = true;

        for(int i=0; i<vCount(); i++)
        {
            for(int j=0; j<vCount(); j++)
            {
                if( isAdjacent(i, j) )
                {
                    ret = ret && isAdjacent(j, i) && (getEdge(i, j) == getEdge(j, i));
                }
            }
        }

        return ret;
    }

    /*
     *  prim 函数执行结束后将返回一个智能指针
     *  这个智能指针执行一个边的数组
     */
    SharedPointer< Array< Edge<E> > > prim(const E& LIMIT, const bool MINMUM = true)
    {
        LinkQueue< Edge<E> > ret;

        if( asUndirected() )
        {
            DynamicArray<int> adjVex(vCount());
            DynamicArray<bool> mark(vCount());
            DynamicArray<E> cost(vCount());
            SharedPointer< Array<int> > aj = NULL;  // 用于保持某个顶点的邻接顶点数组
            bool end = false;
            int v = 0;

            for(int i=0; i<vCount(); i++)
            {
                adjVex[i] = -1;
                mark[i] = false;
                cost[i] = LIMIT;
            }

            mark[v] = true;

            aj = getAdjacent(v);

            for(int j=0; j<aj->length(); j++)
            {
                cost[(*aj)[j]] = getEdge(v, (*aj)[j]);
                adjVex[(*aj)[j]] = v;
            }

            // 找最小值
            for(int i=0; (i < vCount()) && !end; i++)
            {
                E m = LIMIT;
                int k = -1;

                for(int j=0; j<vCount(); j++)
                {
                    if( !mark[j] && (MINMUM ? (cost[j] < m) : (cost[j] > m)) )
                    {
                        m = cost[j];
                        k = j;
                    }
                }

                end = (k == -1);

                if( !end )
                {
                    ret.add(Edge<E>(adjVex[k], k, getEdge(adjVex[k], k)));

                    mark[k] = true;

                    aj = getAdjacent(k);

                    for(int j=0; j<aj->length(); j++)
                    {
                        if( !mark[(*aj)[j]] && (MINMUM ? (getEdge(k, (*aj)[j]) < cost[(*aj)[j]]) : (getEdge(k, (*aj)[j]) > cost[(*aj)[j]])) )
                        {
                            cost[(*aj)[j]] = getEdge(k, (*aj)[j]);
                            adjVex[(*aj)[j]] = k;
                        }
                    }
                }
            }

        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "Prim operation is for undirected graph only ...");
        }

        // n 个顶点的图所产生的最小生产树必然有 n-1 条边
        if( ret.length() != (vCount() - 1) )
        {
            THROW_EXCEPTION(InvalidOperationException, "No enough edge for operation ...");
        }

        return toArray(ret);
    }

    SharedPointer< Array< Edge<E> > > kruskal(const bool MINMUM = true)
    {
        LinkQueue< Edge<E> > ret;
        SharedPointer< Array< Edge<E> > > edges = getUndiretedEdges();
        DynamicArray<int> p(vCount()); // 前驱标记数组

        for(int i=0; i<p.length(); i++)
        {
            p[i] = -1;
        }

        Sort::Shell(*edges, MINMUM);  // ?

        for(int i=0; (i<edges->length()) && (ret.length() < (vCount() - 1)); i++)
        {
            int b = find(p, (*edges)[i].b);
            int e = find(p, (*edges)[i].e);

            if( b != e )
            {
                p[e] = b;

                ret.add((*edges)[i]);
            }
        }

        if( ret.length() != (vCount() - 1) )
        {
            THROW_EXCEPTION(InvalidOperationException, "No enough edges for Kruskal operation ...");
        }

        return toArray(ret);
    }

    // 图的广度优先遍历
    SharedPointer< Array<int> > BFS(int i)
    {
        // 存储图中顶点的编号
        /*
         *  通过数组元素的顺序来反映顶点被访问的先后次序
         */
        DynamicArray<int>* ret = NULL;

        if( (0 <= i) && (i < vCount()) )
        {
            LinkQueue<int> q;
            LinkQueue<int> r;

            DynamicArray<bool> visited(vCount());

            for(int i=0; i<visited.length(); i++)
            {
                visited[i] = false;
            }

            q.add(i);

            while( q.length() > 0 )
            {
                int v = q.front();

                q.remove();

                if( !visited[v] )
                {
                    SharedPointer< Array<int> > aj = getAdjacent(v);

                    for(int j=0; j<aj->length(); j++)
                    {
                        q.add((*aj)[j]);
                    }

                    r.add(v);

                    visited[v] = true;
                }
            }

            ret = toArray(r);
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    // 图的深度优先遍历
    SharedPointer< Array<int> > DFS(int i)
    {
        DynamicArray<int>* ret = NULL;

        if( (0 <= i) && (i < vCount()) )
        {
            LinkStack<int> s;
            LinkQueue<int> r;
            DynamicArray<bool> visited(vCount());

            for(int j=0; j<visited.length(); j++)
            {
                visited[j] = false;
            }

            s.push(i);

            while( s.size() > 0 )
            {
                int v = s.top();

                s.pop();

                if( !visited[v] )
                {
                    SharedPointer< Array<int> > aj = getAdjacent(v);

                    for(int j=aj->length() - 1; j>= 0; j--)
                    {
                        s.push((*aj)[j]);
                    }

                    r.add(v);

                    visited[v] = true;
                }
            }

            ret = toArray(r);

        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Index i is invalid ...");
        }

        return ret;
    }

    // 返回值就是最短路径上的各个顶点
    SharedPointer< Array<int> > dijkstra(int i, int j, const E& LIMIT)
    {
        LinkQueue<int> ret;

        if( (0 <= i) && (i < vCount()) &&(0 <= j) && (j < vCount()) )
        {
            DynamicArray<E> dist(vCount());    // 存储当前的路径值
            DynamicArray<int> path(vCount());  // 存储当前顶点的前驱顶点
            DynamicArray<bool> mark(vCount()); // 标记对应的顶点是否进入S集合

            for(int k=0; k<vCount(); k++)
            {
                mark[k] = false;
                path[k] = -1;

                // 设置为对应边上的权值，即从起始顶点 i 到达顶点 k 的权值
                // 更新path数组， 如果起始顶点 i 到达顶点 k 有连接，那k之前的顶点就是i,所以path[k]=i
                dist[k] = isAdjacent(i, k) ? (path[k]=i, getEdge(i, k)) : LIMIT;
            }

            // 将起始顶点放入 S 集合中
            mark[i] = true;

            for(int k=0; k<vCount(); k++)
            {
                E m = LIMIT;
                int u = -1;

                // 遍历 dist 数组，找最小路径值
                for(int w=0; w<vCount(); w++)
                {
                    // 对于没有被标记的那些顶点所对应的路径值，找最小值
                    if( !mark[w] && (dist[w] < m) )
                    {
                        m = dist[w];
                        u = w;
                    }
                }

                if( u == -1 )
                {
                    break;
                }

                mark[u] = true;

                for(int w=0; w<vCount(); w++)
                {
                    if( !mark[w] && isAdjacent(u, w) && (dist[u] + getEdge(u, w) < dist[w]) )
                    {
                        dist[w] = dist[u] + getEdge(u, w);
                        path[w] = u;
                    }
                }
            }

            LinkStack<int> s;

            s.push(j);

            for(int k=path[j]; k !=-1; k=path[k])
            {
                s.push(k);
            }

            while( s.size() > 0 )
            {
                ret.add(s.top());

                s.pop();
            }

        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "Index <i, j> is invalid ...");
        }

        // 最短路径至少有两个顶点
        if( ret.length() < 2 )
        {
            THROW_EXCEPTION(ArithmeticException, "There is no path from i to j ...");
        }

        return toArray(ret);
    }

    // 求
    int floyd_first(int x, int y, const E& LIMIT)
    {
        int ret = -1;
        if( (0 <= x) && (x < vCount()) &&(0 <= y) && (y < vCount()) )
        {
            DynamicArray< DynamicArray<E> > dist(vCount());

            for(int k=0; k<vCount(); k++)
            {
                dist[k].resize(vCount());
            }

            for(int i=0; i<vCount();i++)
            {
                for(int j=0; j<vCount(); j++)
                {
                    dist[i][j] = isAdjacent(i, j) ? getEdge(i, j) : LIMIT;
                }
            }

            for(int k=0; k<vCount(); k++)
            {
                for(int i=0; i<vCount();i++)
                {
                    for(int j=0; j<vCount(); j++)
                    {
                        if( (dist[i][k] + dist[k][j]) < dist[i][j] )
                        {
                            dist[i][j] = (dist[i][k] + dist[k][j]);
                        }
                    }
                }
            }

            ret = dist[x][y];
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "Index <x, y> is invalid ...");
        }

        return ret;
    }

    // 返回值是一个数组，存储最短路径的顶点
    SharedPointer< Array<int> > floyd(int x, int y, const E& LIMIT)
    {
        LinkQueue<int> ret;

        if( (0 <= x) && (x < vCount()) &&(0 <= y) && (y < vCount()) )
        {
            DynamicArray< DynamicArray<E> > dist(vCount());
            DynamicArray< DynamicArray<int> > path(vCount()); // 路径矩阵：用于记录最短路径上的各个顶点
                                                              // p[i][j]表示 i 到 j 的路径上所经过的第一个顶点

            for(int k=0; k<vCount(); k++)
            {
                dist[k].resize(vCount());
                path[k].resize(vCount());
            }

            for(int i=0; i<vCount();i++)
            {
                for(int j=0; j<vCount(); j++)
                {
                    path[i][j] = -1;
                    dist[i][j] = isAdjacent(i, j) ? (path[i][j]=j, getEdge(i, j)) : LIMIT;
                }
            }

            for(int k=0; k<vCount(); k++)
            {
                for(int i=0; i<vCount();i++)
                {
                    for(int j=0; j<vCount(); j++)
                    {
                        if( (dist[i][k] + dist[k][j]) < dist[i][j] )
                        {
                            dist[i][j] = (dist[i][k] + dist[k][j]);
                            path[i][j] = path[i][k];  // 如果当前的i到j通过k中转一下能够得到更短的路径，那么这条更短路径上，i->j 所经过的第一个顶点就是 i->k 所经过的第一个顶点
                        }
                    }
                }
            }

            while( (x != -1) && (x != y) )
            {
                ret.add(x);
                x = path[x][y];
            }

            if( x != -1 )
            {
                ret.add(x);
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "Index <x, y> is invalid ...");
        }

        // 最短路径至少有两个顶点
        if( ret.length() < 2 )
        {
            THROW_EXCEPTION(ArithmeticException, "There is no path from x to y ...");
        }


        return toArray(ret);
    }
};

}

#endif // GRAPH_H
