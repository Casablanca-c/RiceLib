#include <iostream>
#include "MatrixGraph.h"
#include "ListGraph.h"

using namespace std;
using namespace FLLib;


// 查找图 g 中 v 顶点的局部最多顶点路径，查找到之后记录到辅助数组 count和 path 里面去
int search_max_path(Graph<int, int>& g, int v, Array<int>& count, Array<int>& path, Array<bool>& mark)
{
    int ret = 0;
    int k = -1;
    SharedPointer< Array<int> > aj = g.getAdjacent(v);

    for(int i=0; i<aj->length(); i++)
    {
        int num = 0; // 保存邻接顶点的局部最多顶点路径上的顶点数目

        if( !mark[(*aj)[i]] )
        {
            num = search_max_path(g, (*aj)[i], count, path, mark);
        }
        else
        {
            num = count[(*aj)[i]];
        }

        if( ret < num )
        {
            ret = num;
            k = (*aj)[i]; // 记录邻接顶点的顶点号
        }
    }

    ret++;

    count[v] = ret;
    mark[v] = true;
    path[v] = k;  // 局部路径所经过的第一个顶点就是 k

    return ret;
}

SharedPointer< Graph<int, int> > greate_graph(int* a, int len)
{
    if( (a != NULL) && (len > 0) )
    {
        ListGraph<int, int>*  ret = new ListGraph<int, int>(len);

        if( ret != NULL )
        {
            for(int i=0; i<len; i++)
            {
                ret->setVertex(i, a[i]);
            }

            for(int i=0; i<len; i++)
            {
                for(int j=i+1; j<len; j++)
                {
                    if( a[i] <= a[j] )
                    {
                        ret->setEdge(i, j, 1);
                    }
                }
            }

            return ret;
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No enough memory to create new graph ...");
        }

    }
    else
    {
        THROW_EXCEPTION(InvalidParameterException, "Parameter a or len invalid ...");
    }


    return NULL;

}

void init_array(Array<int>& count, Array<int>& path, Array<bool>& mark)
{
    for(int i=0; i<count.length(); i++)
    {
        count[i] = 0;
    }

    for(int i=0; i<path.length(); i++)
    {
        path[i] = -1;
    }

    for(int i=0; i<mark.length(); i++)
    {
        mark[i] = false;
    }
}

/*
 * count:记录局部最多顶点路劲的顶点数目
 * path：记录局部最多顶点路劲所经过的顶点
 */
void print_max_path(Graph<int, int>& g, Array<int>& count, Array<int>& path)
{
    int max = 0;

    for(int i=0; i<count.length(); i++)
    {
        if( max < count[i] )
        {
            max = count[i];
        }
    }

    cout << "Len: " << max << endl;

    for(int i=0; i<count.length(); i++)
    {
        if( max == count[i] )
        {
            cout << "Element: " << g.getVertex(i) << " ";

            for(int j=path[i]; j!=-1; j=path[j])
            {
                cout << g.getVertex(j) << " ";
            }

            cout << endl;
        }
    }
}


// 对图 g 中的每一个顶点求局部最多顶点路径
void search_max_path(Graph<int, int> &g, Array<int> &count, Array<int> &path, Array<bool> &mark)
{
    for(int i=0; i<g.vCount(); i++)
    {
        if( !mark[i] )
        {
            search_max_path(g, i, count, path, mark);
        }
    }
}

void solution(int* a, int len)
{
    DynamicArray<int> count(len);
    DynamicArray<int> path(len);
    DynamicArray<bool> mark(len);
    SharedPointer< Graph<int, int> > g;

    g = greate_graph(a, len);

    init_array(count, path, mark);

    search_max_path(*g, count, path, mark);

    print_max_path(*g, count, path);

}

int main()
{
    int a[] = {1, 3, 5, 4}; //{1, 2, 3, 4, 5};//{5, 3, 2, 1};//{10, 30, 40, 1, 3, 5}; //{1, 3, 4, 2, 5}; //{3, 18, 7, 14, 10, 12, 23, 41, 16, 24};

    solution(a, sizeof(a)/sizeof(*a));

    return 0;
}

