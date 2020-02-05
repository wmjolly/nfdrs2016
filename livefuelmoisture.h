#ifndef LIVEFUELMOISTURE_H
#define LIVEFUELMOISTURE_H
#include <math.h>
#include <vector>
#include <list>
#include "LFMCalcState.h"
using namespace std;
#define NOVALUE -9999.9
#define RADPERDAY 0.017214
#define RADPERDEG 0.01745329
#define MINDECL -0.4092797
#define SECPERRAD 13750.9871
#define DAYSOFF 10.25

//-----------------------------------------------------------------------------
/*! \page citations References

    \section jolly2005 Jolly, William M, Running, SR, and Rama Nemani. 2005.
    A generalized, bioclimatic index to predict foliar phenology in response to climate
    <i>Glob. Chan. Biol.</i> 11<b>(4)</b>: 619-632.
 */

//------------------------------------------------------------------------------
/*! \class LiveFuelMoisture LiveFuelMoisture.h
    \brief Determines moisture content of live herbaceous and woody fuels
    from ambient air temperature, relative humidity, and time of year.

    DeadFuelMoisture is an implementation of Nelson's dead fuel moisture model
    \ref jolly2005. This section describes LiveFuelMoisture class usage only;

 */
class LiveFuelMoisture
{
    public:
        LiveFuelMoisture();

        LiveFuelMoisture(double Lat,bool IsHerb, bool IsAnnual);
        void Initialize(double Lat,bool IsHerb, bool IsAnnual);
        void SetLimits(double,double,double,double,double,double);
       // void Update(double minRH, double MaxTempF, double MinTempF,double Jday, double Precip24, time_t thisTime);
		void Update(double TempF, double MaxTempF, double MinTempF, double RH, double minRH, int Jday, double Precip24, time_t thisTime);
        void SetMAPeriod(unsigned int MAPeriod);
        void SetLFMParameters(double MaxGSI,double GreenupThreshold,double MinLFMVal, double MaxLFMVal);
        void GetLFMParameters(double * MaxGSI,double * GreenupThreshold ,double * MinLFMVal, double * MaxLFMVal);

        double GetMoisture(bool SnowDay = false);
        double CalcRunningAvgGSI();
        double CalcRunningAvgHerbFM(bool SnowDay = false);
        void ResetHerbState();
        double CalcRunningAvgWoodyFM(bool SnowDay = false);
        double CalcGSI(double minRH, double maxTempF, double minTempF, double lat, int doy);
        double CalcGSI_VPDAvg(double RH, double TempF, double maxTempF, double minTempF, double lat, int doy);
        double GetTminInd(double Tmin);
        double GetVPDInd(double VPD);
        double GetDaylInd(double Dayl);
        double CalcDayl(double lat,int yday);
        double CalcVPDavg(double TempDewF, double TempAvgF);
        double CalcVPD(double RH, double TempF);
        double CalcDPT(double tempF, int RH);
        double CalcVP(double tempF);

		double GetMaxGSI();
		double GetGreenupThreshold();
		double GetMinLFMVal();
		double GetMaxLFMVal();

		void SetUseVPDAvg(bool set);
		bool GetUseVPDAvg();
		bool GetIsAnnual();
		LFMCalcState GetState();
		bool SetState(LFMCalcState state);
		//double GetXDaysPrecipitation(int nDays);
		//static const int nPrecipQueueDays = 90;
    private:
		bool m_UseVPDAvg;
        bool m_IsHerb;
        bool m_IsAnnual;
        int m_LFIdaysAvg;
        double m_Lat;
        //vector<double> iGSI;
		list<double> qGSI;
        double m_TminMin;
        double m_TminMax;
        double m_VPDMin;
        double m_VPDMax;
        double m_DaylenMin;
        double m_DaylenMax;

        //added for live fuel moistues
        double m_MaxGSI;
        double m_GreenupThreshold;
        double m_MaxLFMVal;
        double m_MinLFMVal;
        double m_Slope;
        double m_Intercept;
        //double m_HerbMaxGSI;
       // double m_HerbGreenup;
       // double m_HerbMax;
        //double m_HerbMin;
        //double m_WoodyMaxGSI;
        //double m_WoodyGreenup;
        //double m_WoodyMax;
       // double m_WoodyMin;
        //double m_HerbSlope;
        //double m_HerbIntercept;
        //double m_WoodySlope;
       // double m_WoodyIntercept;
        //bool herbAnnual;
        bool hasGreenedUpThisYear;
        bool hasExceeded120ThisYear;
        bool canIncreaseHerb;
        double lastHerbFM;
		//queue<double> qPrecip;
		time_t lastUpdateTime;
};

#endif // LIVEFUELMOISTURE_H
