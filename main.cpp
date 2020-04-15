/* Purpose:
	Author: W. Matt Jolly
	Date: 30 Mar 2020
	Version: 0.0

   This program is part of the netCDF tutorial:
   http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-tutorial


*/

#include <iostream>
#include <netcdf>
#include <sstream>
#include <vector>
#include <ctime>
#include "deadfuelmoisture.h"
#include "livefuelmoisture.h"
#include "NFDR2016CalcState.h"
#include "nfdrs2016.h"
#define STATESIZE 1017
#include "omp.h"

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

// We are writing 4D data, a 2 x 6 x 12 lvl-lat-lon grid, with 2ll
// timesteps of data.
static const int NLAT = 1597;
static const int NLON = 2345;
static const int NREC = 24;

// Return this code to the OS in case of failure.
static const int NC_ERR = 2;

// Helper Functions
// Split a string on a delimiter
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

// Create a time_t date from date values
time_t GetDate(int yr,int yday)
{

        struct tm t = { 0 };
	t.tm_sec = t.tm_min = t.tm_hour = 0; 
	t.tm_mon = 0;                        
	t.tm_year = yr - 1900;
	t.tm_isdst = -1;                     
	t.tm_mday = yday;
        time_t when = mktime(&t);
        return when;
}

time_t GetDate(int yr, int mon, int day, int hr = 0, int min = 0)
{

        struct tm t = { 0 };
	t.tm_sec = 0;
	t.tm_min = min; t.tm_hour = hr; 
	t.tm_mon = mon -1;                       
	t.tm_year = yr - 1900;
	t.tm_isdst = -1;                     
	t.tm_mday = day;
	time_t when = mktime(&t);
        return when;
}

string usage = "GridNFDRS <infile> <initfile>\n";

int main(int argc,char * argv[])
{
	// These arrays will store the latitude and longitude values.
	float lats[NLAT], lons[NLON];

	// These arrays will hold the data we will read in. We will only
	// need enough space to hold one timestep of data; one record.
	const long aSize = NLAT * NLON;
	static float temp_in[NREC][aSize];
	static float tmax[aSize];
	static float tmin[aSize];
	static float rh_in[NREC][aSize];
	static float rhmax[aSize];
	static float rhmin[aSize];
	static float prcp_in[NREC][aSize];
	static float prcp24[aSize];
	static float ws_in[NREC][aSize];
	static float sr_in[NREC][aSize];
	static float urma_topo[aSize][4];
	string InitFilename;
	bool Init = false;
	if(argc != 2) { cout << usage << endl; return 0;}
	enum {EXE, INFILE,INITFILE};
	ifstream fin("urma_topo_precip.flt",ios::binary);
	fin.read((char *) &urma_topo[0][0], sizeof(urma_topo));
	fin.close();
	enum {ELEV,LON,LAT,ANNPRCP};
	cout << urma_topo[10000][ELEV] << " " << urma_topo[10000][LON] << " " << urma_topo[10000][LAT] << " " << urma_topo[10000][ANNPRCP] << endl;
	
	cout << omp_get_num_procs() << endl;
	if(argc == 3) {InitFilename = argv[INITFILE];} else { Init = true; }
	try
	{
	   	// Open the file.
	     	NcFile dataFile(argv[INFILE], NcFile::read);
		NcVar latVar, lonVar,timeVar;
	     	timeVar = dataFile.getVar("time"); if(timeVar.isNull()) return NC_ERR;
	   	NcVarAtt att;
		att = timeVar.getAtt("units"); if(att.isNull()) return NC_ERR;
		string units;
		att.getValues(units);
		
	   	string delimiter = " ";
	    	vector<string> v = split (units, delimiter);
	    	string sDate = v[2]; string sTime = v[3];

		v = split(sDate,"-");
		int mon = atoi(v[1].c_str()); int yr = atoi(v[0].c_str()); int day = atoi(v[2].c_str());
		v = split(sTime,":");
		int hr = atoi(v[0].c_str()); int min = atoi(v[1].c_str());
		time_t when = GetDate(yr,mon,day,hr);
	    	const struct tm *norm = localtime(&when);   // Normalized time
		char buf[sizeof "2011-10-08T07:07:09Z"];
		strftime(buf, sizeof buf, "%FT%TZ",norm);
	    	std::cout << buf << endl;
		strftime(buf, sizeof buf, "%j",norm);
		int jday = atoi(buf);
		cout << jday << endl;


		// Get the latitude and longitude variables and read data.
	   	latVar = dataFile.getVar("y");
		if(latVar.isNull()) return NC_ERR;
		lonVar = dataFile.getVar("x");
		if(lonVar.isNull()) return NC_ERR;
		lonVar.getVar(lons);
		latVar.getVar(lats);


	   	// Get the pressure and temperature variables and read data one time step at a time
	   
		NcVar tempVar,RHVar,WSVar,SRVar,PrcpVar;
		tempVar = dataFile.getVar("2t");
		if(tempVar.isNull()) return NC_ERR;
		tempVar.getVar(temp_in);
		RHVar = dataFile.getVar("2r");
		if(RHVar.isNull()) return NC_ERR;
		RHVar.getVar(rh_in);
		WSVar = dataFile.getVar("10si");
		if(WSVar.isNull()) return NC_ERR;
		WSVar.getVar(ws_in);
		SRVar = dataFile.getVar("solar");
		if(SRVar.isNull()) return NC_ERR;
		SRVar.getVar(sr_in);
		PrcpVar = dataFile.getVar("tp");
		if(PrcpVar.isNull()) return NC_ERR;
		PrcpVar.getVar(prcp_in);

	
		//for(int x=0;x < NLON;x++){
		//for(int y=0;y < NLAT;y++){
		
		
		NFDR2016Calc nfdrs2016(45.0, 'Y', 1,12,0,0,0);
		nfdrs2016.iSetFuelMoistures(3,4,5,6,60,90,25);
		NFDR2016CalcState nfdrs2016state(&nfdrs2016);
		cout << sizeof(nfdrs2016state) << endl;
		nfdrs2016state.SaveState("Test.State");
		//ofstream stateout("CurrState.bin",ios::binary);
		bool InitFromFile = false;
		
		//#pragma omp parallel	
		for(long a=0; a < aSize; a++){
			if(a % 1000 == 0) cout << a << endl;
			NFDR2016Calc nfdrs2016loop(45.0, 'Y', 1,12,0,0,0);
			nfdrs2016loop.iSetFuelMoistures(3,4,5,6,60,90,25);
			
			nfdrs2016loop.iSetFuelMoistures(3,4,5,6,60,90,25);
			float MyLat = urma_topo[a][LAT];
			float MyAnnPrcp = urma_topo[a][ANNPRCP] * 0.0393701;  // Average annual precip (inches)
			//NFDR2016CalcState nfdrs2016state(&nfdrs2016);
			//vnfdrs2016state.push_back(nfdrs2016state);
			// First compute the 24 hour min, max and sums for temp/rh and precip
			float ftmax = -150; float ftmin =200;
			float frhmax = 0; float frhmin = 100;
			float fprcp24 = 0.0;

			for(int i =0;i < NREC;i++){
				temp_in[i][a] = (temp_in[i][a] - 273.15) * 9/5 + 32.;
				
				if(rh_in[i][a] > 99.999) rh_in[i][a] = 99.999;
				if(temp_in[i][a] > ftmax) ftmax = temp_in[i][a];
				if(temp_in[i][a] < ftmin) ftmin = temp_in[i][a];
				if(rh_in[i][a] > frhmax) frhmax = rh_in[i][a];
				if(rh_in[i][a] < frhmin) frhmin = rh_in[i][a];
				fprcp24 += prcp_in[i][a];

			}
			tmax[a] = ftmax;
			tmin[a] = ftmin;
			rhmax[a] = frhmax;
			rhmin[a] = frhmin;
			prcp24[a] = fprcp24;
			// Run the NFDRS calculations on the grid
			// 1. Read an initfile or start from scratch
			// 2. Load the state variables
			if(InitFromFile == true)
			{
				// Read the init state from the file
				// Load the state
			}
			else
			{
				nfdrs2016loop.Init(MyLat,'Y',1,MyAnnPrcp,1,1,0);
			}
			
			
			// 3. Calculate the NFDRS indices over a loop
			for (int i =0;i < NREC;i++)
			{
				//nfdrs2016loop.Update(yr,mon,day,i,jday, temp_in[i][a], 
					//tmin[a], tmax[a], rh_in[i][a], rhmin[a], 
					//prcp_in[i][a], prcp24[a], sr_in[i][a], ws_in[i][a], false, 13);
			}
			// 3a. Store FM and other values as we go
			// 4. Write the output file
			// 5. Write the carryover file
			//nfdrs2016state.SaveState(stateout);

		}
		ofstream fout("Test.flt",ios::binary);
		fout.write((char *) &tmax,sizeof(tmax));
		fout.close();
		//}
     
   return 0;

   }
   catch(NcException& e)
   {
      e.what();
      cout<<"FAILURE**************************"<<endl;
      return NC_ERR;
   }
  
}
