#ifndef GTREE_H
#define GTREE_H

#include "Tree.h"
#include "GTreeNode.h"
#include "Exception.h"
#include "LinkQueue.h"

namespace FLLib
{

template < typename T >
class GTree : public Tree<T>
{
protected:
    LinkQueue<GTreeNode<T>*> m_queue;

    GTreeNode<T>* find(GTreeNode<T>* node, const T& value) const
    {
        GTreeNode<T>* ret = NULL;

        if( node != NULL )
        {
            if( node->value == value )
            {
                return node;
            }
            else
            {
                // 遍历当前节点的每一棵子树
                for(node->child.move(0); !node->child.end() && (ret == NULL); node->child.next())
                {
                    ret = find(node->child.current(), value);
                }
            }
        }

        return ret;
    }

    GTreeNode<T>* find(GTreeNode<T>* node, GTreeNode<T>*obj) const
    {
        GTreeNode<T>* ret = NULL;

        if( node == obj )
        {
            return node;
        }
        else
        {
            if( node != NULL )
            {
                for(node->child.move(0); !node->child.end() && (ret == NULL); node->child.next())
                {
                    ret = find(node->child.current(), obj);
                }
            }
        } 

        return ret;
    }

    void free(GTreeNode<T>* node)
    {
        if( node != NULL )
        {
            // 遍历每一个孩子节点，对它们执行清除操作
            for(node->child.move(0); !node->child.end(); node->child.next())
            {
                free(node->child.current());
            }

            /* 最后释放根节点
             * delete 只能作用堆空间的变量，对非堆空间变量进行 delete 操作会导致程序的不稳定！！
             * 问题：如何判断堆空间中的结点并释放？？
             */
            if( node->flag() )  
            {
                delete node;
            }

        }
    }
    /*
     *  将 node 为根结点的子树从原来的树中删除
     *  ret 作为子树返回（ret 指向堆空间中的树对象）
     */
    void remove(GTreeNode<T>* node, GTree<T>*& ret)
    {
        ret = new GTree<T>();

        if( ret == NULL )
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new tree ...");
        }
        else
        {
            if( root() == node )
            {
                this->m_root = NULL;
            }
            else
            {
                // 定义一个 node 的父结点中的孩子链表的别名
                LinkList<GTreeNode<T>*>& child = dynamic_cast<GTreeNode<T>*>(node->parent)->child;

                // 将当前要删除的结点从它的父结点给删除掉
                child.remove(child.find(node));

                node->parent = NULL;
            }

            ret->m_root = node;
        }
    }

    // 求以 node 为根结点的树中结点的数目
    int count(GTreeNode<T>* node) const
    {
        int ret = 0;

        if( node != NULL )
        {
            ret = 1;

            for(node->child.move(0); !node->child.end(); node->child.next())
            {
                ret += count(node->child.current()); // 统计当前孩子拥有的结点数目
            }
        }

        return ret;
    }

    // 获取 node 为根结点的树的高度
    int height(GTreeNode<T>* node) const
    {
        int ret = 0;

        if( node != NULL )
        {            
            for(node->child.move(0); !node->child.end(); node->child.next())
            {
                int h = height(node->child.current());

                if( ret < h )
                {
                    ret = h;
                }
            }

            ret = ret + 1;
        }

        return ret;
    }

    // 获取 node  为根结点的树的度数
    int degree(GTreeNode<T>* node) const
    {
        int ret = 0;

        if( node != NULL )
        {
            ret = node->child.length();

            for(node->child.move(0); !node->child.end(); node->child.next())
            {
                int d = degree(node->child.current());

                if( ret < d )
                {
                    ret = d;
                }
            }
        }

        return ret;
    }

public:

    GTree()
    {

    }

    bool insert(TreeNode<T>* node)
    {
        bool ret = true;

        if( node != NULL )
        {
            if( this->m_root == NULL )
            {
                node->parent = NULL;
                this->m_root = node;
            }
            else
            {
                GTreeNode<T>* np = find(node->parent);

                if( np != NULL )
                {
                    GTreeNode<T>* n = dynamic_cast<GTreeNode<T>*>(node);

                    if( np->child.find(n) < 0 ) // 判断当前要插入的节点是否已经存在当前的树中
                    {
                        np->child.insert(n); // 插入到父节点的子节点链表中去
                    }
                }
                else
                {
                    THROW_EXCEPTION(InvalidOperationException, "Invalid parent tree node ...");
                }
            }
        }
        else
        {
            THROW_EXCEPTION(InvalidParameterException, "Parameter node can not be NULL ...");
        }

        return ret;
    }

    bool insert(const T& value, TreeNode<T>* parent)
    {
        bool ret = true;
        GTreeNode<T>* node =  GTreeNode<T>::NewNode();  //new GTreeNode<T>();

        if( node != NULL )
        {
            node->value = value;
            node->parent = parent;

            insert(node);
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new tree node ...");
        }

        return ret;
    }


    SharedPointer< Tree<T> > remove(const T& value)
    {
        GTree<T>* ret = NULL;
        GTreeNode<T>* node = find(value);

        if( node == NULL )
        {
            THROW_EXCEPTION(InvalidParameterException, "Can not find the node via parameter value ...");
        }
        else
        {
            remove(node, ret);

            m_queue.clear();
        }

        return ret;
    }

    SharedPointer< Tree<T> > remove(TreeNode<T>* node)
    {
        GTree<T>* ret = NULL;
        node = find(node);

        if( node == NULL )
        {
            THROW_EXCEPTION(InvalidParameterException, "Parameter node is invalid ...");
        }
        else
        {
            remove(dynamic_cast<GTreeNode<T>*>(node), ret);

            m_queue.clear();
        }

        return ret;
    }

    GTreeNode<T>* find(const T& value) const // 赋值兼容性原则，返回值合法
    {
        return find(root(), value);
    }

    GTreeNode<T>* find(TreeNode<T>* node) const
    {
        return find(root(), dynamic_cast<GTreeNode<T>*>(node));
    }

    GTreeNode<T>* root() const
    {
        return dynamic_cast<GTreeNode<T>*>(this->m_root);
    }

    int degree() const
    {
        return degree(root());
    }

    int count() const
    {
        return count(root());
    }
    int height() const
    {
        return height(root());
    }

    void clear()
    {
        free(root()); //清楚根节点所代表的书

        this->m_root = NULL;

        m_queue.clear();
    }

    bool begin()
    {
        bool ret = (root() != NULL);

        if(ret)
        {
            // 将根结点压入队列里面
            m_queue.clear();
            m_queue.add(root());
        }

        return ret;
    }

    bool end()
    {
        return (m_queue.length() == 0);
    }

    // 将游标指向下一个结点（游标：队列中指向对头元素的指针）
    bool next()
    {
        /*
         * 将对列头元素弹出，然后将其孩子入队列
         */
        bool ret = (m_queue.length() > 0);

        if( ret )
        {
            GTreeNode<T>* node = m_queue.front();

            m_queue.remove();

            for(node->child.move(0); !node->child.end(); node->child.next())
            {
                m_queue.add(node->child.current());
            }
        }

        return ret;
    }

    // 返回当前游标说指向的结点中的数据元素
    T current()
    {
        if( !end() )
        {
            return m_queue.front()->value;
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No value at current position ...");
        }
    }

    ~GTree()
    {
        clear();
    }
};


}

#endif // GTREE_H
