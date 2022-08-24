from time import time,ctime
import math

def myabs(x):
    return math.fabs(x)

def mymul(x):
    return (x*x)

def Lorentz(x):
    return math.sqrt(1-x*x)

def DopplerRela(x):
    return math.sqrt((1-x)/(1+x))
	
def DopplerRelaInv(x):
    return math.sqrt((1+x)/(1-x))


print 'Today is',ctime(time())