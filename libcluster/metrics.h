#ifndef _METRICS_
#define _METRICS_


#include "matrixtools.h"
#include <vector>
#include <string>
#include <unordered_map>

using std::unordered_map;


enum CSKYMODEL { MEINEL, HOTTEL };

struct data_feature
{
	double weight;       // Weighting factor (0-1)
	int divisions;		 // Number of division per day to define averaging periods
	bool daylight_only;  // Use summer daylight hours only when averaging?  Daylight hours defined as nonzero clearsky dni on Jun 21
};


struct s_metric_inputs
{
	int nsimdays;				// Number of days contained in each time block
	int nyears;					// Number of years of weather/price data

	bool is_remove_outliers;	// Remove outliers from price data used to create clusters?	 
	double stowlimit;			// Heliostat wind velocity stow limit (m/s).  DNI will be set to zero when above this value for clustering

	enum CSKYMODEL cskymodel;   // Model used for calculation of clearsky DNI

	std::vector<std::string> weather_files;		// Weather files

	std::vector<double> prices;					// Price
	bool is_price_files;						// Are prices specified by filename? 
	std::vector<std::string> price_files;		// Price files

	std::vector<double> sfavail;				// Solar field availability

	unordered_map<std::string, data_feature> features;

};


struct s_metric_outputs
{
	int nobs;							// Number of data points
	int nfeatures;						// Number of features per data point
	matrix<double> data;				// Matrix of data points for clustering (all points which can be exemplars)
	matrix<double> data_firstday;		// Data from first day (excluded from clustering)
	matrix<double> data_lastday;		// Data from last day (excluded from clustering)
	matrix<double> daily_dni;			// Total daily DNI (kWh per day) 

	int summer_sunrise;		// First point above DNI cutoff on summer solstice
	int n_daylight_pts;		// Number of daylight points on summer solstice
};





class clustering_metrics
{

	std::vector<std::string> split(const std::string &line, char delim);

	void clear_results();

	bool read_csv(const std::string &csvfile, std::vector<double>&data);

	bool read_weather(const std::string &weatherfile, double &lat, double &lon, double &elev, int &tz, int &year, bool &is_tmy, std::vector<double>&dni, std::vector<double>&wspd, std::vector<double>&tdry);

	void approximate_clearsky(int model, double lat, double lon, double elev, int year, int tz, bool is_tmy, double tstephr, std::vector<double>&csky);


public:

	s_metric_inputs inputs;
	s_metric_outputs results;

	clustering_metrics();

	~clustering_metrics() {};

	void set_default_weights();

	void set_default_inputs();

	void calc_metrics();
};


#endif