#include "stdafx.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <Python.h>
#include "src/proto.pb.h"
#include "sqlite3.h"
#include "CppSQLite3.h"
#include "protos.h"

using namespace std;

template <typename T> //templatizado en cpp
void foo ()
{
    std::cout << "Hola desde Foo" << std::endl;
}


template <typename T>
class Base
{
private:
    int _a;
public:
    Base() 
    { 
        _a=1;
    }
    void Set(int a){_a=a;}
    int Get(){return _a;}

    void doSomething()
    {
        T& derived = static_cast<T&>(*this);
        
    }

};
 
class Derived : public Base<Derived>
{
    
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





template <EnumCallType T>
struct JCallType
{
    typedef JCUCallTraits<T> Traits;

    static EnumCallType type () { return T; }
    static std::string name () { return callTypeToString (T); }
};


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


template <typename T1,typename T2,
          EnumCallType Jcu1CallType = EnumCallType::None,
          EnumCallType Jcu2CallType = EnumCallType::None,
          EnumCallType Jcu3CallType = EnumCallType::None,
          EnumCallType Jcu4CallType = EnumCallType::None,
          EnumCallType Jcu5CallType = EnumCallType::None,
          EnumCallType Jcu6CallType = EnumCallType::None>

struct JCUTest
{
    static void test ( std::string texto)
    {
        const std::size_t numExpectedAtlasCalls =
            Jcu1CallType == EnumCallType::None ? 0 :
            Jcu2CallType == EnumCallType::None ? 1 :
            Jcu3CallType == EnumCallType::None ? 2 :
            Jcu4CallType == EnumCallType::None ? 3 :
            Jcu5CallType == EnumCallType::None ? 4 :
            Jcu6CallType == EnumCallType::None ? 5 :
                                               6;

        typedef JCallType<Jcu1CallType>  CallType1;
        typedef JCallType<Jcu2CallType>  CallType2;
        typedef JCallType<Jcu3CallType>  CallType3;
        typedef JCallType<Jcu4CallType>  CallType4;
        typedef JCallType<Jcu5CallType>  CallType5;
        typedef JCallType<Jcu6CallType>  CallType6;


        std::cout << CallType1::name() << std::endl;
        std::cout << CallType2::name() << std::endl;
        std::cout << CallType3::name() << std::endl;
        std::cout << CallType4::name() << std::endl;
        std::cout << CallType5::name() << std::endl;
        std::cout << CallType6::name() << std::endl;
        
        
        auto Ret1 = pp <T1> ();
        auto Ret2 = pp <T2> ();
        Llamada<CallType1 ,1>();
        Llamada<CallType2 ,2>();
        Llamada<CallType3 ,3>();
        Llamada<CallType4 ,4>();
        Llamada<CallType5 ,5>();
        Llamada<CallType6 ,6>();
    }
};

/*
template <class RequestProto, class ResultProto>
static std::unique_ptr<ResultProto> libCall ()
{
    std::unique_ptr<ResultProto> result (new ResultProto);
    return result;
}
*/

template<EnumCallType T>
static std::unique_ptr<typename JCallType<T>::Traits::ResponseProto> libCall (typename JCallType<T>::Traits::RequestProto InProto)
{
    typedef JCallType<T> Type;
    typedef typename Type::Traits::ResponseProto ResultProto;
    
    std::unique_ptr<ResultProto> result (new ResultProto);
    
    return result;
    
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
    return "";
}



namespace prueba
{
    int _a;
    void Set(int a);
    int Get();
};
void prueba::Set(int a)
{
    _a=a;
}
int prueba::Get()
{
    return _a;
}


template <typename Proto, typename Container> 
std::string protoFromBinaryToJson (const Container & binaryProto)
{
    std::string outputJson;

    Proto proto;
    bool ok = proto.ParseFromArray (binaryProto.data(), static_cast<int>(binaryProto.size()));

    if (!ok)
        throw std::invalid_argument ("Cannot parse proto from array");

    ok = google::protobuf::TextFormat::PrintToString (proto, &outputJson);

    if (!ok)
        throw std::invalid_argument ("Cannot convert proto to JSON");

    return outputJson;
}

template <typename T> 
std::unique_ptr<T> parseProtoFromString (std::string text)
{
    std::unique_ptr<T> proto (new T);
    std::string strout;

    google::protobuf::TextFormat::Parser parser;


    if ( ! parser.ParseFromString(text, proto.get()))
        throw std::invalid_argument ("Could not parse input\n" + strout);

    return proto;
}


void PruebasSqLite()
{

     int rc;
   char *error;
 
   // Open Database
   cout << "Opening MyDb.db ..." << endl;
   sqlite3 *db;
   rc = sqlite3_open("MyDb.db", &db);
   if (rc)
   {
      cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
      sqlite3_close(db);
      return;
   }
   else
   {
      cout << "Opened MyDb.db." << endl << endl;
   }
 
   // Execute SQL
   cout << "Creating MyTable ..." << endl;
   const char *sqlCreateTable = "CREATE TABLE MyTable (id INTEGER PRIMARY KEY, value STRING);";
   rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
   if (rc)
   {
      cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
      sqlite3_free(error);
   }
   else
   {
      cout << "Created MyTable." << endl << endl;
   }
 
   // Execute SQL
   cout << "Inserting a value into MyTable ..." << endl;
   const char *sqlInsert = "INSERT INTO MyTable VALUES(NULL, 'A Value');";
   rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
   if (rc)
   {
      cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
      sqlite3_free(error);
   }
   else
   {
      cout << "Inserted a value into MyTable." << endl << endl;
   }
 
   // Display MyTable
   cout << "Retrieving values in MyTable ..." << endl;
   const char *sqlSelect = "SELECT * FROM MyTable;";
   char **results = NULL;
   int rows, columns;
   sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
   if (rc)
   {
      cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
      sqlite3_free(error);
   }
   else
   {
      // Display Table
      for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
      {
         for (int colCtr = 0; colCtr < columns; ++colCtr)
         {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
 
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
         }
 
         // End Line
         cout << endl;
 
         // Display Separator For Header
         if (0 == rowCtr)
         {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
               cout.width(12);
               cout.setf(ios::left);
               cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
         }
      }
   }
   sqlite3_free_table(results);
 
   // Close Database
   cout << "Closing MyDb.db ..." << endl;
   sqlite3_close(db);
   cout << "Closed MyDb.db" << endl << endl;
 
   // Wait For User To Close Program
   cout << "Please press any key to exit the program ..." << endl;
   cin.get();
}

void PruebasCppSQLite()
{
    try
    {
        int i, fld;
        time_t tmStart, tmEnd;
        CppSQLite3DB db;

        cout << "SQLite Header Version: " << CppSQLite3DB::SQLiteHeaderVersion() << endl;
        cout << "SQLite Library Version: " << CppSQLite3DB::SQLiteLibraryVersion() << endl;
        cout << "SQLite Library Version Number: " << CppSQLite3DB::SQLiteLibraryVersionNumber() << endl;

        remove("pruebas.db");
        db.open("pruebas.db");

        cout << endl << "emp table exists=" << (db.tableExists("emp") ? "TRUE":"FALSE") << endl;
        cout << endl << "Creating emp table" << endl;
        db.execDML("create table emp(empno int, empname char(20));");
        cout << endl << "emp table exists=" << (db.tableExists("emp") ? "TRUE":"FALSE") << endl;
        ////////////////////////////////////////////////////////////////////////////////
        // Execute some DML, and print number of rows affected by each one
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "DML tests" << endl;
        int nRows = db.execDML("insert into emp values (7, 'David Beckham');");
        cout << nRows << " rows inserted" << endl;

        nRows = db.execDML("update emp set empname = 'Christiano Ronaldo' where empno = 7;");
        cout << nRows << " rows updated" << endl;

        nRows = db.execDML("delete from emp where empno = 7;");
        cout << nRows << " rows deleted" << endl;

        ////////////////////////////////////////////////////////////////////////////////
        // Transaction Demo
        // The transaction could just as easily have been rolled back
        ////////////////////////////////////////////////////////////////////////////////
        int nRowsToCreate(25000);
        cout << endl << "Transaction test, creating " << nRowsToCreate;
        cout << " rows please wait..." << endl;
        tmStart = time(0);
		cout << "PRE-TXN AUTO COMMIT=" << (db.IsAutoCommitOn() ? "Y" : "N") << endl;
        db.execDML("begin transaction;");
		cout << "IN-TXN AUTO COMMIT=" << (db.IsAutoCommitOn() ? "Y" : "N") << endl;

        for (i = 0; i < nRowsToCreate; i++)
        {
            char buf[128];
            sprintf(buf, "insert into emp values (%d, 'Empname%06d');", i, i);
            db.execDML(buf);
        }

        db.execDML("commit transaction;");
		cout << "POST-TXN AUTO COMMIT=" << (db.IsAutoCommitOn() ? "Y" : "N") << endl;
        tmEnd = time(0);

        ////////////////////////////////////////////////////////////////////////////////
        // Demonstrate CppSQLiteDB::execScalar()
        ////////////////////////////////////////////////////////////////////////////////
        cout << db.execScalar("select count(*) from emp;") << " rows in emp table in ";
        cout << tmEnd-tmStart << " seconds (that was fast!)" << endl;

        ////////////////////////////////////////////////////////////////////////////////
        // Re-create emp table with auto-increment field
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "Auto increment test" << endl;
        db.execDML("drop table emp;");
        db.execDML("create table emp(empno integer primary key, empname char(20));");
        cout << nRows << " rows deleted" << endl;

        for (i = 0; i < 5; i++)
        {
            char buf[128];
            sprintf(buf, "insert into emp (empname) values ('Empname%06d');", i+1);
            db.execDML(buf);
            cout << " primary key: " << (int)db.lastRowId() << endl;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Query data and also show results of inserts into auto-increment field
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "Select statement test" << endl;
        CppSQLite3Query q = db.execQuery("select * from emp order by 1;");

        for (fld = 0; fld < q.numFields(); fld++)
        {
            cout << q.fieldName(fld) << "(" << q.fieldDeclType(fld) << ")|";
        }
        cout << endl;

        while (!q.eof())
        {
            cout << q.fieldValue(0) << "|";
            cout << q.fieldValue(1) << "|" << endl;
            q.nextRow();
        }


        ////////////////////////////////////////////////////////////////////////////////
        // SQLite's printf() functionality. Handles embedded quotes and NULLs
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "SQLite sprintf test" << endl;
        CppSQLite3Buffer bufSQL;
        bufSQL.format("insert into emp (empname) values (%Q);", "He's bad");
        cout << (const char*)bufSQL << endl;
        db.execDML(bufSQL);

        bufSQL.format("insert into emp (empname) values (%Q);", NULL);
        cout << (const char*)bufSQL << endl;
        db.execDML(bufSQL);

        ////////////////////////////////////////////////////////////////////////////////
        // Fetch table at once, and also show how to use CppSQLiteTable::setRow() method
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "getTable() test" << endl;
        CppSQLite3Table t = db.getTable("select * from emp order by 1;");

        for (fld = 0; fld < t.numFields(); fld++)
        {
            cout << t.fieldName(fld) << "|";
        }
        cout << endl;
        for (int row = 0; row < t.numRows(); row++)
        {
            t.setRow(row);
            for (int fld = 0; fld < t.numFields(); fld++)
            {
                if (!t.fieldIsNull(fld))
                    cout << t.fieldValue(fld) << "|";
                else
                    cout << "NULL" << "|";
            }
            cout << endl;
        }


        ////////////////////////////////////////////////////////////////////////////////
        // Test CppSQLiteBinary by storing/retrieving some binary data, checking
        // it afterwards to make sure it is the same
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "Binary data test" << endl;
        db.execDML("create table bindata(desc char(10), data blob);");
        
        unsigned char bin[256];
        CppSQLite3Binary blob;

        for (i = 0; i < sizeof bin; i++)
        {
            bin[i] = i;
        }

        blob.setBinary(bin, sizeof bin);

        bufSQL.format("insert into bindata values ('testing', %Q);", blob.getEncoded());
        db.execDML(bufSQL);
        cout << "Stored binary Length: " << sizeof bin << endl;

        q = db.execQuery("select data from bindata where desc = 'testing';");

        if (!q.eof())
        {
            blob.setEncoded((unsigned char*)q.fieldValue("data"));
            cout << "Retrieved binary Length: " << blob.getBinaryLength() << endl;
        }
		q.finalize();

        const unsigned char* pbin = blob.getBinary();
        for (i = 0; i < sizeof bin; i++)
        {
            if (pbin[i] != i)
            {
                cout << "Problem: i: ," << i << " bin[i]: " << pbin[i] << endl;
            }
        }


		////////////////////////////////////////////////////////////////////////////////
        // Pre-compiled Statements Demo
        ////////////////////////////////////////////////////////////////////////////////
        cout << endl << "Transaction test, creating " << nRowsToCreate*2;
        cout << " rows please wait..." << endl;
        db.execDML("drop table emp;");
        db.execDML("create table emp(empno int, empname char(20));");
        tmStart = time(0);
        db.execDML("begin transaction;");

        cout << endl << "Creating with bind by number" << endl;
        CppSQLite3Statement stmt = db.compileStatement("insert into emp values (?, ?);");
        for (i = 0; i < nRowsToCreate; i++)
        {
            char buf[16];
            sprintf(buf, "EmpName%06d", i);
            stmt.bind(1, i);
            stmt.bind(2, buf);
            stmt.execDML();
            stmt.reset();
        }

        cout << endl << "Creating with bind by name" << endl;
		CppSQLite3Statement stmt2 = db.compileStatement("insert into emp values (:p1, @p2);");
        for (i = 0; i < nRowsToCreate; i++)
        {
            char buf[16];
            sprintf(buf, "EmpName%06d", i);
			stmt2.bind(":p1", i);
			stmt2.bind("@p2", buf);
            stmt2.execDML();
            stmt2.reset();
        }

		db.execDML("commit transaction;");
        tmEnd = time(0);

        cout << db.execScalar("select count(*) from emp;") << " rows in emp table in ";
        cout << tmEnd-tmStart << " seconds (that was even faster!)" << endl;
        cout << endl << "End of tests" << endl;
    }
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }



}

void PruebasPython()
{
    double d = 0.5;
    Py_SetProgramName("prueba");  /* optional but recommended */
    Py_Initialize();

    PyObject* myModule = PyImport_ImportModule((char*)"javier");
    PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)"Lorentz");
    PyObject* arglist = Py_BuildValue("(d)", d);
    PyObject* myResult = PyObject_CallObject(myFunction, arglist);
    double result1 = PyFloat_AsDouble(myResult);

    myFunction = PyObject_GetAttrString(myModule,(char*)"DopplerRela");
    arglist = Py_BuildValue("(d)", d);
    myResult = PyObject_CallObject(myFunction, arglist);
    double result2 = PyFloat_AsDouble(myResult);

    myFunction = PyObject_GetAttrString(myModule,(char*)"DopplerRelaInv");
    arglist = Py_BuildValue("(d)", d);
    myResult = PyObject_CallObject(myFunction, arglist);
    double result3 = PyFloat_AsDouble(myResult);

    std::cout << "Lorentz(" << d << ") = " << result1 << std::endl;
    std::cout << "DopplerRela(" << d << ") = " << result2 << std::endl;
    std::cout << "DopplerRelaInv(" << d << ") = " << result3 << std::endl;

    Py_Finalize();    
}

inline void Pepe()
{
    static int a =0;
    a++;
    if (a>10000)
        return;
    Pepe();
    
}


void Duff()
{

    /*

    do { // se asume count > 0 
   *to++ = *from++;
} while (--count);

    */


    const int count = 20;
    int n = (count + 7) / 8;

    char Nombre[] = "pepeluismaripepeluismaripepeluismaripepeluismari";
    char otro[] = "esternocleidomastoideoesternocleidomastoideoesternocleidomastoideo";
 
    char *to = Nombre;
    char *from = otro;
    switch (count % 8)
    {
    case 0: do  { *to++ = *from++;
    case 7:      *to++ = *from++;
    case 6:      *to++ = *from++;
    case 5:      *to++ = *from++;
    case 4:      *to++ = *from++;
    case 3:      *to++ = *from++;
    case 2:      *to++ = *from++;
    case 1:      *to++ = *from++;
                } while (--n);
    }
}

void Lambda()
{
    int b = 10;
    int c = 100;
    auto f = [&](int& a) -> int 
    {
        b += 1;
        c +=1;
        return a + b + c;
    };
    int a = 10;
    std::cout << f(a) << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;
}



template <EnumCalc T>
void JCall()
{
    typedef typename CalcOfferCallType<T>::Traits::RequestProto CRequestProto;
    CRequestProto Req;
    int a = CalcOfferCallType<T>::Traits::Get(Req);
    int b = 0;
}

template <EnumCalc T>
void Function( std::unique_ptr< typename CalcOfferCallType<T>::Traits::RequestProto> calIn)
{
    calIn->SetA(0);
}
struct CFunc
{
public:
    int *rval0;
    int *rval1;
    int *rval2;
    int *rval3;

    CFunc()
    {
        rval0 = nullptr;
        rval1 = nullptr;
        rval2 = nullptr;
        rval3 = nullptr;
    }
    void set_val0(int &n)
    {
        rval0 = new (int);
        *rval0 = n;
    }
    int val0()
    {
        if (rval0!=nullptr) 
            return  *rval0;
        else return 0;
    }
    bool has_val0() 
    {
        if (rval0!=nullptr)
            return true;
        else
            return false;
    }

    void set_val1(int &n)
    {
        rval1 = new (int);
        *rval1 = n;
    }
    int val1()
    {
        if (rval1!=nullptr) 
            return *rval1;
        else return 0;
    }
    bool has_val1() 
    {
        if (rval1!=nullptr)
            return true;
        else
            return false;
    }


    void set_val2(int &n)
    {
        rval2 = new (int);
        *rval2 = n;
    }
    int val2()
    {
        if (rval2!=nullptr) 
            return *rval2;
        else return 0;
    }
    bool has_val2() 
    {
        if (rval2!=nullptr)
            return true;
        else
            return false;
    }


    void set_val3(int &n)
    {
        rval3 = new (int);
        *rval3 = n;
    }
    int val3()
    {
        if (rval3!=nullptr) 
            return *rval3;
        else return 0;
    }
    bool has_val3() 
    {
        if (rval3!=nullptr)
            return true;
        else
            return false;
    }
};


void FunctionPointers()
{
    struct ProtoFuncs
    {
        const char * name;
        bool (CFunc::*has)() ;
        int  (CFunc::*get)() ;
        void (CFunc::*set)(int &);
    };
    #define PROTO_FUNCS(X) { #X, &CFunc::has_##X, &CFunc::X, &CFunc::set_##X }

    const ProtoFuncs protoFuncs[] =
    {
        "val0",&CFunc::has_val0, &CFunc::val0, &CFunc::set_val0 ,
        PROTO_FUNCS (val1),
        PROTO_FUNCS (val2),
        PROTO_FUNCS (val3)
    };

    #undef PROTO_FUNCS
    int countTotal = 0;

    CFunc Func;

    for (auto & pf : protoFuncs)
    {
        if ((Func.*(pf.has))())
        {
            
            int r = (Func.*(pf.get)) ();
            cout << r << endl;
        }
        else
        {
            (Func.*(pf.set)) (countTotal);
            ++ countTotal;
        }
    }
    for (auto & pf : protoFuncs)
    {
        if ((Func.*(pf.has))())
        {
            int r = (Func.*(pf.get)) ();
            cout << r << endl;
        }
        else
        {
            ++ countTotal;
            (Func.*(pf.set)) (countTotal);
        }
    }
}

void PruebasC()
{
    std::vector<int> v = { 3, 4, 43 };
}
int main(int argc, char* argv[])
{

    PruebasC();

    Derived base;
    base.doSomething();


    prueba::Set(2);
    auto n = prueba::Get();


    const std::string filenameIn1 = Jcu::func<EnumCallType::Jcu_A>();
    const std::string filenameIn2 = Jcu::func<EnumCallType::Jcu_B>();

    std::unique_ptr<Jcu_A> protoA (new Jcu_A());
    std::unique_ptr<Jcu_B> protoB (new Jcu_B());
    

    std::unique_ptr<Jcu_A> R1 = libCall<EnumCallType::Jcu_A>(*protoA);

    JCUTest<
        JClass_A,
        JClass_A,
        EnumCallType::Jcu_A,
        EnumCallType::Jcu_B,
        EnumCallType::Jcu_B
    >::test("./tmp");

    
    std::string strText;
    std::unique_ptr<Respuesta> uniformResponse(new Respuesta());
    std::unique_ptr<RespuestaCorrecta> ED(new RespuestaCorrecta());

    ED->set_requestid("ID00001");
    ED->set_mensaje("Esto es una prueba");
    uniformResponse->set_allocated_response(ED.release());
    
    
    std::string ostr = protoFromBinaryToJson<Respuesta>(uniformResponse->SerializeAsString());

    
    std::ofstream fileo("sample.json");
    fileo << ostr;
    fileo.close();
    

    std::ifstream filei("sample.json");
    
    
    std::string str((std::istreambuf_iterator<char>(filei)), std::istreambuf_iterator<char>());

    filei.close();
    
    std::unique_ptr<Respuesta> uRespond = parseProtoFromString<Respuesta> (ostr);
    
    if (uRespond->has_response())
    {
        
        std::cout << uRespond->response().requestid();
        std::cout << std::endl;
        std::cout << uRespond->response().mensaje();
    }
    std::cout << endl;

    JCall<EnumCalc::Calc>();
    JCall<EnumCalc::ExecuteCalc>();

    CRequestA Req;
    std::unique_ptr<CRequestA> ReqA (new CRequestA());
    Function<EnumCalc::Calc>(std::move(ReqA));
        
    FunctionPointers();
    Lambda();
    Duff();
    PruebasPython();
    PruebasSqLite();
    PruebasCppSQLite();


    return 0;
}



