#ifndef BTREE_H
#define BTREE_H

#include "Tree.h"
#include "BTreeNode.h"
#include "Exception.h"
#include "LinkQueue.h"
#include "DynamicArray.h"

namespace FLLib
{

enum BTTraversal
{
    PreOrder,
    InOrder,
    PostOrder,
    LevelOrder
};

template < typename T >
class BTree : public Tree<T>
{
protected:
    LinkQueue<BTreeNode<T>*> m_queue;

    // 基于数据元素值的查找操作
    virtual BTreeNode<T>* find(BTreeNode<T>* node, const T& value) const
    {
        BTreeNode<T>* ret = NULL;

        if( node != NULL )
        {
            if( node->value == value )
            {
                ret = node;
            }
            else
            {
                // 先递归查找左子树
                if( ret == NULL )
                {
                    ret = find(node->left, value);
                }

                // 左子树找不到再递归查找右子树
                if( ret == NULL )
                {
                    ret = find(node->right, value);
                }
            }
        }

        return ret;
    }

    virtual BTreeNode<T>* find(BTreeNode<T>* node, BTreeNode<T>* obj) const
    {
        BTreeNode<T>* ret = NULL;

        if( node == obj )
        {
            ret = node;
        }
        else
        {
            if( node != NULL )
            {
                // 先递归查找左子树
                if( ret == NULL )
                {
                    ret = find(node->left, obj);
                }

                // 左子树找不到再递归查找右子树
                if( ret == NULL )
                {
                    ret = find(node->right, obj);
                }
            }
        }

        return ret;
    }

    virtual bool insert(BTreeNode<T>* n, BTreeNode<T>* np, BTNodePos pos)
    {
        bool ret = true;

        if( pos == ANY )
        {
             if( np->left == NULL )
             {
                 np->left = n;
             }
             else if( np->right == NULL )
             {
                 np->right = n;
             }
             else
             {
                 ret = false;
             }
        }
        else if( pos == LEFT )
        {
            if( np->left == NULL )
            {
                np->left = n;
            }
            else
            {
                ret = false;
            }
        }
        else if( pos == RIGHT)
        {
            if( np->right == NULL )
            {
                np->right = n;
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    virtual void remove(BTreeNode<T>* node, BTree<T>*& ret)
    {
        ret = new BTree<T>();

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
                BTreeNode<T>* parent = dynamic_cast<BTreeNode<T>*>(node->parent);

                if( parent->left == node )
                {
                    parent->left = NULL;
                }
                else if( parent->right == node )
                {
                    parent->right = NULL;
                }

                node->parent = NULL;
            }

            ret->m_root = node;
        }

    }

    virtual void free(BTreeNode<T>* node)
    {
        if( node != NULL )
        {
            free(node->left);
            free(node->right);

            if( node->flag() )
            {
                delete node;
            }
        }
    }

    int count(BTreeNode<T>* node) const
    {
        /*
        int ret = 0;

        if( node != NULL )
        {
            ret = count(node->left) + count(node->right) + 1;
        }

        return ret;
        */

        return (node != NULL) ? (count(node->left) + count(node->right) + 1) : 0;
    }

    int height(BTreeNode<T>* node) const
    {
        int ret = 0;

        if( node != NULL )
        {
            int hl = height(node->left);
            int hr = height(node->right);

            ret = ((hl < hr) ? hr : hl) + 1;
        }

        return ret;
    }

    int degree(BTreeNode<T>* node) const
    {
        int ret = 0;

        if( node != NULL )
        {
            /*
            int dl = degree(node->left);
            int dr = degree(node->right);

            ret = (!!node->left + !!node->right);  // 根结点的度数

            if( ret < dl )
            {
                ret = dl;
            }

            if( ret < dr )
            {
                ret = dr;
            }
            */

            // 高效的写法：但凡发现一个结点的度数为2，即可确认度数为2
            /*
            ret = (!!node->left + !!node->right);

            if( ret < 2 )
            {
                int dl = degree(node->left);

                if( ret < dl )
                {
                    ret = dl;
                }
            }

            if( ret < 2 )
            {
                int dr = degree(node->right);

                if( ret < dr )
                {
                    ret = dr;
                }
            }
            */

            // 改进的高效写法
            BTreeNode<T>* child[] =  { node->left, node->right };

            ret = (!!node->left + !!node->right);

            for(int i=0; (i<2) && (ret<2); i++)
            {
                int d =  degree(child[i]);

                if( ret < d )
                {
                    ret = d;
                }
            }
        }

        return ret;
    }

    void preOrderTraversal(BTreeNode<T>* node, LinkQueue<BTreeNode<T>*>& queue)
    {
        if( node != NULL )
        {
            queue.add(node);
            preOrderTraversal(node->left, queue);
            preOrderTraversal(node->right, queue);
        }
    }

    void inOrderTraversal(BTreeNode<T>* node, LinkQueue<BTreeNode<T>*>& queue)
    {
        if( node != NULL )
        {
            inOrderTraversal(node->left, queue);
            queue.add(node);
            inOrderTraversal(node->right, queue);
        }
    }

    void postOrderTraversal(BTreeNode<T>* node, LinkQueue<BTreeNode<T>*>& queue)
    {
        if( node != NULL )
        {
            postOrderTraversal(node->left, queue);
            postOrderTraversal(node->right, queue);
            queue.add(node);
        }
    }

    void levelOrderTraversal(BTreeNode<T>* node, LinkQueue<BTreeNode<T>*>& queue)
    {
        if( node != NULL )
        {
            LinkQueue<BTreeNode<T>*> tmp;

            tmp.add(node);

            while( tmp.length() > 0 )
            {
                BTreeNode<T>* n = tmp.front();

                if( n->left != NULL )
                {
                    tmp.add(n->left);
                }

                if( n->right != NULL )
                {
                    tmp.add(n->right);
                }

                tmp.remove();
                queue.add(n);
            }
        }
    }

    BTreeNode<T>* clone(BTreeNode<T>* node) const
    {
        BTreeNode<T>* ret = NULL;

        if( node != NULL )
        {
            ret = BTreeNode<T>::NewNode();

            if( ret != NULL )
            {
                ret->value = node->value;

                ret->left = clone(node->left);
                ret->right = clone(node->right);

                if( ret->left != NULL )
                {
                    ret->left->parent = ret;
                }

                if( ret->right != NULL )
                {
                    ret->right->parent = ret;
                }
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new node ...");
            }
        }

        return ret;
    }

    bool equal(BTreeNode<T>* lh, BTreeNode<T>* rh) const
    {
        if( lh == rh )
        {
            return true;
        }
        else if( (lh != NULL) && (rh != NULL) )
        {
            return (lh->value == rh->value) && equal(lh->left, rh->left) && equal(lh->right, rh->right);
        }
        else
        {
            return false;
        }
    }

    BTreeNode<T>* add(BTreeNode<T>* lh, BTreeNode<T>* rh) const
    {
        BTreeNode<T>* ret = NULL;

        if( (lh == NULL) && (rh != NULL) )
        {
            ret = clone(rh);
        }
        else if( (lh != NULL) && (rh == NULL) )
        {
            ret = clone(lh);
        }
        else if( (lh != NULL) && (rh != NULL) )
        {
            ret = BTreeNode<T>::NewNode();

            if( ret != NULL )
            {
                ret->value = lh->value + rh->value;

                ret->left = add(lh->left, rh->left);
                ret->right = add(lh->right, rh->right);

                if( ret->left != NULL )
                {
                    ret->left->parent = ret;
                }

                if( ret->right != NULL )
                {
                    ret->right->parent = ret;
                }
            }
            else
            {
                THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new node ...");
            }
        }

        return ret;
    }

    void traversal(BTTraversal order, LinkQueue<BTreeNode<T>*>& queue)
    {
        switch( order )
        {
            case PreOrder:
                preOrderTraversal(root(), queue);
                break;
            case InOrder:
                inOrderTraversal(root(), queue);
                break;
            case PostOrder:
                postOrderTraversal(root(), queue);
                break;
            case LevelOrder:
                levelOrderTraversal(root(), queue);
                break;
            default:
                THROW_EXCEPTION(InvalidParameterException, "Parameter order is invalid ...");
                break;
        }
    }

    // 将队列 queue 中的二叉树结点连接为一个双向链表， 并返回双向链表的首结点地址
    BTreeNode<T>* connect(LinkQueue<BTreeNode<T>*>& queue)
    {
        BTreeNode<T>* ret = NULL;

        if( queue.length() > 0 )
        {
            ret = queue.front();

            BTreeNode<T>* slider =  queue.front();

            queue.remove();

            slider->left = NULL;

            while( queue.length() > 0 )
            {
                slider->right = queue.front();
                queue.front()->left = slider;

                slider = queue.front();
                queue.remove();
            }

            slider->right = NULL; // 双向链表的最后一个结点的 next 指针为空
        }

        return ret;
    }

public:
    bool insert(TreeNode<T>* node)
    {
        return insert(node, ANY);
    }

    virtual bool insert(TreeNode<T>* node, BTNodePos pos)
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
                BTreeNode<T>* np = find(node->parent);

                if( np != NULL )
                {                 
                    ret = insert(dynamic_cast<BTreeNode<T>*>(node), np, pos);
                }
                else
                {
                    THROW_EXCEPTION(InvalidParameterException, "Invalid parent tree node ...");
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
        return insert(value, parent, ANY);
    }

    virtual bool insert(const T& value, TreeNode<T>* parent, BTNodePos pos)
    {
        bool ret = true;
        BTreeNode<T>* node = BTreeNode<T>::NewNode();

        if( node == NULL )
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new node ...");
        }
        else
        {
            node->value =value;
            node->parent = parent;

            ret = insert(node, pos);

            if( !ret )
            {
                delete node;
            }
        }

        return ret;
    }

    SharedPointer< Tree<T> > remove(const T& value)
    {
        BTree<T>* ret = NULL;
        BTreeNode<T>* node = find(value);

        if( node == NULL )
        {
            THROW_EXCEPTION(InvalidParameterException, "Can not find the tree node via value ...");
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
        BTree<T>* ret = NULL;

        node = find(node);

        if( node == NULL )
        {
            THROW_EXCEPTION(InvalidParameterException, "Parameter node is invalid ...");
        }
        else
        {
            remove(dynamic_cast<BTreeNode<T>*>(node), ret);

            m_queue.clear();
        }

        return ret;
    }

    BTreeNode<T>* find(const T& value) const
    {
        return find(root(), value);
    }

    BTreeNode<T>* find(TreeNode<T>* node) const
    {
        return find(root(), dynamic_cast<BTreeNode<T>*>(node) );
    }

    BTreeNode<T>* root() const
    {
        return dynamic_cast<BTreeNode<T>*>(this->m_root);
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
        free(root());

        m_queue.clear();

        this->m_root = NULL;
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
            BTreeNode<T>* node = m_queue.front();

            m_queue.remove();

            if( node->left != NULL )
            {
                m_queue.add(node->left);
            }

            if( node->right != NULL )
            {
                m_queue.add(node->right);
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

    SharedPointer< Array<T> > traversal(BTTraversal order)
    {
        DynamicArray<T>* ret = NULL;
        LinkQueue<BTreeNode<T>*> queue;  // 保持遍历结点时的次序

        traversal(order, queue);

        // 将遍历的到的结果保存到动态数组中去
         ret = new DynamicArray<T>(queue.length());

         if( ret != NULL )
         {
            for(int i=0; i<ret->length(); i++, queue.remove())
            {
                ret->set(i, queue.front()->value);
            }
         }
         else
         {
             THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create return array ...");
         }

        return ret;
    }

    BTreeNode<T>* thread(BTTraversal order)
    {
        BTreeNode<T>* ret = NULL;
        LinkQueue<BTreeNode<T>*> queue;

        traversal(order, queue);

        ret = connect(queue);

        this->m_root = NULL;

        m_queue.clear();

        return ret;
    }

    SharedPointer< BTree<T> > clone() const
    {
        BTree<T>* ret = new BTree<T>();

        if( ret != NULL )
        {
            ret->m_root = clone(root());
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new tree ...");
        }

        return ret;
    }

    bool operator == (const BTree<T>& btree)
    {
        return equal(root(), btree.root());
    }

    bool operator != (const BTree<T>& btree)
    {
        return !(*this == btree);
    }

    SharedPointer< BTree<T> > add(const BTree<T>& btree) const
    {
        BTree<T>* ret = new BTree<T>();

        if( ret != NULL )
        {
            ret->m_root = add(root(), btree.root());
        }
        else
        {
            THROW_EXCEPTION(NoEnoughMemoryException, "No memory to create new tree ...");
        }

        return ret;
    }

    ~BTree()
    {
        clear();
    }

};

}

#endif // BTREE_H
