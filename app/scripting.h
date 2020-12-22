#ifndef _DAO_SCRIPTING
#define _DAO_SCRIPTING

#include <wex/lkscript.h>

//extern ssc_bool_t ssc_progress_handler(ssc_module_t, ssc_handler_t, int action, float f0, float f1, const char *s0, const char *, void *);

extern void _run_annual_sim(lk::invoke_t& cxt);
extern void _test(lk::invoke_t &cxt);
extern void _var(lk::invoke_t &cxt);
extern void _sscvar(lk::invoke_t &cxt);
extern void _varinfo(lk::invoke_t &cxt);
extern void _generate_solarfield(lk::invoke_t &cxt);
extern void _power_cycle(lk::invoke_t &cxt);
extern void _simulate_optical(lk::invoke_t &cxt);
extern void _simulate_solarfield(lk::invoke_t &cxt);
extern void _simulate_performance(lk::invoke_t &cxt);
extern void _initialize(lk::invoke_t &cxt);
extern void _simulate_financial(lk::invoke_t &cxt);
extern void _simulate_objective(lk::invoke_t &cxt);
extern void _setup_clusters(lk::invoke_t &cxt);
extern void _simulate_cycle(lk::invoke_t &cxt);
extern void _optimize(lk::invoke_t &cxt);

extern void _simulate_flux_profiles(lk::invoke_t &cxt);
extern void _clear_model(lk::invoke_t &cxt);

#endif