#ifndef BTREENODE_H
#define BTREENODE_H

#include "TreeNode.h"

namespace FLLib
{

enum BTNodePos
{
    ANY,
    LEFT,
    RIGHT
};


template < typename T >
class BTreeNode : public TreeNode<T>
{
public:
    BTreeNode<T>* left;
    BTreeNode<T>* right;

    BTreeNode()
    {
        left = NULL;
        right = NULL;
    }

    // 工厂模式的本质就是一个共有的静态成员函数而已
    static BTreeNode<T>* NewNode()
    {
        BTreeNode<T>* ret = new BTreeNode<T>();

        if( ret != NULL )
        {
            ret->m_flag = true;
        }

        return ret;
    }

};

}
#endif // BTREENODE_H
