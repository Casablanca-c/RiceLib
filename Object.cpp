#include "Object.h"
#include <cstdlib>
#include <iostream>

using namespace std;

namespace FLLib {

// 异常规格说明 throw()的作用：表示当前的重载函数是不会抛出任何异常的
void* Object::operator new(unsigned int size) throw()
{
    //cout << "Object::operator new: " << size << endl;
    return malloc(size);
}

void Object::operator delete(void* p)
{
    //cout << "Object::operator delete: " << p << endl;
    free(p);
}

void* Object::operator new[] (unsigned int size) throw()
{
    return malloc(size);
}

void Object::operator delete[](void* p)
{
    free(p);
}
bool Object::operator == (const Object& obj)
{
    return (this == &obj);
}

bool Object::operator != (const Object& obj)
{
    return (this != &obj);
}

Object::~Object()
{

}


}

