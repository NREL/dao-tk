#ifndef _FLUXSIMTHREAD_
#define _FLUXSIMTHREAD_ 1


#include "project.h"

#include <thread>
#include <mutex>

class FluxSimThread
{
    bool
        Finished,
        CancelFlag,
        FinishedWithErrors;

    int _thread_id;
    int _thread_ct;
    int _nsim_complete;
    ssc_data_t _ssc_data;

    Project *_P;
    std::vector<std::string> _sim_messages;


    //wxMutex
    std::mutex
        StatusLock,
        CancelLock,
        FinishedLock,
        FinErrLock;

public:
    std::vector< std::vector< double > > _results;

    void Setup(int thread_num, int thread_ct, Project *P, ssc_data_t ssc_data);

    void CancelSimulation();

    bool IsSimulationCancelled();

    bool IsFinished();

    bool IsFinishedWithErrors();

    void UpdateStatus(int ncomp, int ntot);

    void GetStatus(int *ncomp, int *ntot);

    std::vector<std::string> *GetSimMessages();    //can be called only after simulation is terminated

    void StartThread();

};


#endif 

