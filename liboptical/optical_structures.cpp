#include "optical_structures.h"

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <stdio.h>
//#include <chrono>

Heliostat::Heliostat()
{
    refl_base = 1.;
    soil_loss = 1.;
    age_hours = 0;
    soil_history = 0;
    refl_history = 0;
};

Heliostat::~Heliostat()
{
    if( soil_history != 0 )
        delete [] soil_history;
    if( refl_history != 0 )
        delete [] refl_history;
}

Crew::Crew()
{
    current_heliostat = -1;
    carryover_wash_time = 0.;
    replacements_made = 0;
    hours_this_week = 0.;
    hours_today = 0.;
}

void simulate(settings &S, Results &R, int seed, std::string *results_file_name, std::string *trace_file_name)
{
    //validation
    if( S.n_helio < 1 )
    {
        std::cout << "**** Invalid simulation parameter: ****\nNumber of heliostats is " << S.n_helio << "\n";
        return;
    }



    //scale the problem
    int n_helio_s;
    double wash_units_per_hour_s;
    double problem_scale = 1.;
    double hscale = 250.;
    bool do_trace = trace_file_name != 0;
    //always scale the problem, ensures sampling repeatability
    problem_scale = (float)S.n_helio/hscale;

    n_helio_s = (int)hscale;
    wash_units_per_hour_s = S.wash_units_per_hour/problem_scale;
    //-------------------

    std::vector< Heliostat > helios(n_helio_s, Heliostat());
    if(do_trace)
    {
        for(int i=0; i<n_helio_s; i++)
        {
            //helios.push_back( Heliostat() );
            helios.at(i).refl_history = new double[ S.n_hr_sim ];
            helios.at(i).soil_history = new double[ S.n_hr_sim ];
        }
    }

    std::vector< Crew > crews;
    for(int i=0; i<S.n_wash_crews; i++)
        crews.push_back( Crew() );


    std::vector< double > soil(S.n_hr_sim);
    std::vector< double > degr(S.n_hr_sim);
    std::vector< int > repr(S.n_hr_sim);
    std::vector< double > repr_cum(S.n_hr_sim);

    int update_per = (int)(S.n_hr_sim/20.);

    //random generators
    double soil_scalar = 100.;
    double degr_scalar = 1000.;
    unsigned seed1 = seed; //std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rand(seed1);
    std::chi_squared_distribution<double> chi2_soil(S.soil_loss_per_hr*soil_scalar);
    std::chi_squared_distribution<double> chi2_degr(S.degr_loss_per_hr*degr_scalar);
    unsigned seed2 = seed + 12354; //std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rand2(seed2);
    //std::uniform_int_distribution<int> uint(0,n_helio_s-1);
    //---------

    //initialize where each crew starts in the field
    int inc = (int)((float)n_helio_s/(float)crews.size());
    for(int c=0; c<crews.size(); c++)
        crews.at(c).current_heliostat = c*inc;
    int n_replacements_cumu = 0;

    //create the degradation rate by age
    double degr_accel = (1. + S.degr_accel_per_year/8760.);
    std::vector< float > degr_mult_by_age(S.n_hr_sim);
    degr_mult_by_age[0] = degr_accel;
    for(int t=1; t<S.n_hr_sim; t++)
        degr_mult_by_age[t] = degr_mult_by_age[t-1]*degr_accel;

    for(int t=0; t<S.n_hr_sim; t++)
    {

        if( t%(int)(S.n_hr_sim/20.) == 0)
            std::cout << ((float)t/(float)S.n_hr_sim)*100. << "% Complete\n";

        for(std::vector<Heliostat>::iterator h=helios.begin(); h!= helios.end(); h++)
        {
            //soil each heliostat
            double ss = 1. - chi2_soil(rand)/soil_scalar;
            ss = ss > 0. ? ss : 0.;

            //degrade each heliostat
            double dd = 1. - chi2_degr(rand2)/degr_scalar*degr_mult_by_age[ h->age_hours ];
            dd = dd > 0. ? dd : 0.;

            h->refl_base *= dd;
            h->soil_loss *= ss;
            h->age_hours ++; 

            if(do_trace)
            {
                h->refl_history[t] = h->refl_base;
                h->soil_history[t] = h->soil_loss;
            }
        }
        
        //reset time limits if needed
        if( t % 24 == 0 )
            for( std::vector<Crew>::iterator crew=crews.begin(); crew!=crews.end(); crew++)
                crew->hours_today = 0.;

        if( t % (24*7) == 0 )
            for( std::vector<Crew>::iterator crew=crews.begin(); crew!=crews.end(); crew++)
                crew->hours_this_week = 0.;

        int n_replacements_t = 0;
        //each crew attends to X number of heliostats
        for( std::vector<Crew>::iterator crew=crews.begin(); crew!=crews.end(); crew++)
        {
            
            //check crew availability
            if( crew->hours_this_week >= S.hours_per_week ||
                crew->hours_today >= S.hours_per_day)
                continue;

            //double crew_time = 0.;
            double units_washed_remain = wash_units_per_hour_s;
            for(;;)
            {
                //if the current heliostat is out of range, reset to 0
                if( crew->current_heliostat > n_helio_s-1 )
                    crew->current_heliostat = 0;
                
                int this_heliostat = crew->current_heliostat;
                
                bool do_break = false;

                //take care of any remaining time on this heliostat
                if( crew->carryover_wash_time > 0. )
                {
                    units_washed_remain += -crew->carryover_wash_time;

                    if( units_washed_remain <= 0. )
                    {
                        crew->carryover_wash_time = -units_washed_remain;
                        do_break = true;
                    }
                    else
                    {
                        crew->carryover_wash_time = 0.;
                        helios.at(crew->current_heliostat).soil_loss = 1.;  //reset
                        crew->current_heliostat ++;
                    }
                }
                else
                {
                    units_washed_remain += -1.;

                    if( units_washed_remain <= 0. )
                    {
                        crew->carryover_wash_time = -units_washed_remain;
                        do_break = true;
                    }
                    else
                    {
                        crew->carryover_wash_time = 0.;
                        helios.at(crew->current_heliostat).soil_loss = 1.;  //reset
                        crew->current_heliostat ++;
                    }
                }

                //make repairs as needed
                if( helios.at(this_heliostat).refl_base < S.replacement_threshold )
                {
                    crew->replacements_made ++;
                    n_replacements_t ++;
                    n_replacements_cumu ++;
                    helios.at(this_heliostat).age_hours = 0;
                    helios.at(this_heliostat).refl_base = 1.;
                }
                
                if(do_break)
                    break;
            }
            
            //track time spent
            crew->hours_today ++;
            crew->hours_this_week ++;

 
        }

        
        //log averages
        double refl_ave = 0.;
        double soil_ave = 0.;
        for(int i=0; i<helios.size(); i++)
        {
            refl_ave += helios.at(i).refl_base;
            soil_ave += helios.at(i).soil_loss;
        }
        
        soil.at(t) = soil_ave / (float)helios.size();
        degr.at(t) = refl_ave / (float)helios.size();
        repr.at(t) = n_replacements_t*problem_scale;
        repr_cum.at(t) = n_replacements_cumu*problem_scale;
    }


    //fill in the return data
    double hours_worked =0.;
    int replacements_made = 0;
    for (int s=0; s < crews.size(); s++)
    {
        //hours_worked += crews.at(s).hours_worked;
        replacements_made += crews.at(s).replacements_made;
    }

    R.n_replacements = replacements_made*problem_scale;

    R.soil_schedule = new float[ S.n_hr_sim ];
    R.degr_schedule = new float[ S.n_hr_sim ];
    R.repl_schedule = new float[ S.n_hr_sim ];
    R.repl_total = new float[ S.n_hr_sim ];

    for(int i=0; i<S.n_hr_sim; i++)
    {
        R.soil_schedule[i] = soil.at(i);
        R.degr_schedule[i] = degr.at(i);
        R.repl_schedule[i] = repr.at(i);
        R.repl_total[i] = repr_cum.at(i);
    }
    R.n_schedule = S.n_hr_sim;
    
    //if a file name is provided, write to that file
    if( results_file_name != 0)
    {
        //write the files
        std::ofstream ofs;
        ofs.open(*results_file_name, std::ofstream::out);

        ofs.clear();

        //summary
        ofs << "\nrepairs by crew,";
        for(int s=0; s<crews.size(); s++)
            ofs << "," << crews.at(s).replacements_made*problem_scale;
        ofs << "\n";


        ofs << "hour,soil,degr,tot,repl,cumu\n";
        //hourly data
        for(int t=0; t<S.n_hr_sim; t++)
        {
            ofs << t << "," << soil.at(t) << "," << degr.at(t) << "," << soil.at(t)*degr.at(t) << "," 
                << repr.at(t) << "," << repr_cum.at(t) << "\n";
        }

        ofs.close();
    }

    if( trace_file_name != 0 )
    {
        //write the files
        std::ofstream ofs;
        ofs.open(*trace_file_name, std::ofstream::out);

        ofs.clear();

        //headers
        for(int i=0; i<helios.size(); i++)
            ofs << "degr_" << i << ",";
        for(int i=0; i<helios.size(); i++)
            ofs << "soil_" << i << (i<helios.size()-1 ? "," : "\n");

        for(int t=0; t<S.n_hr_sim; t+=24*7)
        {
            //degradation
            int hct=0;
            for(std::vector<Heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
                ofs << h->refl_history[t] << ",";

            //soiling
            hct=0;
            for(std::vector<Heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
                ofs << h->soil_history[t] << ( hct++ < n_helio_s-1 ? "," : "\n" );
        }

        ofs.close();

    }
}
