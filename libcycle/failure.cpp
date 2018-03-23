#include "failure.h"
#include "distributions.h"
#include "well512.h"
#include <cmath>

//##################################################################################
FailureType::FailureType() {}

FailureType::FailureType(std::string component, std::string id,
	std::string failure_mode, std::string dist_type, double alpha, double beta)
{
	m_component = component;
	m_id = id;
	m_failure_mode = failure_mode;
	if (dist_type == "beta")
	{
		Distribution *bdist = new BetaDist(alpha, beta, dist_type);
		m_failure_dist = (BetaDist *) bdist;
	}
	else if (dist_type == "gamma")
	{
		Distribution *gdist = new GammaDist(alpha, beta, dist_type);
		m_failure_dist = (GammaDist *) gdist;
	}
	else if (dist_type == "exponential")
	{
		Distribution *edist = new ExponentialDist(alpha, beta, dist_type);
		m_failure_dist = (ExponentialDist *) edist;
	}
	else
		throw std::exception("invalid distribution type provided as input.");
	m_life_remaining = 0.0; 
	m_probability = 0.0;
}

std::string FailureType::GetComponentName()
{
	return m_component;
}

std::string FailureType::GetFailureID()
{
	return m_id;
}

std::string FailureType::GetFailureMode()
{
	return m_failure_mode;
}

Distribution *FailureType::GetFailureDist()
{
	return m_failure_dist;
}

double FailureType::GetLifeRemaining()
{
	return m_life_remaining;
}

void FailureType::SetLifeRemaining(double life_remaining)
{
	m_life_remaining = life_remaining;
}

void FailureType::ReduceLifeRemaining(double life_reduction)
{
	m_life_remaining -= life_reduction;
}

double FailureType::GetFailureProbability()
{
	return m_probability;
}

void FailureType::SetFailureProbability(double probability)
{
	m_probability = probability;
}

double FailureType::GetLifeOrProb()
{
	if (GetFailureDist()->IsBinary())
		return m_probability;
	else
		return m_life_remaining;
}

void FailureType::SetLifeOrProb(double life_remaining)
{
	if (GetFailureDist()->IsBinary())
		m_probability = life_remaining;
	else
		m_life_remaining = life_remaining;
}

void FailureType::GenerateTimeToFailure(WELLFiveTwelve &gen)
{
	/*
	generate a random 'lifetime', which is the number of hours of
	operation to a component failure given normal operations that do not
	increase the hazard rate.
	gen -- random U[0,1] generator object
	retval -- lifetime in adjusted hours of operation
	*/

	//For Gamma or Exponentially distributed failure distribution
	if (m_failure_dist->IsBinary())
		throw std::exception("lifetime generated with binary distribution.");
	m_life_remaining = m_failure_dist->GetVariate(gen);
}

void FailureType::GenerateFailureProbability(WELLFiveTwelve &gen)
{
	/*
	generate a random 'failure probability'
	gen -- random U[0,1] generator object
	*/

	//For Beta distributed failure probability distribution
	if (!(m_failure_dist->IsBinary()))
		throw std::exception("fail probability generated with non-beta distribution.");
	m_probability = m_failure_dist->GetVariate(gen);
}

void FailureType::GenerateFailureVariate(WELLFiveTwelve &gen)
{
	if (m_failure_dist->IsBinary())
		GenerateFailureProbability(gen);
	else
		GenerateTimeToFailure(gen);
}