#include "project.h"



void Project::hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars)
{
    for( lk::varhash_t::iterator v = vars.begin(); v != vars.end(); v++ )
    {
        unsigned char c = v->second->type();

        switch(c)
        {
            case lk::vardata_t::NUMBER:
                ssc_data_set_number(cxt, v->first.c_str(), v->second->as_number() );
            break;
            case lk::vardata_t::STRING:
                ssc_data_set_string(cxt, v->first.c_str(), v->second->as_string().c_str() );
            break;
            // case lk::vardata_t::HASH:
            // {
                
            //     // ssc_data_set_table(cxt, v->first.c_str(), 

            //     break;
            // }
            case lk::vardata_t::VECTOR:
            {
                //vector needs to contain either another vector or a simple type
                unsigned char cc = v->second->vec()->front().type();
                switch(cc)
                {
                    case lk::vardata_t::NUMBER:
                    {
                        size_t nv = v->second->vec()->size();
                        ssc_number_t *tvec = new ssc_number_t[nv];

                        for(size_t i=0; i<nv; i++)
                            tvec[i] = v->second->vec()->at(i).as_number();
                        
                        ssc_data_set_array(cxt, v->first.c_str(), tvec, nv);

                        delete [] tvec;
                        break;
                    }
                    case lk::vardata_t::VECTOR:
                    {
                        size_t nr, nc;
                        nr = v->second->vec()->size();
                        nc = v->second->vec()->front().vec()->size();

                        ssc_number_t *tvec = new ssc_number_t[nr*nc];

                        for( size_t i=0; i<nr; i++ )
                        {
                            std::vector< lk::vardata_t > *cr = v->second->vec()->at(i).vec();
                            for( size_t j=0; j<nc; j++ )
                                tvec[i*nc + j] = cr->at(j).as_number();
                        }

                        ssc_data_set_matrix(cxt, v->first.c_str(), tvec, nr, nc);

                        delete [] tvec;
                        break;
                    }
                    default:
                        //exception
                    break;
                }

                break;
            }
            default:
            break;
        }
    }
}

void Project::initialize_ssc_project(ssc_data_t &cxt)
{
    
}