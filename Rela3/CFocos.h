#ifndef DEF_CFocos_h
#define DEF_CFocos_h
#include <map>
#include <vector>
#include <deque>
using namespace std;

class CFoco;
class Distancia
{
    double _inc;
    double _time;
    CFoco *Foco;
    Distancia *Retorno;

    bool AlarmZero;
    unsigned int ActualTime;
    
public:
    Distancia()
    {
        _inc = 1.0;
        _time = 0;
        Retorno = nullptr;
        Foco = nullptr;
        AlarmZero = false;
        ActualTime = 0;
    }
    void SetFoco(CFoco *f)
    {
        Foco = f;
    }
    void SetActualTime(unsigned int t)
    {
        ActualTime = t;
    }
    void SetAlarmZero(bool t)
    {
        AlarmZero= t;
    }
    void setInc(double d)
    {
        _inc = d;
    }
    void TimeInc(double t)
    {
        _time+=(_inc*(1.0/t))/1000.0;
        if (AlarmZero)
        {
            if (_time<((double)ActualTime/1000.0))
            {
                throw std::invalid_argument ("Stop");
            }
        }
    }
    double GetInc()
    {
        return (_inc);
    }
    double GetTime()
    {
        return _time;
    }
    CFoco *GetFoco()
    {
        return Foco;
    }
    void SetRetorno(Distancia *e)
    {
        Retorno = e;
    }
    Distancia *GetRetorno()
    {
        return Retorno;
    }
};
class CFoco
{
private:
	double _time_inc;
    double _VelFromMaster;
	unsigned long int _ID;
    std::vector<Distancia*> _Distancias;
    double curve;
    std::deque<std::pair<double, double>> History;
public:
	CFoco(unsigned long int nID)
	{
		_ID = nID;
		_time_inc = 1.0;
        curve = 0;
        _VelFromMaster = 0;
	};
	CFoco()
	{
		_ID = 0;
		_time_inc = 1.0;
        curve = 0;
        _VelFromMaster = 0;
	};

	~CFoco()
	{
	};

    void AddVelMaster(double vel)
    {
        _VelFromMaster+=vel;
    }
    double GetVelMaster()
    {
        return _VelFromMaster;
    }
    std::vector<Distancia*> *GetListaDistancias()
    {
        return &_Distancias;
    }
	void Clear()
	{
		_time_inc = 1.0;
        curve = 0;
	};
	void inc()
	{
		for (vector<Distancia*>::iterator itf = _Distancias.begin(); itf != _Distancias.end(); ++itf)
	    {
            Distancia* e = *itf;
            e->TimeInc(_time_inc);
        }
	};
	unsigned long int GetId(){ return _ID; };
	void SetId(unsigned long int n ){ _ID = n; };
    void SAmp(double _curve)
	{
		_time_inc = _curve;
	}
	void Amp(double _curve)
	{
		_time_inc = _curve;
        History.push_back(std::make_pair(10,_curve));
	}
    
    Distancia *AddDistancia(CFoco *f)
	{
        Distancia *distancia = new Distancia();
        distancia->SetFoco(f);
        _Distancias.push_back(distancia);
        return distancia;
	}
    void printState()
    {
        std::cout << "[" << GetId() << "]  |  (";
        std::cout << _time_inc << ")    ";
		for (vector<Distancia*>::iterator itf = _Distancias.begin(); itf != _Distancias.end(); ++itf)
	    {
            Distancia* e = *itf;
            std::cout << "[" << e->GetFoco()->GetId() << "]" << ":" << e->GetInc() << "    ";
        }
        std::cout << endl;
    }
    void printTimes()
    {
        std::cout << "[" << GetId() <<+ "]  |  (";
		for (vector<Distancia*>::iterator itf = _Distancias.begin(); itf != _Distancias.end(); ++itf)
	    {
            Distancia* e = *itf;
            std::cout << "[" << e->GetFoco()->GetId() << "]" << ":" << e->GetTime() << "    ";
        }
        std::cout << endl;
    }
};

class CFocos
{
	std::map<unsigned long int, CFoco*> _Focos;
	unsigned long int _nextID;
public:
	CFocos();
	~CFocos();
	void Step();
	CFoco * NewFoco();
    CFoco * NewFoco(double initTime);
	void inc();
	void ChangeAmp(unsigned int n, double v);
    void SetZeroAlarm(unsigned int n, bool v);
    void ActualizeMainTime(unsigned int n, unsigned int t);
    void PrintState();
    void PrintTimes();
    void DrawTimes(std::deque<std::deque<std::pair<double, double> >> *deqFactors);
};

#endif //DEF_CFocos_h