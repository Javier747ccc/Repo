#include <map>
#include <iostream>
#include <deque>
#include "CFocos.h"


inline double Lorentz(double v)
{
	double l = sqrt(1.0-(v*v));
	return l;
}

CFocos::CFocos()
{
	_nextID = 0;
}

CFocos::~CFocos()
{
}

void CFocos::Step()
{

}

CFoco * CFocos::NewFoco()
{
	
	CFoco *pf = new CFoco();
	pf->SetId(_nextID);
	
	for (std::map<unsigned long int, CFoco*>::iterator it = _Focos.begin(); it != _Focos.end(); ++it)
	{
		CFoco *f = it->second;
        Distancia *E1 = pf->AddDistancia(f);
        Distancia *E2  = f->AddDistancia(pf);
        E1->SetRetorno(E2);
        E2->SetRetorno(E1);
	}
	_Focos[_nextID] = pf;

	_nextID++;
    return pf;
}

CFoco * CFocos::NewFoco(double initTime)
{
    CFoco *f = NewFoco();
    return f;
}
void CFocos::inc()
{
	for (std::map<unsigned long int, CFoco*>::iterator it = _Focos.begin(); it != _Focos.end(); ++it)
	{
		it->second->inc();
        
	}
}

void CFocos::SetZeroAlarm(unsigned int n, bool v)
{
    std::map<unsigned long int, CFoco*>::iterator it = _Focos.find(n);
	if (it == _Focos.end())
	{
		printf("Error ID:%ud Not Found!!!",n);
		return;
	}
    CFoco *SelectF = it->second;
    vector<Distancia*> *Distancias = it->second->GetListaDistancias();
    for (vector<Distancia*>::iterator itf = Distancias->begin(); itf != Distancias->end(); ++itf)
	{
        Distancia* e = *itf;
        e->SetAlarmZero(v);
    }
}

void CFocos::ActualizeMainTime(unsigned int n, unsigned int t)
{
    std::map<unsigned long int, CFoco*>::iterator it = _Focos.find(n);
	if (it == _Focos.end())
	{
		printf("Error ID:%ud Not Found!!!",n);
		return;
	}
    CFoco *SelectF = it->second;
    vector<Distancia*> *Distancias = it->second->GetListaDistancias();
    for (vector<Distancia*>::iterator itf = Distancias->begin(); itf != Distancias->end(); ++itf)
	{
        Distancia* e = *itf;
        e->SetActualTime(t);
    }
}

void CFocos::ChangeAmp(unsigned int n, double ve)
{
	std::map<unsigned long int, CFoco*>::iterator it = _Focos.find(n);
	if (it == _Focos.end())
	{
		printf("Error ID:%ud Not Found!!!",n);
		return;
	}
    

    CFoco *SelectF = it->second;

    SelectF->AddVelMaster(ve);
    double totalVel = SelectF->GetVelMaster();

    double lz = sqrt(1-totalVel*totalVel);
    double d1 = sqrt((1+totalVel)/(1-totalVel));
    double k1 = lz*d1;
    double k2 = (1.0/lz)*d1;
    
    SelectF->Amp(lz); 
	
    
    vector<Distancia*> *Distancias = it->second->GetListaDistancias();

	for (vector<Distancia*>::iterator itf = Distancias->begin(); itf != Distancias->end(); ++itf)
	{
        Distancia* distancia = *itf;
        distancia->setInc(k1);
        CFoco *foco = distancia->GetFoco();

        Distancia *distancia2 = distancia->GetRetorno();
        distancia2->setInc(k1);
	}
}

void CFocos::PrintState()
{
    for (std::map<unsigned long int, CFoco*>::iterator it = _Focos.begin(); it != _Focos.end(); ++it)
	{
        CFoco *foco = it->second; 
        foco->printState();
    }
}
void CFocos::PrintTimes()
{
    for (std::map<unsigned long int, CFoco*>::iterator it = _Focos.begin(); it != _Focos.end(); ++it)
	{
        CFoco *foco = it->second; 
        foco->printTimes();
    }
}

void CFocos::DrawTimes(std::deque<std::deque<std::pair<double, double> >> *deqFactors)
{
    std::deque<std::pair<double, double> > DefF;
    for (std::map<unsigned long int, CFoco*>::iterator it = _Focos.begin(); it != _Focos.end(); ++it)
	{
        CFoco *foco = it->second; 
       int a = 0;
    }
}