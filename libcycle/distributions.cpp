#include "distributions.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

//##################################################################################

Distribution::Distribution() {}

Distribution::Distribution(double alpha, double beta, std::string type) 
{
	m_alpha = alpha;
	m_beta = beta;
	m_type = type;
}

bool Distribution::IsBinary()
{
	return false;
}

double Distribution::GetVariate(WELLFiveTwelve &)
{
	return -1.0;
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
	return (-1.0 * log(1. - unif) * GetAlpha()) + GetBeta();
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
	alpha > 1.  Algorithm source:  George S. Fishman "Monte Carlo:  Concepts,
	Algorithms, and Applications" pgs 194, 197.
	Note for alpha = 1, uses exponential distribution.

	(here, alpha denotes the shape parameter and
	beta denotes the scale.)
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
	else if (GetAlpha() > 1 && GetAlpha() < 2.5)
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
	else if (GetAlpha() > 2.5)
	{
		double a = GetAlpha() - 1;
		double b = (GetAlpha() - 1 / (6 * GetAlpha())) / a;
		double m = 2 / a;
		double d = m + 2;
		double f = sqrt(GetAlpha());

		while (!success) {
			Z = -1;
			Y = 0;
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

//##################################################################################

BoundedJohnsonDist ::BoundedJohnsonDist () {}

BoundedJohnsonDist ::BoundedJohnsonDist (double gamma, double delta, double xi, double lambda, 
		std::string type) : Distribution(0, 0, type)
{
	m_gamma = gamma;
	m_delta = delta;
	m_xi = xi;
	m_lambda = lambda;
}

bool BoundedJohnsonDist ::IsBinary()
{
	return false;
}

double BoundedJohnsonDist ::RationalApproximation(double u)
{
	// Abramowitz and Stegun formula 26.2.23.
	// The absolute value of the error should be less than 4.5 e-4.
	double c[] = { 2.515517, 0.802853, 0.010328 };
	double d[] = { 1.432788, 0.189269, 0.001308 };
	return u - ((c[2] * u + c[1])*u + c[0]) /
		(((d[2]*u + d[1])*u + d[0])*u + 1.0);
}


double BoundedJohnsonDist::NormalCDFInverse(double p)
{
	if (p <= 0.0 || p >= 1.0)
	{
		std::stringstream os;
		os << "Invalid input argument (" << p
			<< "); must be larger than 0 but less than 1.";
		throw std::invalid_argument(os.str());
	}
	if (p < 0.5)
	{
		// F^-1(p) = - G^-1(p)
		return -RationalApproximation(sqrt(-2.0*log(p)));
	}
	else
	{
		// F^-1(p) = G^-1(1-p)
		return RationalApproximation(sqrt(-2.0*log(1 - p)));
	}
}

double BoundedJohnsonDist::GetInverseCDF(double u)
{
	double z = NormalCDFInverse(u);
	double e = (z - m_gamma) / m_delta;
	return m_lambda * ( exp(e) / (1 + exp(e)) ) + m_xi;
}

double BoundedJohnsonDist ::GetVariate(WELLFiveTwelve &gen)
{
	double unif = gen.getVariate();
	return GetInverseCDF(unif);
}

//##################################################################################

BetaDist::BetaDist() {}

BetaDist::BetaDist(double alpha, double beta, std::string type = "beta") :
	Distribution(alpha, beta, type)
{
	m_alpha_dist = GammaDist(GetAlpha(), 1, "gamma");
	m_beta_dist = GammaDist(GetBeta(), 1, "gamma");
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


GammaProcessDist::GammaProcessDist()
{
}

GammaProcessDist::GammaProcessDist(
	double b, 
	double c, 
	double beta, 
	std::string type
)
{
	/*The Gamma Process assumes a fixed scale parameter, and 
	a shape parameter that is a function of the length of time.*/
	m_b = b;
	m_c = c;
	m_beta = beta;
	m_type = type;
}

bool GammaProcessDist::IsBinary()
{
	return false;
}

std::string GammaProcessDist::GetType()
{
	return m_type;
}

double GammaProcessDist::GetBeta()
{
	return m_beta;
}

double GammaProcessDist::GetAlpha(double t, double delta_t)
{
	/* 
	Calculates alpha according to the gamma process function 
	A(t+delta_t) - A(t), 
	which determines the shape parameter for a particular interval.
	t -- elapsed time in the gamma process
	delta_t -- interval length
	*/
	if (m_type == "linear")
	{
		return m_b + m_c * delta_t;
	}
	if (m_type == "exponential")
	{
		return (
			std::exp(m_b + m_c * (t + delta_t)) - 
				std::exp(m_b + m_c * (t))
			);
	}
    
    return std::numeric_limits<double>::quiet_NaN();
}

double GammaProcessDist::GetMean(double t, double delta_t)
{
	return GetAlpha(t, delta_t) * GetBeta();
}

double GammaProcessDist::GetVariate(double alpha, WELLFiveTwelve &gen)
{
	/*
	generates a gamma distributed random variable for alpha < 1 and
	alpha > 1.  Algorithm source:  George S. Fishman "Monte Carlo:  Concepts,
	Algorithms, and Applications" pgs 194, 197.
	Note for alpha = 1, uses exponential distribution.

	(here, alpha denotes the shape parameter and
	beta denotes the scale.)

	alpha -- shape parameter of the distribution. 
	*/
	double W, X, Y, Z;
	double random1;
	double random2;
	bool success = false;

	int iterations = 0;

	// For alpha = 1, use exponential distrubution
	if (alpha == 1)
	{
		random1 = gen.getVariate();
		return GetBeta() * (-log(1 - random1));
	}

	// For alpha less than 1  //////////////////////
	if (alpha < 1)
	{
		double b = (alpha + exp(1)) / exp(1);

		while (!success)
		{
			iterations++;
			random1 = gen.getVariate();
			random2 = gen.getVariate();

			Y = b * random1;
			if (Y <= 1)
			{
				Z = pow(Y, (1 / alpha));
				W = -log(1 - random2);
				if (W >= Z)
				{
					return GetBeta()*Z;
				}
			}
			else {
				Z = -log((b - Y) / alpha);
				W = pow(random2, 1 / (alpha - 1));
				if (W <= Z)
				{
					return GetBeta()*Z;
				}
			}
		}
	}

	// For alpha greater than 1 and less than 2.5 //////////////////////
	else if (alpha > 1 && alpha < 2.5)
	{
		double a = alpha - 1;
		double b = (alpha - 1 / (6 * alpha)) / a;
		double m = 2 / a;
		double d = m + 2;

		while (!success)
		{
			iterations++;
			X = gen.getVariate();
			Y = gen.getVariate();
			double V = b * Y / X;

			if (m*X - d + V + pow(V, -1) <= 0)
			{
				return a * V*GetBeta();
			}
			if (m*log(X) - log(V) + V - 1 <= 0)
			{
				return a * V*GetBeta();
			}
		}
	}

	// For large alpha      //////////////////////
	else if (alpha > 2.5)
	{
		double a = alpha - 1;
		double b = (alpha - 1 / (6 * alpha)) / a;
		double m = 2 / a;
		double d = m + 2;
		double f = sqrt(alpha);

		while (!success) {
			Z = -1;
			Y = 0;
			while (!(Z > 0 && Z < 1)) {
				iterations++;
				X = gen.getVariate();
				Y = gen.getVariate();
				Z = Y + (1 - 1.857764 * X) / f;
			}
			double V = b * Y / Z;

			if (m*Z - d + V + pow(V, -1) <= 0)
			{
				return a * V*GetBeta();
			}
			if (m*log(Z) - log(V) + V - 1 <= 0)
			{
				return a * V*GetBeta();
			}
		}
	}

	return 0.0;
}

InvGammaDist::InvGammaDist() {}

double InvGammaDist::GetVariate(WELLFiveTwelve &gen)
{
	double var = GammaDist::GetVariate(gen);
	return 1.0 / var;
}