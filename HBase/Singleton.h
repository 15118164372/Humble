
#ifndef H_SINGLETON_H_
#define H_SINGLETON_H_

#include "Macros.h"

H_BNAMSP

//¼òµ¥µ¥Àý
template <typename T>
class CSingleton
{
public:
    CSingleton(void)
    {
        m_Singleton = static_cast< T* >(this);
    };
    virtual ~CSingleton(void)
    {
        m_Singleton = NULL;
    };

    static T& getSingleton(void)
    {
        return (*m_Singleton);
    };
    static T* getSingletonPtr(void)
    {
        return m_Singleton;
    };

private:
    H_DISALLOWCOPY(CSingleton);

private:
    static T *m_Singleton;
};

#define SINGLETON_INIT(T)							\
    template <>	 T * CSingleton<T>::m_Singleton = NULL;	\

H_ENAMSP

#endif//H_SINGLETON_H_
