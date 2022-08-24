#ifndef __CFOCOS_H__
#define __CFOCOS_H__


#include <map>

double Lorent(double  v){return sqrt(1-(v*v));}

class CFoco
{
private:
	unsigned int ID;
    double timeInc;
    double Pos;
public:
    std::vector<CFoco *> Focus;

    CFoco(int _id)
    {
		ID = _id;
        timeInc = 1;
        Pos = 0;
    };

    void inc()
    {
		Pos += timeInc;
    }
    void ChangeAmp(double t,bool changeAll = true)
    {
        double factor = 0 ;
		if (t>=0)
			timeInc*= t;
		else
			timeInc /= -t;
		
        if (changeAll==true)
        {
            for (std::vector<CFoco *>::iterator it = Focus.begin();it!=Focus.end();++it)
            {
                CFoco *foco = *it;
                foco->ChangeAmp(-t,false);
            }
        }
    }
    double GetTime()
    {
        return timeInc;
    }
	double GetPos()
	{
		return Pos;
	}
};

class CListaFocos
{
private:
    std::map<unsigned int , CFoco *> focos;
public:
    CListaFocos(int numFocos)
    {
        for (int i = 0;i<numFocos;i++)
        {
            focos[i] = new CFoco(i);
        }
        RecalcFocos();
    };
    void RecalcFocos()
    {
        for (std::map<unsigned int , CFoco *>::iterator it = focos.begin();it!=focos.end();++it)
        {
            CFoco *foco = it->second;
            for (std::map<unsigned int , CFoco *>::iterator fit = focos.begin();fit!=focos.end();++fit)
            {
                if (it->first != fit->first)
                {
                    foco->Focus.push_back(fit->second);
                }
            }   
        }
    }

    void Add()
    {
        int id = focos.size();
        focos[id] = new CFoco(id);
    }
    CFoco * GetFoco(int ID)
    {
        return focos[ID];
    }
    void Inc()
    {
        for (std::map<unsigned int , CFoco *>::iterator it = focos.begin();it!=focos.end();++it)
        {
            it->second->inc();
        }
    }
};
#endif // __CFOCOS_H__