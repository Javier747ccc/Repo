#ifndef __UTIL_H__
#define __UTIL_H__
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <memory>

#include "util.h"
using namespace std;


class CNone
{
private:
    int _a;
public:
    CNone() 
    { 
        _a=0;
    }
    void Set(int a){_a=a;}
    int Get(){return _a;}
};

class Jcu_A
{
private:
    int _a;
public:
    Jcu_A() 
    { 
        _a=1;
    }
    void Set(int a){_a=a;}
    int Get(){return _a;}
};
class Jcu_B
{
private:
    int _b;
public:
    Jcu_B() 
    { 
        _b=2;
    }
    void Set(int b){_b=b;}
    int Get(){return _b;}
};

class JClass_A
{
private:
    int _a;
public:
    JClass_A() 
    { 
        _a=3;
    }
};
class JClass_B
{
private:
    int _b;
public:
    JClass_B() 
    { 
        _b=4;
    }
};


enum class EnumCallType
{
    Jcu_A,
    Jcu_B,
    None
};

template <EnumCallType T>
struct JCUCallTraits
{
};

template <>
struct JCUCallTraits<EnumCallType::None>
{
    typedef CNone  ResponseProto;
    typedef CNone  RequestProto;
    static std::string suffix () { return "None"; }
};

template <>
struct JCUCallTraits<EnumCallType::Jcu_A>
{
    typedef Jcu_A  ResponseProto;
    typedef Jcu_A  RequestProto;

    static std::string suffix () { return "Llamada a Jcu_A"; }
};

template <>
struct JCUCallTraits<EnumCallType::Jcu_B>
{
    typedef Jcu_B  ResponseProto;
    typedef Jcu_B  RequestProto;

    static std::string suffix () { return "Llamada a Jcu_B"; }
};


std::string callTypeToString (EnumCallType callType)
{
    struct
    {
        EnumCallType type;
        const char * name;
    }
    types[] =
    {
        { EnumCallType::Jcu_A,       "JCU_A" },
        { EnumCallType::Jcu_B,       "JCU_B" },
        { static_cast<EnumCallType>(-1),                nullptr }
    };

    for (unsigned i=0; types[i].name; i++)
    {
        if (callType == types[i].type)
        {
            return types[i].name;
        }
    }

    return "UNKNOWN";
}







template <typename T>
std::unique_ptr<T> pp ()
{
    std::unique_ptr<T> proto (new T);
    return proto;
}

template <typename T, unsigned CallNumber>
void Llamada()
{
    typedef typename T::Traits::ResponseProto Proto;

    std::unique_ptr<Proto> proto (new Proto);
    int n = 0;
    n = CallNumber;
    //T::JCallType::name();
    int n2 =  proto->Get();
    
    std::cout << T::Traits::suffix() << " Value=" << n2 << std::endl;
    
}




class Jcu
{
public:
    template<EnumCallType T>
    static const std::string func();
};

template<EnumCallType T>
const std::string Jcu::func()
{
    typedef typename JCallType<T>::Traits::ResponseProto Proto;
    std::unique_ptr<Proto> proto (new Proto);
    return "";
}


#endif