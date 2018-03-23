#include "distributions.h"
#include "well512.h"
#include <cmath>
#include <iostream>
#include <string>

//##################################################################################

Distribution::Distribution() {}

Distribution::Distribution(double alpha, double beta, std::string type) 
{
	m_alpha = alpha;
	m_beta = beta;
	m_type = type;
}

std::string Distribution::GetType()
{
	return m_type;
}


double Distribution::GetAlpha()
{
	return m_alpha;
}


double Distribution::GetBeta()
{
	return m_beta;
}

Distribution::~Distribution(){}

ExponentialDist::ExponentialDist() {}


ExponentialDist::ExponentialDist(double alpha, double beta, std::string type) :
	Distribution(alpha, beta, type) {}


bool ExponentialDist::IsBinary()
{
	return false;
}

double ExponentialDist::GetVariate(WELLFiveTwelve &gen)
{
	/*
	returns the equivalent of the ppf function in the scipy.stats
	package, that is, the inverse CDF of a U[0,1] random variate for this
	distribution
	*/
	double unif = gen.getVariate();
	return -1.0 * (log(1. - unif) / GetAlpha()) + GetBeta();
}

//##################################################################################

GammaDist::GammaDist() {}

bool GammaDist::IsBinary()
{
	return false;
}

double GammaDist::GetVariate(WELLFiveTwelve &gen)
{
	/*
	generates a gamma distributed random variable for alpha < 1 and
	aplha > 1.  Algorithm source:  George S. Fishman "Monte Carlo:  Concepts,
	Algorithms, and Applications" pgs 194, 197.
	Note for alpha = 1, uses exponential distribution.
	*/
	double W, X, Y, Z;
	double random1;
	double random2;
	bool success = false;

	int iterations = 0;

	// For alpha = 1, use exponential distrubution
	if (GetAlpha() == 1)
	{
		random1 = gen.getVariate();
		return GetBeta() * (-log(1 - random1));
	}

	// For alpha less than 1  //////////////////////
	if (GetAlpha() < 1)
	{
		double b = (GetAlpha() + exp(1)) / exp(1);

		while (!success)
		{
			iterations++;
			random1 = gen.getVariate();
			random2 = gen.getVariate();

			Y = b * random1;
			if (Y <= 1)
			{
				Z = pow(Y, (1 / GetAlpha()));
				W = -log(1 - random2);
				if (W >= Z)
				{
					return GetBeta()*Z;
				}
			}
			else {
				Z = -log((b - Y) / GetAlpha());
				W = pow(random2, 1 / (GetAlpha() - 1));
				if (W <= Z)
				{
					return GetBeta()*Z;
				}
			}
		}
	}

	// For alpha greater than 1 and less than 2.5 //////////////////////
	if (GetAlpha() > 1 && GetAlpha() < 2.5)
	{
		double a = GetAlpha() - 1;
		double b = (GetAlpha() - 1 / (6 * GetAlpha())) / a;
		double m = 2 / a;
		double d = m + 2;

		while (!success)
		{
			iterations++;
			X = gen.getVariate();
			Y = gen.getVariate();
			double V = b*Y / X;

			if (m*X - d + V + pow(V, -1) <= 0)
			{
				return a*V*GetBeta();
			}
			if (m*log(X) - log(V) + V - 1 <= 0)
			{
				return a*V*GetBeta();
			}
		}
	}

	// For large alpha      //////////////////////
	if (GetAlpha() > 2.5)
	{
		double a = GetAlpha() - 1;
		double b = (GetAlpha() - 1 / (6 * GetAlpha())) / a;
		double m = 2 / a;
		double d = m + 2;
		double f = sqrt(GetAlpha());

		while (!success) {
			Z = -1;
			while (!(Z > 0 && Z < 1)) {
				iterations++;
				X = gen.getVariate();
				Y = gen.getVariate();
				Z = Y + (1 - 1.857764 * X) / f;
			}
			double V = b*Y / Z;

			if (m*Z - d + V + pow(V, -1) <= 0)
			{
				return a*V*GetBeta();
			}
			if (m*log(Z) - log(V) + V - 1 <= 0)
			{
				return a*V*GetBeta();
			}
		}
	}

	return 0.0;
}

BetaDist::BetaDist() {}

BetaDist::BetaDist(double alpha, double beta, std::string type = "beta") :
	Distribution::Distribution(alpha, beta, type)
{
	m_alpha_dist = GammaDist::GammaDist(GetAlpha(), 1, "gamma");
	m_beta_dist = GammaDist::GammaDist(GetBeta(), 1, "gamma");
}

bool BetaDist::IsBinary()
{
	return true;
}

double BetaDist::GetVariate(WELLFiveTwelve &gen)
{
	double x = m_alpha_dist.GetVariate(gen);
	double y = m_beta_dist.GetVariate(gen);
	return x / (x + y);
}