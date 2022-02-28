#include "Exception.h"
#include <cstring>
#include <cstdlib>

using namespace std;

namespace FLLib
{

void Exception::init(const char *message, const char *file, int line)
{
    m_message = (message ? strdup(message) : NULL); // glibc2.20 strdup函数没有对空指针进行处理

    if( file != NULL )
    {
        char sl[16] = {0};

        itoa(line, sl, 10);

        m_location = static_cast<char*>(malloc(strlen(file) + strlen(sl) + 2));

        if(m_location != NULL)
        {
            m_location = strcpy(m_location, file);
            m_location = strcat(m_location, ":");
            m_location = strcat(m_location, sl);
        }

    }
    else
    {
        m_location = NULL;
    }
}

Exception::Exception(const char* message)
{
    init(message, NULL, 0);
}

Exception::Exception(const char* file, int line)
{
    init(NULL, file, line);
}

Exception::Exception(const char* message, const char* file, int line)
{
    init(message, file, line);
}


// 拷贝构造以及赋值操作符要保证每一个 Exception 对象内部的
// m_message和m_location指针所指向的内容都是独立的一段堆空间
Exception::Exception(const Exception& e)
{
    m_message = strdup(e.m_message);
    m_location = strdup(e.m_location);
}

Exception& Exception::operator= (const Exception& e)
{
    if( this != &e)
    {
        free(m_message);
        free(m_location);

        m_message = strdup(e.m_message);
        m_location = strdup(e.m_location);
    }

    return *this;
}


const char* Exception::message() const
{
    return m_message;
}
const char* Exception::location() const
{
    return m_location;
}

Exception::~Exception()
{
    free(m_message);
    free(m_location);
}

}
