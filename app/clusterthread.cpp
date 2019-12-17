#include "clusterthread.h"


void ClusterThread::Setup(std::string &tname, Project *P, cluster_sim *C, ssc_data_t ssc_data, std::vector<double> *sf_avail,
    matrix<double>* avg_sfavail, result_map *results, keylist *keys, int sim_g_start, int sim_g_end)
{
    /*
    Assign all of the arguments to local memory
    */
    _thread_id = tname;
    _P = P;
    _C = C;
    _result = results;
    _keys = keys;
    Finished = false;
    CancelFlag = false;
    _sim_g_start = sim_g_start;
    _sim_g_end = sim_g_end;
    _nsim_total = sim_g_end - sim_g_start;
    _sf_avail = sf_avail;
    _avg_sfavail = avg_sfavail;

    //copy over all of the data in the ssc_data object into a new object to avoid memory conflicts
    _ssc_data = ssc_data_create();

    const char* vname = ssc_data_first(ssc_data);
    while (vname != SSC_INVALID)
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

void ClusterThread::CancelSimulation()
{
    CancelLock.lock();
    CancelFlag = true;
    CancelLock.unlock();
}

bool ClusterThread::IsSimulationCancelled()
{
    bool r;
    CancelLock.lock();
    r = CancelFlag;
    CancelLock.unlock();
    return r;
}

bool ClusterThread::IsFinished()
{
    bool f;
    FinishedLock.lock();
    f = Finished;
    FinishedLock.unlock();
    return f;
}

bool ClusterThread::IsFinishedWithErrors()
{
    bool f;
    FinErrLock.lock();
    f = FinishedWithErrors;
    FinErrLock.unlock();
    return f;
}

void ClusterThread::UpdateStatus(int nsim_complete, int nsim_total)
{
    StatusLock.lock();
    _nsim_complete = nsim_complete;
    _nsim_total = nsim_total;
    StatusLock.unlock();
}

void ClusterThread::GetStatus(int *nsim_complete, int *nsim_total)
{
    StatusLock.lock();
    *nsim_complete = _nsim_complete;
    *nsim_total = _nsim_total;
    StatusLock.unlock();
}

std::vector<std::string> *ClusterThread::GetSimMessages()
{
    return &_sim_messages;
}

void ClusterThread::StartThread() //Entry()
{
    /*
    This method duplicates the functionality of SolarField::LayoutSimulate(...)

    This method is intended to be thread safe and can be called by the GUI directly. Each thread must have
    its own instance of _SF. Before running multiple threads, create a solar field object, prepare it with
    PrepareFieldLayout(...), and use the deep copy constructor in SolarField to create as many duplicate
    objects as there are threads. Call this method for each duplicate object.

    */
    try {

        FinErrLock.lock();
        FinishedWithErrors = false;
        FinErrLock.unlock();
        _sim_messages.clear();


        //Simulate for each time
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
        
        ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

        for (int g = _sim_g_start; g < _sim_g_end; g++) 
        {

            //do------


            bool is_cancel;

            StatusLock.lock();
            is_cancel = this->CancelFlag;
            StatusLock.unlock();


            int ncount = _C->inputs.combined.n.at(g) * _C->inputs.days.ncount;	  // Number of days counting toward results in this group
            int nsim_nom = ncount + _C->inputs.days.nprev + _C->inputs.days.nnext;  // Nominal total number of days simulated in this group

            int exemplar = _C->inputs.combined.start.at(g);	// First cluster exemplar in this combined group

            int d1 = _C->firstday(exemplar);					// First day to be counted in group g
            int d0 = _C->firstsimday(exemplar);				// First day to be simulated in group g

            int nprev = d1 - d0;									// Actual number of previous days simulated
            int nsim = ncount + nprev + _C->inputs.days.nnext;		// Actual number of total days simulated

            double tstart = d0 * 24 * 3600;
            double tend = std::min((d0 + nsim)*24. * 3600, 8760.*3600.);

            ssc_number_t wf_steps_per_hour;
            ssc_data_get_number(_ssc_data, "time_steps_per_hour", &wf_steps_per_hour);
            int nperday = (int)wf_steps_per_hour * 24;

            // Update solar field hourly availability to reflect cluster-average values for this exemplar simulation
            std::vector<double> sfavail_sim = *_sf_avail;
            if (_C->inputs.is_clusteravg_sfavail)
            {
                std::vector<double> current_sfavail = _C->compute_combined_clusteravg(g, *_avg_sfavail);  // Cluster average solar field availability for combined group g
                for (int h = 0; h < nperday*nsim_nom; h++)
                {
                    int p = (d1 - _C->inputs.days.nprev) * nperday + h;
                    if (p > 0 && p < 365 * nperday)
                        sfavail_sim.at(p) = current_sfavail.at(h);
                }
            }

            // Set initial storage charge state
            double tes_charge = _C->inputs.initial_hot_charge;
            if (_C->inputs.is_initial_charge_heuristic)
            {
                int d = std::max((d1 - _C->inputs.days.nprev - 1), 0);  // Day before first simulated day
                double prev_dni = _P->metric_outputs.daily_dni.at(d, 0);
                tes_charge = _C->initial_charge_heuristic(prev_dni, _P->m_variables.solarm.as_number());
            }


            // Update inputs: 
            ssc_data_set_number(_ssc_data, "time_start", tstart);
            ssc_data_set_number(_ssc_data, "time_stop", tend);
            ssc_data_set_number(_ssc_data, "vacuum_arrays", 1);
            ssc_data_set_number(_ssc_data, "time_steps_per_hour", wf_steps_per_hour);
            ssc_data_set_number(_ssc_data, "csp.pt.tes.init_hot_htf_percent", tes_charge);

            int nr = (int)sfavail_sim.size();
            ssc_number_t *p_vals = new ssc_number_t[nr];
            for (int i = 0; i < nr; i++)
                p_vals[i] = 100.*(1. - sfavail_sim.at(i));
            ssc_data_set_array(_ssc_data, "sf_adjust:hourly", p_vals, nr);
            delete[] p_vals;


            // Run simulation 
            //if (!ssc_module_exec_with_handler(mod_mspt, _ssc_data, ssc_progress_handler, 0))
            if(!ssc_module_exec(mod_mspt, _ssc_data))
            {
                message_handler("SSC simulation failed");
                int ty; float tms;
                for (int k = 0; ; k++)
                {
                    const char *msg = ssc_module_log(mod_mspt, k, &ty, &tms);
                    if (!msg)
                        break;
                    message_handler(msg);
                }
                
                ssc_module_free(mod_mspt);
                
                FinErrLock.lock();
                FinishedWithErrors = true;
                FinErrLock.unlock();

                return;
            }

            // Fill in exemplar time block in full year array with ssc solution
            int doy_full = d1;
            int doy_sim = nprev;
            for (size_t k = 0; k < _keys->size(); k++)
            {
                std::string key = (*_keys)[k];
                ssc_number_t *p_data = ssc_data_get_array(_ssc_data, key.c_str(), &nr);
                for (int h = 0; h < nperday*ncount; h++)
                    (*_result)[key].at(doy_full*nperday + h) = p_data[doy_sim*nperday + h];
            }
			//single output values (enter as individual vectors)
			ssc_number_t tic;
			ssc_data_get_number(_ssc_data, "total_installed_cost", &tic);
			(*_result)["total_installed_cost_v"] = { tic };
            //------------------------------------

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
        ssc_data_free(_ssc_data);

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


