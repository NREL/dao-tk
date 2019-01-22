#include "cyclethread.h"


void CycleThread::Setup(
	std::string &tname,
	Project *P,
	PowerCycle *C,
	ssc_data_t ssc_data,
	std::vector<double> *t_amb,
	std::vector<double> *thermal_gen,
	std::vector<double> *elec_gen,
	result_map *results,
	keylist *keys,
	int sim_g_start,
	int sim_g_end
)
{
	/*
	Assign all of the arguments to local memory
	*/
	_thread_id = tname;
	_P = P;
	_C = C;
	_ssc_data = ssc_data;
	_result = results;
	_keys = keys;
	Finished = false;
	CancelFlag = false;
	_sim_g_start = sim_g_start;
	_sim_g_end = sim_g_end;
	_nsim_total = sim_g_end - sim_g_start;
	_t_amb = t_amb;
	_thermal_gen = thermal_gen;
	_elec_gen = elec_gen;

	//copy over all of the data in the ssc_data object into a new object to avoid memory conflicts
	_ssc_data = ssc_data_create();

	const char* vname = ssc_data_first(ssc_data);
	while (vname != '\0')
	{
		switch (ssc_data_query(ssc_data, vname))
		{
		case SSC_NUMBER:
		{
			ssc_number_t v;
			ssc_data_get_number(ssc_data, vname, &v);
			ssc_data_set_number(_ssc_data, vname, v);
			break;
		}
		case SSC_STRING:
			ssc_data_set_string(_ssc_data, vname, ssc_data_get_string(ssc_data, vname));
			break;
		case SSC_ARRAY:
		{
			int n;
			ssc_number_t* vals = ssc_data_get_array(ssc_data, vname, &n);
			ssc_data_set_array(_ssc_data, vname, vals, n);
			break;
		}
		case SSC_MATRIX:
		{
			int n, m;
			ssc_number_t* vals = ssc_data_get_matrix(ssc_data, vname, &n, &m);
			ssc_data_set_matrix(_ssc_data, vname, vals, n, m);
			break;
		}
		}
		vname = ssc_data_next(ssc_data);
	}


};

void CycleThread::CancelSimulation()
{
	CancelLock.lock();
	CancelFlag = true;
	CancelLock.unlock();
}

bool CycleThread::IsSimulationCancelled()
{
	bool r;
	CancelLock.lock();
	r = CancelFlag;
	CancelLock.unlock();
	return r;
}

bool CycleThread::IsFinished()
{
	bool f;
	FinishedLock.lock();
	f = Finished;
	FinishedLock.unlock();
	return f;
}

bool CycleThread::IsFinishedWithErrors()
{
	bool f;
	FinErrLock.lock();
	f = FinishedWithErrors;
	FinErrLock.unlock();
	return f;
}

void CycleThread::UpdateStatus(int nsim_complete, int nsim_total)
{
	StatusLock.lock();
	_nsim_complete = nsim_complete;
	_nsim_total = nsim_total;
	StatusLock.unlock();
}

void CycleThread::GetStatus(int *nsim_complete, int *nsim_total)
{
	StatusLock.lock();
	*nsim_complete = _nsim_complete;
	*nsim_total = _nsim_total;
	StatusLock.unlock();
}

std::vector<std::string> *CycleThread::GetSimMessages()
{
	return &_sim_messages;
}

void CycleThread::StartThread() //Entry()
{
	/*
	This method duplicates the functionality of SolarField::LayoutSimulate(...)

	This method is intended to be thread safe and can be called by the GUI 
	directly. Before running multiple threads, create and load a power cycle 
	object, prepare it using outputs from S(), and use the deep copy 
	constructor to create as many PowerCycle objects as there are threads. 
	Call this method for each duplicate object.

	*/
	try {

		FinErrLock.lock();
		FinishedWithErrors = false;
		FinErrLock.unlock();
		_sim_messages.clear();


		//Simulate for each scenario
		{
			StatusLock.lock();
			bool is_cancel = this->CancelFlag; //check for cancelled simulation
			StatusLock.unlock();
			if (is_cancel) {
				FinishedLock.lock();
				Finished = true;
				FinishedLock.unlock();
				return; // (wxThread::ExitCode)-1;
			}
		}

		int nsim = _sim_g_end - _sim_g_start + 1;

		for (int g = _sim_g_start; g < _sim_g_end; g++)
		{

			//do------


			bool is_cancel;

			StatusLock.lock();
			is_cancel = this->CancelFlag;
			StatusLock.unlock();
			


			// Update scenario number and simulate.
			// loading of plant state is contained within SingleScen.
			_C->SetScenarioIndex(g);
			_C->SingleScen(false, false, false, g == _sim_g_start);

			//Update progress
			UpdateStatus(g - _sim_g_start + 1, nsim);
			//Check for user cancel
			StatusLock.lock();
			is_cancel = this->CancelFlag;
			StatusLock.unlock();
			if (is_cancel) {
				FinishedLock.lock();
				Finished = true;
				FinishedLock.unlock();
				return;
			}
		}

		FinishedLock.lock();
		Finished = true;
		FinishedLock.unlock();

	}
	catch (std::runtime_error &e)
	{
		/* Handle exceptions within a thread by adding the exception to a list and returning normally */
		StatusLock.lock();
		this->CancelFlag = true;
		StatusLock.unlock();

		FinishedLock.lock();
		Finished = true;
		FinishedLock.unlock();

		FinErrLock.lock();
		FinishedWithErrors = true;
		FinErrLock.unlock();

		_sim_messages.push_back("Thread " + this->_thread_id + ": " + e.what());
	}
	catch (...)
	{
		/* Handle exceptions within a thread by adding the exception to a list and returning normally */
		StatusLock.lock();
		this->CancelFlag = true;
		StatusLock.unlock();

		FinishedLock.lock();
		Finished = true;
		FinishedLock.unlock();

		FinErrLock.lock();
		FinishedWithErrors = true;
		FinErrLock.unlock();

		_sim_messages.push_back("Thread " + this->_thread_id + ": " + "Caught unspecified error in a simulation thread. Simulation was not successful.");
	}

	return;

};


