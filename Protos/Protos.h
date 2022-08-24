#pragma once

#ifndef __PROTOS_H__
#define __PROTOS_H__


template <typename T>
void foo ();

template
void foo<int> ();


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

class CRequestA
{
private:
    int _a;
public:
    CRequestA() 
    { 
        _a=1;
    }
    void SetA(int a){_a=a;}
    int GetA(){return _a;}
};
class CResultA
{
private:
    int _a;
public:
    CResultA() 
    { 
        _a=2;
    }
    void SetA(int a){_a=a;}
    int GetA(){return _a;}
};

class CRequestB
{
private:
    int _b;
public:
    CRequestB() 
    { 
        _b=3;
    }
    void SetB(int b){_b=b;}
    int GetB(){return _b;}
};
class CResultB
{
private:
    int _b;
public:
    CResultB() 
    { 
        _b=4;
    }
    void SetB(int b){_b=b;}
    int GetB(){return _b;}
};


enum class EnumCalc
{
    Calc,
    ExecuteCalc,
    None
};

template <EnumCalc T>
struct CalcOfferCallTraits
{
};

template <EnumCalc T>
struct CalcOfferCallType
{
    typedef CalcOfferCallTraits<T> Traits;

    static EnumCalc type () { return T; }
};


template <>
struct CalcOfferCallTraits<EnumCalc::None>
{
    static std::string suffix () { return "None"; }
};

template <>
struct CalcOfferCallTraits<EnumCalc::Calc>
{
    typedef CRequestA  RequestProto;
    typedef CResultA ResponseProto;

    static std::string suffix () { return "Llamada a CalcOfferPrice"; }

    static int Get( RequestProto & req)
    {
        int a = req.GetA();
        return a;
    }
};

template <>
struct CalcOfferCallTraits<EnumCalc::ExecuteCalc>
{
    typedef CRequestB  RequestProto;
    typedef CResultB ResponseProto;

    static std::string suffix () { return "Llamada a ExecutedCalcOfferPrice"; }
    static int Get( RequestProto & req)
    {
        int a = req.GetB();
        return a;
    }
};


template <EnumCalc T>
void JCall();
    
template
void JCall<EnumCalc::Calc>();

template
void JCall<EnumCalc::ExecuteCalc>();



#endif //__PROTOS_H__
