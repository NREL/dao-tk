#include "structures.h"

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
//#include <time.h>
//#include <chrono>

Heliostat::Heliostat()
{
    operating = true;
    repair_remain = 0.;
    number_failure = 0;
};

Staff::Staff()
{
    queue.clear();
    queue_length.clear();
    repairs_made = 0;
    hours_worked = 0;
    hours_this_week = 0;
    hours_today = 0;
}

void simulate(settings &S, Results &R, std::string *results_file_name)
{

    //set up problem scaling
    //always scale the problem, ensures sampling repeatability
    double hscale = (double)S.n_helio_sim;
    double problem_scale = (float)S.n_helio/hscale;
    int n_helio_s = (int)hscale;


    std::vector< Heliostat > helios(n_helio_s, Heliostat() );
    std::vector< Staff > staff(S.n_om_staff, Staff() );

    int staff_last_assigned = -1; //index tracking which staff was most recently assigned a task

    std::vector< double > avail(S.n_hr_sim);

    double outage_time = 0.;

    int update_per = (int)(S.n_hr_sim/20.);

    //random generators
    std::default_random_engine rand(S.seed);
    std::chi_squared_distribution<double> chi2_2p2(2.2);
    std::default_random_engine rand2(S.seed + 1234);
    std::uniform_real_distribution<double> ureal(0., 1.0);
    //---------

    for(int t=0; t<S.n_hr_sim; t++)
    {
        
        //log the availability for this time step before doing repair operations
        int nop = 0;
        for(int i=0; i<n_helio_s; i++)
            nop += helios.at(i).operating ? 1 : 0;
        avail[t] = float(nop) / float(n_helio_s);
    
        //determine which heliostats fail this time step
        int hod = (t/24. - t/24)*24;
    
        //generate new failures
        //only during operating hours
        std::vector< Heliostat *> new_fails;
        if( hod > 6.9 && hod < 19 )
        {
            for(int i=0; i<n_helio_s; i++)
            {
                if( ureal(rand2) < 1./(float)S.mf )
                    new_fails.push_back( &helios.at(i) );
            }
        }

        //generate failure times for all new fails
        std::vector<double> times;
        for(int i=0; i<new_fails.size(); i++)
        {
            double ft = std::fmax(S.rep_min, std::fmin(chi2_2p2(rand), S.rep_max))*problem_scale;
            times.push_back( ft );
            outage_time += ft;
        }

        //update?
        if( t/float(update_per) == t/update_per )
            std::cout << t/float(S.n_hr_sim)*100. << " %\n";
    
        //Add the failure time to the heliostats
        for(int i=0; i<new_fails.size(); i++)
        {
            Heliostat *h = new_fails.at(i);

            if( h->operating)
            {
                //n_helio_out += 1;
                h->repair_remain += times[i];
                h->operating = false;
                h->number_failure += 1;
            }
        
            //assign this heliostat to a staff member
            staff_last_assigned += 1;
            if(staff_last_assigned == staff.size() )
                staff_last_assigned = 0;
        
            staff[staff_last_assigned].queue.push_back(h);
        }    
    
        int how = (t/(24*7.) - t/(24*7))*24*7;  //hour of the week
        if(how == 0)  //reset for new week
        {
            for( int j=0; j<staff.size(); j++)
                staff.at(j).hours_this_week = 0;
        }

        if( hod == 0 )
        {
            for(int j=0; j<staff.size(); j++)
                staff.at(j).hours_today = 0.;
        }

        //handle repairs
        for( int i=0; i<staff.size(); i++)
        {
            Staff *s = &staff.at(i);

            //limit hours worked
            if( s->hours_this_week < S.hr_prod && s->hours_today < 9 )
            {
    
                if( s->queue.size() == 0)
                {
                    //try to take jobs from other staff if possible
                    std::vector<int> sco; 
                    for(int j=0; j<staff.size(); j++)
                        sco.push_back(j);  //integer array of length (staff)

                    std::random_shuffle(sco.begin(), sco.end());    //randomly shuffle the array
                
                    for(int si=0; si<sco.size(); si++)   //Hi Jim - who I randomly bumped into...
                    {
                        if( &staff[si] == s )
                            continue;
                        if( staff[si].queue.size() > 1 ) //can I help you with anything?
                        {
                            s->queue.push_back( staff[si].queue.back() );
                            staff[si].queue.pop_back();
                            break;
                        }
                    }
                }
                //check again for any queued jobs
                if( s->queue.size() > 0)
                {
                    double step_remain = 1.;

                    while( step_remain > 0. )
                    {

                        //Do work on current job
                        if( s->queue.size() == 0 )
                            break;
                        Heliostat *hcur = s->queue[0];
                        double rr_save = hcur->repair_remain;
                        hcur->repair_remain += -step_remain;
                
                        if(hcur->repair_remain <= 0)
                        {
                            step_remain += -rr_save;
                            hcur->repair_remain = 0.;
                            hcur->operating = true;
                            s->queue.erase( s->queue.begin() );
                            s->repairs_made ++;
                        }
                        else
                        {
                            step_remain = 0.;
                        }
                    }
                    s->hours_worked += 1;
                    s->hours_this_week += 1;
                    s->hours_today += 1;
                }
            }
            //track the total queue length remaining for this staff
            double ql=0.;
            for(int hi=0; hi<s->queue.size(); hi++)
                ql += s->queue.at(hi)->repair_remain;
            
            s->queue_length.push_back( ql );
        }
    }


    //fill in the return data
    double hours_worked =0.;
    int repairs_made = 0;
    for (int s=0; s < staff.size(); s++)
    {
        hours_worked += staff.at(s).hours_worked;
        repairs_made += staff.at(s).repairs_made*problem_scale;
    }

    R.n_repairs = repairs_made;
    R.staff_utilization = (float)hours_worked * 168. / (float)(S.hr_prod * S.n_hr_sim * staff.size() );

    R.avail_schedule = new float[ S.n_hr_sim ];

    for(int i=0; i<S.n_hr_sim; i++)
        R.avail_schedule[i] = avail.at(i);
    R.n_avail_schedule = S.n_hr_sim;
    
    //if a file name is provided, write to that file
    if( results_file_name != 0)
    {
        //write the files
        std::ofstream ofs;
        ofs.open(*results_file_name, std::ofstream::out);

        ofs.clear();

        //header
        ofs << "hour,avail";
        for(int s=0; s<staff.size(); s++)
            ofs << ",staff_" << s;
        ofs << "\n";

        //summary
        ofs << "hrs_worked,";
        for(int s=0; s<staff.size(); s++)
            ofs << "," << staff.at(s).hours_worked;
        ofs << "\nrepairs,";
        for(int s=0; s<staff.size(); s++)
            ofs << "," << staff.at(s).repairs_made*problem_scale;
        ofs << "\n";

        //header
        ofs << "hour,avail";
        for(int s=0; s<staff.size(); s++)
            ofs << ",staff_" << s;
        ofs << "\n";

        //hourly data
        for(int t=0; t<S.n_hr_sim; t++)
        {
            ofs << t << "," << avail.at(t);
            for(int s=0; s<staff.size(); s++)
                ofs << "," << staff.at(s).queue_length.at(t);
            ofs << "\n";
        }

        ofs.close();
    }
}
