#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost::filesystem;

class Funda
{
public:
    virtual ~Funda () {}
   
};

class FundaDeSilla : public Funda
{
public:
    typedef Silla TipoDeAsiento;
   
};

class FundaDeTaburete : public Funda
{
public:
    typedef Taburete TipoDeAsiento;
   
};

class Funda;
class FundaDeSilla;
class FundaDeTaburete;
template <typename ASIENTO>
struct CaracteristicaDeAsiento
{
};

template <>
struct CaracteristicaDeAsiento<Silla>
{
    typedef FundaDeSilla TipoDeFunda;
};

template <>
struct CaracteristicaDeAsiento<Taburete>
{
    typedef FundaDeTaburete TipoDeFunda;
};

template <typename ASIENTO>
std::unique_ptr<Funda> CrearFundaEspecifica ()
{
    std::unique_ptr<Funda> funda (new typename CaracteristicaDeAsiento<ASIENTO>::TipoDeFunda());
    //...
    return funda;
}


int main(int argc, char* argv[])
{
  std::unique_ptr<Funda> fundaParaSilla = CrearFundaEspecifica<Silla> ();
    std::unique_ptr<Funda> fundaParaTaburete = CrearFundaEspecifica<Taburete> (); 
    return 0;
}