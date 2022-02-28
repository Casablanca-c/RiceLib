#include <iostream>
#include "linkstack.h"
#include "LinkQueue.h"


using namespace std;
using namespace FLLib;

template < typename T >
class StackToQueue : public Queue<T>
{
protected:
    mutable LinkStack<T> m_stack_in;
    mutable LinkStack<T> m_stack_out;

    void move() const
    {
        if( m_stack_out.size() == 0 )
        {
            while( m_stack_in.size() > 0 )
            {
                m_stack_out.push(m_stack_in.top());
                m_stack_in.pop();
            }
        }
    }
public:
    void add(const T& e)
    {
        m_stack_in.push(e);
    }
    void remove()
    {
        move();

        if(m_stack_out.size() > 0)
        {
            m_stack_out.pop();
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No element in current queue ...");
        }
    }

    T front() const
    {
        move();

        if( m_stack_out.size() > 0 )
        {
            return m_stack_out.top();
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No element in current queue ...");
        }
    }
    void clear()
    {
        m_stack_in.clear();
        m_stack_out.clear();
    }



    int length() const
    {
        return m_stack_in.size() + m_stack_out.size();
    }
};

template < typename T >
class QueueToStack : public Stack<T>
{
protected:
    LinkQueue<T> m_queue1;
    LinkQueue<T> m_queue2;
    LinkQueue<T>* m_pIn;
    LinkQueue<T>* m_pOut;

    void move() const
    {
        int n = m_pIn->length() - 1;

        for(int i = 0; i < n; i++)
        {
            m_pOut->add(m_pIn->front());
            m_pIn->remove();
        }
    }

    void swap()
    {
        LinkQueue<T>* temp = NULL;

        temp = m_pIn;
        m_pIn = m_pOut;
        m_pOut = temp;
    }

public:
    QueueToStack()
    {
        m_pIn  = &m_queue1;
        m_pOut = &m_queue2;
    }

    void push(const T& e)
    {
        m_pIn->add(e);
    }

    void pop()
    {
        if( m_pIn->length() > 0 )
        {
            move();

            m_pIn->remove();

            swap();
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No element in current stack ...");
        }
    }

    T top() const
    {
        if( m_pIn->length() > 0 )
        {
            move();

            return m_pIn->front();
        }
        else
        {
            THROW_EXCEPTION(InvalidOperationException, "No element in current stack ...");
        }
    }

    void clear()
    {
        m_queue1.clear();
        m_queue2.clear();
    }

    int size() const
    {
        return m_queue1.length() + m_queue2.length();
    }
};


int main()
{
#if 0
    StackToQueue<int> sq;

    for(int i = 0; i < 10; i++)
    {
        sq.add(i);
    }

    while(sq.length() > 0)
    {
        cout << sq.front() << endl;
        sq.remove();
    }
 #endif

    QueueToStack<int> qs;

    for(int i = 0; i < 10; i++)
    {
        qs.push(i);
    }

    while(qs.size() > 0)
    {
        cout << qs.top() << endl;
        qs.pop();
    }
    return 0;
}
