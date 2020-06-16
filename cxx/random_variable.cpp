#include <iostream>
#include <random>
#include <vector>
#include <stdint.h>
#include <stdlib.h>



#include "random_variable.h"


template<class T>
UniformRandomVariable<T>::UniformRandomVariable(T min,T max,int n)
{
	std::random_device rd;
	std::mt19937_64 gen(rd());

	std::uniform_real_distribution<double> distribution(min,max);

	min_ = min;
	max_ = max;
	n_ = n;

	for (int i = 0;i < n;++ i)
		values.push_back(distribution(gen));
}

template<>
UniformRandomVariable<int>::UniformRandomVariable(int min,int max,int n)
{
	std::random_device rd;
	std::mt19937_64 gen(rd());

	std::uniform_int_distribution<int> distribution(min,max);

	min_ = min;
	max_ = max;
	n_ = n;

	for (int i = 0;i < n;++ i)
		values.push_back(distribution(gen));
}

template<class T>
UniformRandomVariable<T>::~UniformRandomVariable()
{
		values.clear();
}


GeometricRandomVariable::GeometricRandomVariable(double p)
{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::default_random_engine generator;
	p_ = p;
	std::geometric_distribution<int> distribution(p_);
	
	value_ = distribution(gen);
}

int* UniformIntRandomVariable(int min,int max, int n)
{
	UniformRandomVariable<int> rg(min,max,n);
	int* values = new int[n];
	for (int i = 0;i < n;++ i)
		values[i] = rg.values[i];
	return values;
}

double* UniformDoubleRandomVariable(double min,double max, int n)
{
	UniformRandomVariable<double> rg(min,max,n);
	double* values = new double[n];

	for (int i = 0;i < n;++ i)
		values[i] = rg.values[i];

	return values;
}

int CGeometricRandomVariable(double p)
{
	GeometricRandomVariable rg(p);
	return rg.value_;
}