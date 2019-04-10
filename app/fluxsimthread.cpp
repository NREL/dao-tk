#include "fluxsimthread.h"


ssc_bool_t ssc_fluxthread_handler(ssc_module_t, ssc_handler_t, int action, float f0, float, const char *, const char *, void *data)
{

    if (action == SSC_UPDATE)
    {
        FluxSimThread* F = static_cast<FluxSimThread*>(data);
        
        //we don't know how many total simulations are being run, so just base the progress on a scale of 0-99
        F->UpdateStatus((int)f0, 100);

        return !F->IsSimulationCancelled();
    }
    else if (action == SSC_LOG)
        return 1;
    else 
        return 0;
}


void FluxSimThread::Setup(int thread_id, int thread_ct, Project *P, ssc_data_t ssc_data)
{
    /*
    Assign all of the arguments to local memory
    */
    _thread_id = thread_id;
    _thread_ct = thread_ct;
    _P = P;
    _ssc_data = ssc_data;
    Finished = false;
    CancelFlag = false;

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

    ssc_data_set_number(_ssc_data, "thread_id", _thread_id);
    ssc_data_set_number(_ssc_data, "thread_ct", _thread_ct);
    ssc_data_set_number(_ssc_data, "calc_fluxmaps", 1.);
};

void FluxSimThread::CancelSimulation()
{
    CancelLock.lock();
    CancelFlag = true;
    CancelLock.unlock();
}

bool FluxSimThread::IsSimulationCancelled()
{
    bool r;
    CancelLock.lock();
    r = CancelFlag;
    CancelLock.unlock();
    return r;
}

bool FluxSimThread::IsFinished()
{
    bool f;
    FinishedLock.lock();
    f = Finished;
    FinishedLock.unlock();
    return f;
}

bool FluxSimThread::IsFinishedWithErrors()
{
    bool f;
    FinErrLock.lock();
    f = FinishedWithErrors;
    FinErrLock.unlock();
    return f;
}

void FluxSimThread::UpdateStatus(int nsim_complete, int)
{
    StatusLock.lock();
    _nsim_complete = nsim_complete;
    StatusLock.unlock();
}

void FluxSimThread::GetStatus(int *nsim_complete, int *nsim_total)
{
    StatusLock.lock();
    *nsim_complete = _nsim_complete;
    *nsim_total = 100;
    StatusLock.unlock();
}

std::vector<std::string> *FluxSimThread::GetSimMessages()
{
    return &_sim_messages;
}

void FluxSimThread::StartThread() //Entry()
{
    /*
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
        
        ssc_module_t mod_solarpilot = ssc_module_create("solarpilot");

        bool is_cancel;

        StatusLock.lock();
        is_cancel = this->CancelFlag;
        StatusLock.unlock();

        // Run simulation 
        ssc_bool_t resok = ssc_module_exec_with_handler(mod_solarpilot, _ssc_data, ssc_fluxthread_handler, (void*)this);
            
        if(!resok)
        {
            message_handler("Flux simulation failed");
            int ty; float tms;
            for (int k = 0; ; k++)
            {
                const char *msg = ssc_module_log(mod_solarpilot, k, &ty, &tms);
                if (!msg)
                    break;
                message_handler(msg);
            }
                
            ssc_module_free(mod_solarpilot);
                
            FinErrLock.lock();
            FinishedWithErrors = true;
            FinErrLock.unlock();

            return;
        }

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

        //collect the results
        _results.clear();
        int nrow, ncol;
        ssc_number_t *opteff_table = ssc_data_get_matrix(_ssc_data, "opteff_table", &nrow, &ncol);

        int nflux;
        ssc_number_t *flux_table = ssc_data_get_matrix(_ssc_data, "flux_table", &nrow, &nflux);

        _results.resize(nrow, std::vector<double>(ncol + nflux));

        for (size_t i = 0; i < nrow; i++)
        {
            for (size_t j = 0; j < ncol; j++)
                _results[i][j] = opteff_table[i*3 + j];
            for (size_t j = 0; j < nflux; j++)
                _results[i][j + ncol] = flux_table[i*nflux + j];
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

        _sim_messages.push_back("Thread " + std::to_string(this->_thread_id) + ": " + e.what());
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

        _sim_messages.push_back("Thread " + std::to_string(this->_thread_id) + ": " + "Caught unspecified error in a flux simulation thread. Simulation was not successful.");
    }

    return;

};


