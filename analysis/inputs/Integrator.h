#ifndef INTEGRATOR_VICTOR_MIKHALEV
#define INTEGRATOR_VICTOR_MIKHALEV

#include "TF1.h"

class FUNCTOR{
private:
	TF1* func;

public:
	FUNCTOR(TF1* f){
		func=f;
	}
	inline double operator()(double* x, double* par){
		return Evaluate(x);
	}

private:
	inline double Evaluate(double* x){
		return x[0]*func->Eval(x[0]);
	}
};

class Integrator{
public:
	static double GetMean(TF1* f,const double x_min,const double x_max){
		FUNCTOR functor(f);

		TF1* f_x = new TF1("f_x",functor,x_min,x_max,0,"FUNCTOR");
		double result=f_x->Integral(x_min,x_max)/f->Integral(x_min,x_max);
		
		delete f_x;

		return result;
	}

};
#endif