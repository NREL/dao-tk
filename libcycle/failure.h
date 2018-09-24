#ifndef _COMPONENT_FAILURE_MODE_
#define _COMPONENT_FAILURE_MODE_

#include <string>
#include "distributions.h"
#include "well512.h"

class FailureType
{
	std::string m_component;
	std::string m_id;
	std::string m_failure_mode;
	Distribution *m_failure_dist;
	double m_duration;
	double m_probability;
	double m_life_remaining;

public:
	FailureType();

	FailureType(std::string component, std::string id, 
		std::string failure_mode, std::string dist_type,
		double alpha, double beta);

	std::string GetComponentName(); 

	std::string GetFailureID();
	
	std::string GetFailureMode();

	Distribution *GetFailureDist();

	double GetLifeRemaining();

	void SetLifeRemaining(double life_remaining);

	void ReduceLifeRemaining(double life_reduction);

	double GetFailureProbability();

	void SetFailureProbability(double probability);

	double GetLifeOrProb();

	void SetLifeOrProb(double life_remaining);

	void GenerateTimeToFailure(WELLFiveTwelve &gen);

	void GenerateFailureProbability(WELLFiveTwelve &gen);

	void GenerateFailureVariate(WELLFiveTwelve &gen);

};

#endif