#include"http.h"
template<typename T>
ldanar::basic_abstract<T>::basic_abstract(const T &v)
{
    this->val = v;
};
template<typename T>
bool ldanar::basic_abstract<T>::operator==(const basic_abstract &v)
{
    return v.val == this->val;
};
template<typename T>
bool ldanar::basic_abstract<T>::operator==(const T &v)
{
    return v == this->val;
};
template<typename T>
ldanar::basic_abstract<T> ldanar::basic_abstract<T>::operator= (const T v)
{
    return v;
};
template<typename T>
T ldanar::basic_abstract<T>::getbit(const T &v)
{
   return (bool)this->val &(1UL << v);
};
#define BUILD_BASIC_OPERATION_TEMPLATE2(t,x) \
        template<typename T>\
        t<T> t<T>::operator x (const T &v)\
        {return this->val x v;}
#define BUILD_BASIC_ABSTRACT_OPERATIONS(x) BUILD_BASIC_OPERATION_TEMPLATE2(ldanar::basic_abstract,x)
BUILD_BASIC_ABSTRACT_OPERATIONS(&);
BUILD_BASIC_ABSTRACT_OPERATIONS(|);
BUILD_BASIC_ABSTRACT_OPERATIONS(+);
BUILD_BASIC_ABSTRACT_OPERATIONS(-);
BUILD_BASIC_ABSTRACT_OPERATIONS(*);
BUILD_BASIC_ABSTRACT_OPERATIONS(/);
BUILD_BASIC_ABSTRACT_OPERATIONS(%);
BUILD_BASIC_ABSTRACT_OPERATIONS(^);
#undef BUILD_BASIC_ABSTRACT_OPERATIONS(x)
template<typename T>
ldanar::basic_abstract<T> ldanar::basic_abstract<T>::operator~ ()
{
    return ~(this->val);
};
