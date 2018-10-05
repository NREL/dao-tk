#include "failure.h"
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
		throw std::runtime_error("invalid distribution type provided as input.");
	//m_life_remaining = 0.0; 
	//m_probability = 0.0;
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

double FailureType::GenerateTimeToFailure(WELLFiveTwelve &gen)
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
		throw std::runtime_error("lifetime generated with binary distribution.");
	return m_failure_dist->GetVariate(gen);
}

double FailureType::GenerateFailureProbability(WELLFiveTwelve &gen)
{
	/*
	generate a random 'failure probability'
	gen -- random U[0,1] generator object
	*/

	//For Beta distributed failure probability distribution
	if (!(m_failure_dist->IsBinary()))
		throw std::runtime_error("fail probability generated with non-beta distribution.");
	return m_failure_dist->GetVariate(gen);
}

double FailureType::GenerateFailureVariate(WELLFiveTwelve &gen)
{
	if (m_failure_dist->IsBinary())
		return GenerateFailureProbability(gen);
	else
		return GenerateTimeToFailure(gen);
}
