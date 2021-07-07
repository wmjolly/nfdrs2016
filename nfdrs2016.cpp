/**
\mainpage The United States National Fire Danger Rating System, version 2016 (NFDRS2016)

*/
// Note: Moved IC calculation into CalcIndex 15 Apr 2016

// Standard include files
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <algorithm>
#include "nfdrs2016.h"
#include "station.h"
#include <time.h>
#include "utctime.h"
#include "NFDR2016CalcState.h"

#define SNOWDAYS_TRIGGER 5
const double NORECORD = -999.0;

/*#ifdef _DEBUG
#include <windows.h>
#endif*/
// Converts degrees to radians.
#define degreesToRadians(angleDegrees) (angleDegrees * 3.14159 / 180.0)

// Converts radians to degrees.
#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

using namespace std;
using namespace utctime;

//#define DEBUG
#undef DEBUG

/*! \def USE_CDB_METHOD
    \brief Disables Nelson's rainstorm sections,
    making the storm transition value unnecessary.
 */
#define USE_CDB_METHOD
//#undef USE_CDB_METHOD

NFDR2016Calc::NFDR2016Calc()
{
	CTA = 0.0459137;
	NFDRSVersion = 16;                                          // NFDRS Model Version
	num_updates = 0;                                            // Counter for number of fuel moisture update cycles
	CummPrecip = 0.0;                                           // Place to store cummulative precip
	YKBDI = KBDI = KBDIThreshold = 100;                                                // Starting KBDI
	MCHERB = 30.0;
	MCWOOD = 60.0;
	PrevYear = -999;
	YesterdayJDay = -999;
	StartKBDI = 100;
	mxdHumid = false;
	FuelTemperature = -999;
	m_GSI = 0.0;
	nConsectiveSnowDays = 0;
	//lastUpdateTime = 0;
    Init(45, 'Y', 1, 0.0, true, true, true, 100, 13);
}


// Constructor for the NFDRS 2016 Calculator Class
// inLat: Latitude (degrees)
// FuelModel: Fuel Model (char)
//
NFDR2016Calc::NFDR2016Calc(double inLat, char FuelModel,int inSlopeClass, double inAvgAnnPrecip,bool LT,bool Cure, bool IsAnnual)
{
	StartKBDI = 100;
	Init(inLat, FuelModel, inSlopeClass, inAvgAnnPrecip, LT, Cure, IsAnnual, 100);// , 1.0, 0.5);
   
	//the below commented out as now done in Init
  /*  
	CTA = 0.0459137;
    NFDRSVersion = 16;                                          // NFDRS Model Version
    Lat = inLat;                                                // Latitude (degrees)
    num_updates = 0;                                            // Counter for number of fuel moisture update cycles
    AvgPrecip = inAvgAnnPrecip;                                    // Average Annual Precip
    CummPrecip = 0.0;                                           // Place to store cummulative precip
    // Initialize the live fuel moisture models
    HerbFM.Initialize(Lat,true,true);                           // Live Herb FM model init
    WoodyFM.Initialize(Lat,false,false);                        // Live Woody FM model init
    // Initialize the dead fuel moisture models
    OneHourFM.initializeParameters(0.2,"One Hour");             // 1hr Dead FM model init
    TenHourFM.initializeParameters(0.64,"Ten Hour");            // 10hr Dead FM model init
    HundredHourFM.initializeParameters(2.0,"Hundred Hour");     // 100hr Dead FM model init
    ThousandHourFM.initializeParameters(6.4,"Thousand Hour");   // 1000hr Dead FM model init
    iSetFuelModel(FuelModel);                                   // Set the Fuel model
    UseLoadTransfer = LT;                                       // Use Load Transfer? (bool)
    UseCuring = Cure;                                           // Use Curing? (bool)
    YKBDI = KBDI = 100;                                                // Starting KBDI
    SlopeClass = inSlopeClass;

    MCHERB = 30.0;
    MCWOOD = 60.0;*/

}

NFDR2016Calc::~NFDR2016Calc()
{

}

void NFDR2016Calc::Init(double inLat, char iFuelModel, int inSlopeClass, double inAvgAnnPrecip, bool LT, bool Cure, bool isAnnual, int kbdiThreshold, int RegObsHour/* = 13*/)//, double fMaxGSI , double fGSIGreenupThreshold )
{
	CTA = 0.0459137;
	NFDRSVersion = 16;                                          // NFDRS Model Version
	Lat = inLat;                                                // Latitude (degrees)
	num_updates = 0;                                            // Counter for number of fuel moisture update cycles
	AvgPrecip = inAvgAnnPrecip;                                    // Average Annual Precip
	CummPrecip = 0.0;                                           // Place to store cummulative precip
	KBDIThreshold = kbdiThreshold;															// Initialize the live fuel moisture models
	GsiFM.Initialize(Lat, true, isAnnual);
	HerbFM.Initialize(Lat, true, isAnnual);                           // Live Herb FM model init
	WoodyFM.Initialize(Lat, false, false);                        // Live Woody FM model init
	GsiFM.SetLFMParameters(GsiFM.GetMaxGSI(), GsiFM.GetGreenupThreshold(), GsiFM.GetMinLFMVal(), GsiFM.GetMaxLFMVal());
	HerbFM.SetLFMParameters(HerbFM.GetMaxGSI(), HerbFM.GetGreenupThreshold(), HerbFM.GetMinLFMVal(), HerbFM.GetMaxLFMVal());
	WoodyFM.SetLFMParameters(WoodyFM.GetMaxGSI(), WoodyFM.GetGreenupThreshold(), WoodyFM.GetMinLFMVal(), WoodyFM.GetMaxLFMVal());

	// Initialize the dead fuel moisture models
	OneHourFM.initializeParameters(0.2, "One Hour");             // 1hr Dead FM model init
	TenHourFM.initializeParameters(0.64, "Ten Hour");            // 10hr Dead FM model init
	HundredHourFM.initializeParameters(2.0, "Hundred Hour");     // 100hr Dead FM model init
	ThousandHourFM.initializeParameters(3.81, "Thousand Hour"); 
	
	
	OneHourFM.setAdsorptionRate(0.462252733);
	TenHourFM.setAdsorptionRate(0.079548303);
	HundredHourFM.setAdsorptionRate(0.06);
	ThousandHourFM.setAdsorptionRate(0.06);
		
	//these two lines were not done in the original constructor. 
	//sets maximum stick moisture limit. Not really relevant for 100 and 1000 hour moistures.
	OneHourFM.setMaximumLocalMoisture(0.35);
	TenHourFM.setMaximumLocalMoisture(0.35);
	HundredHourFM.setMaximumLocalMoisture(0.35);
	ThousandHourFM.setMaximumLocalMoisture(0.35);

	//iSetFuelModel(FuelModel);                                   // Set the Fuel model
	UseLoadTransfer = LT;                                       // Use Load Transfer? (bool)
	UseCuring = Cure;                                           // Use Curing? (bool)
	YKBDI = KBDI = StartKBDI;// = 100;                                                // Starting KBDI
	SlopeClass = inSlopeClass;

    MCHERB = HerbFM.GetMinLFMVal();// 30.0;
    MCWOOD = WoodyFM.GetMinLFMVal();// 60.0;

	PrevYear = -999;
	YesterdayJDay = -999;

	mxdHumid = false;
	FuelTemperature = -999;
	m_GSI = 0.0;
	nConsectiveSnowDays = 0;
	//lastUpdateTime = 0;
	iSetFuelModel(iFuelModel);
	//fill the 24 hours pcparray with 0's
//	for (int h = 0; h < 24; h++)
//		prcp.push_back(0.0);
    m_regObsHour = RegObsHour;
    for (int h = 0; h < nHoursPerDay; h++)
    {
        qHourlyTemp.push_back(NORECORD);
        qHourlyRH.push_back(NORECORD);
        qHourlyPrecip.push_back(NORECORD);
    }
    utcHourDiff = utctime::get_hour_diff();
}

void NFDR2016Calc::SetSCMax(int maxSC)
{
	SCM = maxSC;
}

int NFDR2016Calc::GetSCMax()
{
	return SCM;
}

void NFDR2016Calc::SetMxdHumid(bool isHumid)
{
	mxdHumid = isHumid;
	if (isHumid)
	{
		MXD = 40;
	}
	else
	{
		switch (FuelModel)
		{
		case 'v':
		case 'V':
		case 'w':
		case 'W':
			MXD = 15;
			break;
		case 'x':
		case 'X':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
			MXD = 25;
			break;
		}
	}
}

bool NFDR2016Calc::GetMxdHumid()
{
	return mxdHumid;
}


double NFDR2016Calc::GetFuelTemperature()
{
	return FuelTemperature;
}

/* begin iSetFuelMoistures	****************************************************
*
*
***************************************************************************/
int NFDR2016Calc::iSetFuelMoistures (double fMC1, double fMC10,double fMC100, double fMC1000, double fMCWood, double fMCHerb, double fuelTempC)
{
   MC1 = fMC1;
   MC10 = fMC10;
   MC100 = fMC100;
   MC1000 = fMC1000;
   MCWOOD = fMCWood;
   MCHERB = fMCHerb;
   FuelTemperature = fuelTempC;
   return EXIT_SUCCESS;
}

bool IsLeapYear(int year)
{
    bool isLeap = false;
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        isLeap = true;
    return isLeap;
}

int CalcJulianDay(int year, int month, int day)
{
    const int DaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int dayOfYear = 0;
    for (int m = 0; m < month; m++)
        dayOfYear += DaysOfMonth[m];
    dayOfYear += day;
    if (IsLeapYear(year) && month >= 2)
        dayOfYear++;
    return dayOfYear;
}

//void NFDR2016Calc::Update(int Year, int Month, int Day, int Hour, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double PPTAcc, double PPTAmt, double SolarRad, double WS, bool SnowDay, int RegObsHr)
void NFDR2016Calc::Update(int Year, int Month, int Day, int Hour, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double MinRH, double PPTAmt, double pcp24, double SolarRad, double WS, bool SnowDay, int RegObsHr)
{
    int tJulian = CalcJulianDay(Year, Month - 1, Day);
    if (Julian != tJulian)
        printf("Julain day mismatch for Year = %d, Month = %d, Day = %d, passed Julian = %d, calced Julian = %d\n",
            Year, Month, Day, Julian, tJulian);

	if (PrevYear > 0 && YesterdayJDay > 0)
	{
		if (Year < PrevYear || (Year > (PrevYear + 1)) || (365 * (Year - PrevYear) + Julian - YesterdayJDay > 30))
		{
			//reinit
			Init(Lat, FuelModel, SlopeClass, AvgPrecip, UseLoadTransfer, UseCuring, HerbFM.GetIsAnnual(), KBDIThreshold);// , HerbFM.GetMaxGSI(), HerbFM.GetGreenupThreshold());
		}
	}

	//Herb and 1-Hour reset every year.... Verify we want to do this
    if (Julian < YesterdayJDay || YesterdayJDay < 0) {
        HerbFM.ResetHerbState();
      //  OneHourFM.initializeStick();
    }
	if (PrevYear != Year)
	{		
		PrevYear = Year;
	}
	
    if (Hour == RegObsHr || num_updates == 0)
    {
        if (SnowDay) { SnowCovered = true; }
        else { SnowCovered = false; }
    }
	if (SnowDay) { SnowCovered = true; }
	else { SnowCovered = false; }
    double temp = (Temp - 32.0) * 5.0 / 9.0, rh = RH / 100.0, /*accprcp = PPTAcc * 2.54, */sr = SolarRad, pptamnt = PPTAmt * 2.54;
	if (num_updates == 0)
	{

		OneHourFM.setMoisture(0.2);
		TenHourFM.setMoisture(0.2);
		HundredHourFM.setMoisture(0.2);
		ThousandHourFM.setMoisture(0.2);
	}

    double MyMC1 = 0, MyMC10 = 0, MyMC100 = 0, MyMC1000 = 0;
   // double PPTAmt24 = 0.0;
	double neltemp = floor(temp * 100 + 0.5) / 100;
	double nelrh = rh, nelsr = sr, nelppt = pptamnt;

    // Update dead fuel moistures for each time period
    //cout << wxRec.m_Year << " " << wxRec.m_Month << " " << wxRec.m_Day << " " << wxRec.m_Hour << endl;
    //if (Temp < 28 && PPTAmt > 0) SnowDay = 1;
    // Need to set the date / time of the sticks to one hour before the first observation time
    if (SnowCovered)
    {
        neltemp = 0.;
        nelrh = 0.999;
        nelsr = 0.;
        //nelppt = pptamnt;  // This is the place to deal with snow melt.
		nelppt = 0.;
		
    }
#pragma omp parallel sections num_threads(4)
    {

#pragma omp section
        {

            OneHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp,nelrh,nelsr,nelppt,0.02179999999,true);
            MC1 = MyMC1 = OneHourFM.medianRadialMoisture() * 100;
			//MC1 = MyMC1 = OneHourFM.meanWtdMoisture() * 100;
        }
#pragma omp section
        {

            TenHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr,nelppt,0.02179999999, true);
            MC10 = MyMC10 = TenHourFM.medianRadialMoisture() * 100;
			//MC10 = MyMC10 = TenHourFM.meanWtdMoisture() * 100;
        }
#pragma omp section
        {

            HundredHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC100 = MyMC100 = HundredHourFM.medianRadialMoisture() * 100;
			//MC100 = MyMC100 = HundredHourFM.meanWtdMoisture() * 100;

        }
#pragma omp section
        {

            ThousandHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC1000 = MyMC1000 = ThousandHourFM.medianRadialMoisture() * 100;
			//MC1000 = MyMC1000 = ThousandHourFM.meanWtdMoisture() * 100;

        }
    }

	//moved here so we have hourly fueltemp to save to DB
	FuelTemperature = OneHourFM.surfaceTemperature();
    //update 24 hour deques
    UTCTime thisUtcTime(Year, Month, Day, Hour, 0, 0);
    time_t thisDiff = thisUtcTime - lastUtcUpdateTime;
    time_t hoursDiff = thisDiff / utcHourDiff;
    if (hoursDiff > 1)//gap, insert NODATA
    {
        for (int h = 1; h < hoursDiff; h++)
        {
            qHourlyPrecip.push_back(NORECORD);
            qHourlyTemp.push_back(NORECORD);
            qHourlyRH.push_back(NORECORD);
        }
    }
    qHourlyPrecip.push_back(PPTAmt);
    qHourlyTemp.push_back(Temp);
    qHourlyRH.push_back(RH);

   // Update live fuel moisture once per day
    if (Hour == RegObsHr)// || num_updates==0)
    {

        //LFM requires temperatures in F and RH between 1 and 100
        // Note: Need to set up a common units structure for all calculations
        // Maybe just need to change Nelson code to accept above temp / rh scales
       // int secs = difftime(thisUtcTime.timestamp(), lastDailyUpdateTime.timestamp());
        int secs = thisUtcTime.timestamp() - lastDailyUpdateTime.timestamp();
  		if (SnowDay)
			nConsectiveSnowDays++;
		else
			nConsectiveSnowDays = 0;

		//update the precip deque before updating GSI!!!!
		int days = secs / 86400;//86400 seconds per day
		if (days > 1)//gap, deal with it by inserting zeroes
		{
			int pDays = min(days - 1, nPrecipQueueDays);
			for (int p = 0; p < pDays; p++)
				qPrecip.push_back(0.0);
		}
		qPrecip.push_back(pcp24);
		while (qPrecip.size() > nPrecipQueueDays)
			qPrecip.pop_front();

		GsiFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(GsiFM.GetNumPrecipDays()), thisUtcTime.timestamp());
		HerbFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(HerbFM.GetNumPrecipDays()), thisUtcTime.timestamp());
		WoodyFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(WoodyFM.GetNumPrecipDays()), thisUtcTime.timestamp());

		m_GSI = GsiFM.CalcRunningAvgGSI();
        MCHERB = HerbFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
        MCWOOD = WoodyFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
        // Calculate the daily KBDI that is used for the drought fuel loading
        KBDI = iCalcKBDI(pcp24, (int) MaxTemp, CummPrecip, YKBDI, AvgPrecip);
		YKBDI = KBDI;


        lastDailyUpdateTime = thisUtcTime;

    }
	iSetFuelMoistures(MC1, MC10, MC100, MC1000, MCWOOD, MCHERB, FuelTemperature);
	// Calculate the indices

	double fSC, fERC, fBI, fIC;
	iCalcIndexes((int)WS, SlopeClass, &fSC, &fERC, &fBI, &fIC);
	num_updates++;
    YesterdayJDay = Julian;
    lastUtcUpdateTime = thisUtcTime;
}

void NFDR2016Calc::Update(int Year, int Month, int Day, int Hour, double Temp, double RH, double PPTAmt, double SolarRad, double WS, bool SnowDay)
{
    int Julian = CalcJulianDay(Year, Month - 1, Day);
    if (PrevYear > 0 && YesterdayJDay > 0)
    {
        if (Year < PrevYear || (Year > (PrevYear + 1)) || (365 * (Year - PrevYear) + Julian - YesterdayJDay > 30))
        {
            //reinit
            Init(Lat, FuelModel, SlopeClass, AvgPrecip, UseLoadTransfer, UseCuring, HerbFM.GetIsAnnual(), KBDIThreshold);// , HerbFM.GetMaxGSI(), HerbFM.GetGreenupThreshold());
        }
    }

    //Herb and 1-Hour reset every year.... Verify we want to do this
    if (Julian < YesterdayJDay || YesterdayJDay < 0) {
        HerbFM.ResetHerbState();
        //  OneHourFM.initializeStick();
    }
    if (PrevYear != Year)
    {
        PrevYear = Year;
    }

    if (Hour == m_regObsHour || num_updates == 0)
    {
        if (SnowDay) { SnowCovered = true; }
        else { SnowCovered = false; }
    }
    if (SnowDay) { SnowCovered = true; }
    else { SnowCovered = false; }
    double temp = (Temp - 32.0) * 5.0 / 9.0, rh = RH / 100.0, /*accprcp = PPTAcc * 2.54, */sr = SolarRad, pptamnt = PPTAmt * 2.54;
    if (num_updates == 0)
    {

        OneHourFM.setMoisture(0.2);
        TenHourFM.setMoisture(0.2);
        HundredHourFM.setMoisture(0.2);
        ThousandHourFM.setMoisture(0.2);
    }

    double MyMC1 = 0, MyMC10 = 0, MyMC100 = 0, MyMC1000 = 0;
    // double PPTAmt24 = 0.0;
    double neltemp = floor(temp * 100 + 0.5) / 100;
    double nelrh = rh, nelsr = sr, nelppt = pptamnt;

    // Update dead fuel moistures for each time period
    //cout << wxRec.m_Year << " " << wxRec.m_Month << " " << wxRec.m_Day << " " << wxRec.m_Hour << endl;
    //if (Temp < 28 && PPTAmt > 0) SnowDay = 1;
    // Need to set the date / time of the sticks to one hour before the first observation time
    if (SnowCovered)
    {
        neltemp = 0.;
        nelrh = 0.999;
        nelsr = 0.;
        //nelppt = pptamnt;  // This is the place to deal with snow melt.
        nelppt = 0.;

    }
#pragma omp parallel sections num_threads(4)
    {

#pragma omp section
        {

            OneHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC1 = MyMC1 = OneHourFM.medianRadialMoisture() * 100;
            //MC1 = MyMC1 = OneHourFM.meanWtdMoisture() * 100;
        }
#pragma omp section
        {

            TenHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC10 = MyMC10 = TenHourFM.medianRadialMoisture() * 100;
            //MC10 = MyMC10 = TenHourFM.meanWtdMoisture() * 100;
        }
#pragma omp section
        {

            HundredHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC100 = MyMC100 = HundredHourFM.medianRadialMoisture() * 100;
            //MC100 = MyMC100 = HundredHourFM.meanWtdMoisture() * 100;

        }
#pragma omp section
        {

            ThousandHourFM.update(Year, Month, Day, Hour, 0, 0, neltemp, nelrh, nelsr, nelppt, 0.02179999999, true);
            MC1000 = MyMC1000 = ThousandHourFM.medianRadialMoisture() * 100;
            //MC1000 = MyMC1000 = ThousandHourFM.meanWtdMoisture() * 100;

        }
    }

    //moved here so we have hourly fueltemp to save to DB
    FuelTemperature = OneHourFM.surfaceTemperature();

    //update 24 hour deques
    UTCTime thisUtcTime(Year, Month, Day, Hour, 0, 0);   
    time_t thisDiff = thisUtcTime - lastUtcUpdateTime;
    time_t hoursDiff = thisDiff / utcHourDiff;
    if (hoursDiff > 1)//gap, insert NODATA
    {
        for (int h = 1; h < hoursDiff; h++)
        {
            qHourlyPrecip.push_back(NORECORD);
            qHourlyTemp.push_back(NORECORD);
            qHourlyRH.push_back(NORECORD);
        }
    }
    qHourlyPrecip.push_back(PPTAmt);
    qHourlyTemp.push_back(Temp);
    qHourlyRH.push_back(RH);
    //trim the deques
    while (qHourlyTemp.size() > 24)
        qHourlyTemp.pop_front();
    while (qHourlyRH.size() > 24)
        qHourlyRH.pop_front();
    while (qHourlyPrecip.size() > 24)
        qHourlyPrecip.pop_front();
    //deques OK, now figure Min/Max's and 24 hour pcp
    double MinRH = NORECORD, MinTemp = NORECORD, MaxTemp = NORECORD, pcp24 = 0.0;
    for (std::deque<double>::iterator it = qHourlyTemp.begin(); it != qHourlyTemp.end(); ++it)
    {
        if ((*it) != NORECORD)
        {
            if (MaxTemp == NORECORD)
                MaxTemp = *it;
            else
                MaxTemp = max(MaxTemp, *it);
            if (MinTemp == NORECORD)
                MinTemp = *it;
            else
                MinTemp = min(MinTemp, *it);
        }
    }
    for (std::deque<double>::iterator it = qHourlyRH.begin(); it != qHourlyRH.end(); ++it)
    {
        if ((*it) != NORECORD)
        {
            if (MinRH == NORECORD)
                MinRH = *it;
            else
                MinRH = min(MinRH, *it);
        }
    }
    for (std::deque<double>::iterator it = qHourlyPrecip.begin(); it != qHourlyPrecip.end(); ++it)
    {
        if (*it != NORECORD)
            pcp24 += *it;
    }
    // Update live fuel moisture once per day
    if (Hour == m_regObsHour)// || num_updates==0)
    {
        int secs = thisUtcTime.timestamp() - lastDailyUpdateTime.timestamp();
        //LFM requires temperatures in F and RH between 1 and 100
        // Note: Need to set up a common units structure for all calculations
        // Maybe just need to change Nelson code to accept above temp / rh scales
         if (SnowDay)
            nConsectiveSnowDays++;
        else
            nConsectiveSnowDays = 0;
        
        //update the precip deque before updating GSI!!!!
        int days = secs / 86400;//86400 seconds per day
        if (days > 1)//gap, deal with it by inserting zeroes
        {
            int pDays = min(days - 1, nPrecipQueueDays);
            for (int p = 0; p < pDays; p++)
                qPrecip.push_back(0.0);
        }
        qPrecip.push_back(pcp24);
        while (qPrecip.size() > nPrecipQueueDays)
            qPrecip.pop_front();

        GsiFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(GsiFM.GetNumPrecipDays()), thisUtcTime.timestamp());
        HerbFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(HerbFM.GetNumPrecipDays()), thisUtcTime.timestamp());
        WoodyFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(WoodyFM.GetNumPrecipDays()), thisUtcTime.timestamp());

        m_GSI = GsiFM.CalcRunningAvgGSI();
        MCHERB = HerbFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
        MCWOOD = WoodyFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
        // Calculate the daily KBDI that is used for the drought fuel loading
        KBDI = iCalcKBDI(pcp24, (int)MaxTemp, CummPrecip, YKBDI, AvgPrecip);
        YKBDI = KBDI;

        lastDailyUpdateTime = thisUtcTime;
 
    }
    iSetFuelMoistures(MC1, MC10, MC100, MC1000, MCWOOD, MCHERB, FuelTemperature);
    // Calculate the indices

    double fSC, fERC, fBI, fIC;
    iCalcIndexes((int)WS, SlopeClass, &fSC, &fERC, &fBI, &fIC);
    num_updates++;
    YesterdayJDay = Julian;
    lastUtcUpdateTime = thisUtcTime;
}

void NFDR2016Calc::UpdateDaily(int Year, int Month, int Day, int Julian, double Temp, double MinTemp, 
	double MaxTemp, double RH, double MinRH, double pcp24, double WS,
	double fMC1, double fMC10, double fMC100, double fMC1000, double fuelTemp,bool SnowDay)
{
    if (PrevYear > 0 && YesterdayJDay > 0)
	{
		if (Year < PrevYear || (Year >(PrevYear + 1)) || (365 * (Year - PrevYear) + Julian - YesterdayJDay > 30))
		{
			//reinit
			Init(Lat, FuelModel, SlopeClass, AvgPrecip, UseLoadTransfer, UseCuring, HerbFM.GetIsAnnual(), KBDIThreshold);// , HerbFM.GetMaxGSI(), HerbFM.GetGreenupThreshold());
		}
	}

	//Herb resets every year.... Verify we want to do this
	if (Julian < YesterdayJDay || YesterdayJDay < 0) {
		HerbFM.ResetHerbState();
	}
	if (PrevYear != Year)
	{
		PrevYear = Year;
	}
	if (SnowDay)
		nConsectiveSnowDays++;
	else
		nConsectiveSnowDays = 0;

    UTCTime thisUtcTime(Year, Month, Day, m_regObsHour, 0, 0);
   // tm thisTM;
	/*thisTM.tm_year = Year - 1900;
	thisTM.tm_mon = Month;
	thisTM.tm_mday = Day;
	thisTM.tm_hour = 13;
	thisTM.tm_min = 0;
	thisTM.tm_sec = 0;
	thisTM.tm_isdst = 0;*/
	//time_t 
	//time_t thisTime = mktime(&thisTM);
   // int secs = difftime(thisTime, lastUpdateTime);
    int secs = thisUtcTime.timestamp() - lastDailyUpdateTime.timestamp(); //difftime(thisTime, lastUpdateTime);
    int days = secs / 86400;//86400 seconds per day

	//do the precip deque before updating GSI!
	if (days > 1)//gap, deal with it by inserting zeroes
	{
		int pDays = min(days - 1, nPrecipQueueDays);
		for (int p = 0; p < pDays; p++)
			qPrecip.push_back(0.0);
	}
	qPrecip.push_back(pcp24);
	while (qPrecip.size() > nPrecipQueueDays)
		qPrecip.pop_front();

	// Update live fuel moisture once per day
	GsiFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(GsiFM.GetNumPrecipDays()), thisUtcTime.timestamp());
	HerbFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(HerbFM.GetNumPrecipDays()), thisUtcTime.timestamp());
		WoodyFM.Update(Temp, MaxTemp, MinTemp, RH, MinRH, Julian, GetXDaysPrecipitation(WoodyFM.GetNumPrecipDays()), thisUtcTime.timestamp());

		m_GSI = GsiFM.CalcRunningAvgGSI();
		MCHERB = HerbFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
		MCWOOD = WoodyFM.GetMoisture(nConsectiveSnowDays >= SNOWDAYS_TRIGGER ? true : false);
		// Calculate the daily KBDI that is used for the drought fuel loading
		KBDI = iCalcKBDI(pcp24, (int)MaxTemp, CummPrecip, YKBDI, AvgPrecip);
		YKBDI = KBDI;

		iSetFuelMoistures(fMC1, fMC10, fMC100, fMC1000, MCWOOD, MCHERB, fuelTemp);
		// Calculate the indices

		double fSC, fERC, fBI, fIC;
		//FuelTemperature = fuelTemp;
		iCalcIndexes((int)WS, SlopeClass, &fSC, &fERC, &fBI, &fIC);


	num_updates++;
	YesterdayJDay = Julian;

	//lastUpdateTime = thisTime;
    lastUtcUpdateTime = lastUtcUpdateTime = thisUtcTime;
}

void NFDR2016Calc::Update(Wx wxRec)
{
    // "old" method of determining daily obs -   Type 'O', or 1300hr
    int RegObsHr=13;
    if (wxRec.m_ObsType == "O")
        RegObsHr = wxRec.m_Hour;

	//Changed 3/1/2017 SB to pass RHMin
	//Update(wxRec.m_Year, wxRec.m_Month, wxRec.m_Day, wxRec.m_Hour, wxRec.m_JDay, wxRec.m_Temp, wxRec.m_TmpMin, wxRec.m_TmpMax, wxRec.m_RH, wxRec.m_PPTACC, wxRec.m_PPTAMT24, wxRec.m_SolarRadiation, wxRec.m_WS, wxRec.m_SnowDay, 13);
	Update(wxRec.m_Year, wxRec.m_Month, wxRec.m_Day, wxRec.m_Hour, wxRec.m_JDay, wxRec.m_Temp, wxRec.m_TmpMin, wxRec.m_TmpMax, wxRec.m_RH, wxRec.m_RHMin, wxRec.m_PPTAMT, wxRec.m_PPTAMT24, wxRec.m_SolarRadiation, wxRec.m_WS, wxRec.m_SnowDay, 13);

}


void NFDR2016Calc::iSetFuelModel (char cFM)
{

    SG1 = 2000;
    SG10 = 109;
    SG100 = 30;
    SG1000 = 8;
    SGWOOD = 1500;
    SGHERB = 2000;
    HD = 8000;
    FuelModel = static_cast<char>(cFM);

    // Fuel Model V: Grass
    if (cFM == 'V' || cFM == 'v')
    {
        //cout << "Setting fuel model V" << endl;

        L1 = 0.1;
        L10 = 0;
        L100 = 0;
        L1000 = 0;
        LWOOD = 0;
        LHERB = 1.0;
        DEPTH = 1;
        MXD = 15;
        SCM = 108;
        LDROUGHT = 0; // From 1988, Fuel Model A
        WNDFC = 0.6;
    }
    // Fuel Model W: Grass-Shrub
    else if (cFM == 'W' || cFM == 'w')
    {
        //cout << "Setting fuel model W" << endl;

        L1 = 0.5;
        L10 = 0.5;
        L100 = 0;
        L1000 = 0;
        LWOOD = 1.0;
        LHERB = 0.6;
        DEPTH = 1.5;
        MXD = 15;
        SCM = 62;
        LDROUGHT = 1.0; // From 1988, Fuel Model T
        WNDFC = 0.4;
    }
    // Fuel Model X: Brush
    else if (cFM == 'X' || cFM == 'x')
    {
        //cout << "Setting fuel model X" << endl;

        L1 = 4.5;
        L10 = 2.45;
        L100 = 0;
        L1000 = 0;
        LWOOD = 7;
        LHERB = 1.55;
        DEPTH = 4.4;
        MXD = 25;
        SCM = 104;
        LDROUGHT = 2.5; // From 1988, Fuel Model F
        WNDFC = 0.4;
    }
    // Fuel Model Y: Timber
    else if (cFM == 'Y' || cFM == 'y')
    {
        //cout << "Setting fuel model Y" << endl;

        L1 = 2.5;       // Set 1 hour loading from Fuel Model G
        L10 = 2.2;
        L100 = 3.6;
        L1000 = 10.16;
        LWOOD = 0.0;
        LHERB = 0.0;
        DEPTH = 0.6;
        MXD = 25;
        SCM = 5;
        LDROUGHT = 5;   // From 1988, Fuel Model G
        WNDFC = 0.2;
		
    }
    // Fuel Model Z: Slash / Blowdown
    else if (cFM == 'Z' || cFM == 'z')
    {
        //cout << "Setting fuel model Z" << endl;

        L1 = 4.5;
        L10 = 4.25;
        L100 = 4;
        L1000 = 4;
        LWOOD = 0;
        LHERB = 0;
        DEPTH = 1;
        MXD = 25;
        SCM = 19;
        LDROUGHT = 7;   // From 1988, Fuel Model J
        WNDFC = 0.4;
    }
    else
    {
        L1 = 2.5;       // Set 1 hour loading from Fuel Model G
        L10 = 2.2;
        L100 = 3.6;
        L1000 = 10.16;
        LWOOD = 0.0;
        LHERB = 0.0;
        DEPTH = 0.6;
        MXD = 25;
        SCM = 30;
        LDROUGHT = 5;   // From 1988, Fuel Model G
        WNDFC = 0.4;
    }

}

// Calculates all Components and Indices for NFDRS2016
// iWS: Windspeed (mph)
// iSlopeCls: Slope Class (1-5 or actual in degrees)
// fSC: Spread Component (dim)
// fERC: Energy Release Component (dim)
// fBI: Burning Index (dim)
// fIC: Ignition Component (dim)
int NFDR2016Calc::iCalcIndexes (int iWS, int iSlopeCls,double* fSC,double* fERC, double* fBI, double *fIC, double fGSI, double fKBDI)
{

    double STD = .0555, STL = .0555;
    double RHOD = 32, RHOL = 32;
    double ETASD = 0.4173969, ETASL = 0.4173969;
    double WTOTD, WTOTL, W1N, W10N, W100N, WHERBN, WWOODN, WTOTLN;
    double RHOBED, RHOBAR, BETBAR;
    double HN1, HN10, HN100, HNHERB, HNWOOD, WRAT, MCLFE, MXL;
    double SA1, SA10, SA100,  SAWOOD, SAHERB, SADEAD, SALIVE;
    double F1, F10, F100, FHERB, FWOOD, FDEAD, FLIVE, WDEADN, WLIVEN;
    double SGBRD, SGBRL, SGBRT, BETOP, GMAMX, AD, GMAOP, ZETA;
    double DEDRT, LIVRT, ETAMD, ETAML, B, C, E, UFACT;
    double IR, PHIWND, PHISLP;
    double XF1, XF10, XF100, XFHERB, XFWOOD, HTSINK;
    double F1E, F10E, F100E, F1000E, FHERBE, FWOODE, FDEADE, FLIVEE, WDEDNE, WLIVNE;
    double SGBRDE, SGBRLE, SGBRTE, BETOPE, GMAMXE, ADE, GMAOPE;
    double DEDRTE, LIVRTE, ETAMDE, ETAMLE;
    double IRE, TAU;
    double fWNDFC, PackingRatio, fDEPTH;

    double fTemp = 0;
    int itempWoody = 0;

    if ((iSlopeCls < 1) || (iWS < 0) || (iSlopeCls > 5) || (DEPTH <= 0))
        return(0);

    /* Reset */
    W1 = L1 * CTA;
    W10 = L10 * CTA;
    W100 = L100 * CTA;
    W1000 = L1000 * CTA;
    WWOOD = LWOOD * CTA;
    WHERB = LHERB * CTA;
    WWOOD = LWOOD * CTA;
    WDROUGHT = LDROUGHT * CTA;
    //cout << W1 << " " << W10 << " " << W100 << " " << W1000 << " " << WHERB << " " << WWOOD << endl;
    fDEPTH = DEPTH;

    double tmpKBDI = KBDI;
    if(fKBDI >= 0 && fKBDI <= 800)
    {
        tmpKBDI = fKBDI;
    }

	if (tmpKBDI > KBDIThreshold )
    {

        WTOTD = W1 + W10 + W100;
        WTOTL = WHERB + WWOOD;
        WTOT = WTOTD + WTOTL;
        PackingRatio = WTOT / fDEPTH;
        if (PackingRatio == 0) PackingRatio = 1.0;
        WTOTD = WTOTD + W1000;
        DroughtUnit = WDROUGHT / (800.0 - KBDIThreshold);

        W1 = W1 + (W1 / WTOTD) * (tmpKBDI - KBDIThreshold) * DroughtUnit;
        W10 = W10 + (W10 / WTOTD) * (tmpKBDI - KBDIThreshold) * DroughtUnit;
        W100 = W100 + (W100 / WTOTD) * (tmpKBDI - KBDIThreshold) * DroughtUnit;
        W1000 = W1000 + (W1000 / WTOTD) * (tmpKBDI - KBDIThreshold) * DroughtUnit;
        WTOT = W1 + W10 + W100 + W1000 + WTOTL;
        fDEPTH = (WTOT - W1000) / PackingRatio;
    }

    // Herbaceous Load Transfers
	//fTemp = Cure(MCHERB, fGSI, HerbFM.GetGreenupThreshold(), HerbFM.GetMaxGSI());
	fTemp = Cure(fGSI, HerbFM.GetGreenupThreshold(), HerbFM.GetMaxGSI());
	
	WTOTD = W1P + W10 + W100 + W1000;					// Total Dead Fuel Loading
	WTOTL = WHERBP + WWOOD;								// Total Live Fuel Loading
	WTOT = WTOTD + WTOTL;								// Total Fuel Loading
	W1N = W1P * (1.0 - STD);							// Net 1hr Fuel Loading
	W10N = W10 * (1.0 - STD);							// Net 10hr Fuel Loading
	W100N = W100 * (1.0 - STD);							// Net 100hr Fuel Loading
	WHERBN = WHERBP * (1.0 - STL);						// Net Herbaceous Fuel Loading
	WWOODN = WWOOD * (1.0 - STL);						// Net Woody Fuel Loading
	WTOTLN = WTOTL * (1.0 - STL);						// Net Total Live Fuel Lodaing
	RHOBED = (WTOT - W1000) / fDEPTH;					// Bulk density of the fuel bed
	RHOBAR = ((WTOTL * RHOL) + (WTOTD * RHOD)) / WTOT;  // Weighted particle density of the fuel bed
	BETBAR = RHOBED / RHOBAR;							// Ratio of bulk density to particle density

    // If live net fuel loading is greater than 0, calculate the 
	// Live Fuel Moisture of Extinction
	if (WTOTLN > 0)
    {
        HN1 = W1N * exp(-138.0 / SG1);
        HN10 =W10N  * exp(-138.0 / SG10);
        HN100 = W100N * exp(-138.0 / SG100);
        if ((-500 / SGHERB) < -180.218)
        {HNHERB = 0;}
        else
        {HNHERB = WHERBN * exp(-500.0 / SGHERB);}

        if ((-500 / SGWOOD) < -180.218)
        {HNWOOD = 0;}
        else
        {HNWOOD = WWOODN * exp(-500.0 / SGWOOD);}

        if ((HNHERB + HNWOOD) == 0)
        {WRAT = 0;}
        else
        {WRAT = (HN1 + HN10 + HN100) / (HNHERB + HNWOOD);}
        MCLFE = ((MC1 * HN1) + (MC10 * HN10) + (MC100 * HN100)) / (HN1 + HN10 + HN100);
        MXL = (2.9 * WRAT * (1.0 - MCLFE / MXD) - 0.226) * 100;
    }
    else
    {MXL = 0;}

	if (MXL < MXD) MXL = MXD;
	SA1 = (W1P / RHOD) * SG1;           // Surface area of dead 1hr fuel
	SA10 = (W10 / RHOD) * SG10;         // Surface area of dead 10hr fuel
	SA100 = (W100 / RHOD) * SG100;       // Surface area of dead 100hr fuel
	SAHERB = (WHERBP / RHOL) * SGHERB;   // Surface area of live herbaceous fuel
	SAWOOD = (WWOOD / RHOL) * SGWOOD;    // Surface area of live woody fuel
	SADEAD = SA1 + SA10 + SA100;		// Surface area of dead fuel
	SALIVE = SAHERB + SAWOOD;			// Surface area of live fuel

	if (SADEAD <= 0) return(0);

	F1 = SA1 / SADEAD;      //Proportion of dead-fuel surface area in 1-hour class,used as a weighting factor for ROS calculation
	F10 = SA10 / SADEAD;    //Proportion of dead-fuel surface area in 10-hour class,used as a weighting factor for ROS calculation
	F100 = SA100 / SADEAD;  //Proportion of dead-fuel surface area in 100-hour class,used as a weighting factor for ROS calculation
    if (WTOTL <=0)
    {
       FHERB = 0;
       FWOOD = 0;
    }
    else
    {
       FHERB = SAHERB / SALIVE;
       FWOOD = SAWOOD / SALIVE;
    }
    FDEAD = SADEAD / (SADEAD + SALIVE);		// Fraction of Dead Fuel Surface area to total loading
    FLIVE = SALIVE / (SADEAD + SALIVE);		// Fraction of Live Fuel Surface area to total loading
    WDEADN = (F1 * W1N) + (F10 * W10N) + (F100 * W100N);	// Weighted deaf-fuel loading

    if (SGWOOD > 1200 && SGHERB > 1200)
    {WLIVEN = WTOTLN;}
    else
    {WLIVEN = (FWOOD * WWOODN) + (FHERB * WHERBN);}

	// Characteristic surface area-to-volume ratio of dead fuel, surface area weighted
	SGBRD = (F1 * SG1) + (F10 * SG10) + (F100 * SG100);

	// Characteristic surface area-to-volume ratio of live fuel, surface area weighted.
	SGBRL = (FHERB * SGHERB) + (FWOOD * SGWOOD);

	// Characteristic surface area-to-volume ratio of fuel bed, surface area weighted.
	SGBRT = (FDEAD * SGBRD) + (FLIVE * SGBRL);

	// Optimum packing ratio, surface area weighted
	BETOP = 3.348 * pow(SGBRT, -0.8189);

	// Weighted maximum reaction velocity of surface area
	GMAMX = pow(SGBRT, 1.5) / (495.0 + 0.0594 * pow(SGBRT, 1.5));
	AD = 133 * pow(SGBRT, -0.7913);
	// Weighted optimum reaction velocity of surface area
	GMAOP = GMAMX * pow((BETBAR / BETOP), AD) * exp(AD * (1.0 - (BETBAR / BETOP)));

    ZETA = exp((0.792 + 0.681 * pow(SGBRT, 0.5)) * (BETBAR + 0.1));
    ZETA = ZETA / (192.0 + 0.2595 * SGBRT);

    WTMCD = (F1 * MC1) + (F10 * MC10) + (F100 * MC100);
    WTMCL = (FHERB * MCHERB) + (FWOOD * MCWOOD);
    DEDRT = WTMCD / MXD;
    LIVRT = WTMCL / MXL;
    ETAMD = 1.0 - 2.59 * DEDRT + 5.11 * pow(DEDRT,2.0) - 3.52 * pow(DEDRT, 3.0);
    ETAML = 1.0 - 2.59 * LIVRT + 5.11 * pow(LIVRT,2.0) - 3.52 * pow(LIVRT, 3.0);
    if (ETAMD < 0) ETAMD = 0;
    if (ETAMD > 1) ETAMD = 1;
    if (ETAML < 0) ETAML = 0;
    if (ETAML > 1) ETAML = 1;

    B = 0.02526 * pow(SGBRT, 0.54);
    C = 7.47 * exp(-0.133 * pow(SGBRT,0.55));
    E = 0.715 * exp(-3.59 * pow(10.0, -4.0) * SGBRT);
    UFACT = C * pow(BETBAR / BETOP, -1 * E);

    /* HL = HD */
    IR = GMAOP * ((WDEADN * HD * ETASD * ETAMD) + (WLIVEN * HD * ETASL * ETAML));
    //cout << GMAOP << " " << WDEADN << " " << HD << " " << ETASD << " " << ETAMD << " " << WLIVEN << " " << ETASL <<" " << ETAML << endl;
    fWNDFC = WNDFC;

    if (88.0 * (double) (iWS) * fWNDFC > 0.9 * IR)
    {
        PHIWND = UFACT * pow(0.9 * IR, B);
    }
    else
    {
        PHIWND = UFACT * pow((double) (iWS) * 88.0 * fWNDFC, B);
    }

    // Actual slopes in degrees (>5) can now be input
    // Matches forumla used in WIMS developed by Larry Bradshaw (31 Aug 2016)
    double slpfct = 0.267;
    switch (iSlopeCls)
    {
    case 1:
      slpfct = 0.267;
      break;
    case 2:
      slpfct = 0.533;
      break;
    case 3:
      slpfct = 1.068;
      break;
    case 4:
      slpfct = 2.134;
      break;
    case 5:
      slpfct = 4.273;
      break;
    default:
        slpfct = 5.275 * (tan(degreesToRadians((double)iSlopeCls)));
        break;
    }
    //cout << slpfct << endl;
    PHISLP = slpfct * pow(BETBAR, -0.3);

    XF1 = F1 * exp(-138.0 / (double) (SG1)) * (250.0 + 11.16 * MC1);
    XF10 = F10 * exp(-138.0 / (double)(SG10)) * (250.0 + 11.16 * MC10);
    XF100 = F100 * exp(-138.0 / (double)(SG100)) * (250.0 + 11.16 * MC100);
    XFHERB = FHERB * exp(-138.0 / (double)(SGHERB)) * (250.0 + 11.16 * MCHERB);
    XFWOOD = FWOOD * exp(-138.0 / (double)(SGWOOD)) * (250.0 + 11.16 * MCWOOD);
    HTSINK = RHOBED * (FDEAD * (XF1 + XF10 + XF100) + FLIVE * (XFHERB + XFWOOD));
    // cout << HTSINK << " " << RHOBED << " " << FDEAD << " " << XF1 << " " << XF10 << " " << XF100 << " " << FLIVE << " " << XFHERB << " " << XFWOOD << endl;
    *fSC = IR * ZETA * (1.0 + PHISLP + PHIWND) / HTSINK;

    F1E = W1P / WTOTD;
    F10E = W10 / WTOTD;
    F100E = W100 / WTOTD;
    F1000E = W1000 / WTOTD;
    /* NOTE 4 */
    if (WTOTL <=0)
    {
        FHERBE = 0;
        FWOODE = 0;
    }
    else
    {
        FHERBE = WHERBP / WTOTL;
        FWOODE = WWOOD / WTOTL;
    }
    FDEADE = WTOTD / WTOT;
    FLIVEE = WTOTL / WTOT;
    WDEDNE = WTOTD * (1.0 - STD);
    WLIVNE = WTOTL * (1.0 - STL);
    SGBRDE = (F1E * SG1) + (F10E * SG10) + (F100E * SG100) + (F1000E * SG1000);
    SGBRLE = (FHERBE * SGHERB) + (FWOODE * SGWOOD);
    SGBRTE = (FDEADE * SGBRDE) + (FLIVEE * SGBRLE);
    BETOPE = 3.348 * pow(SGBRTE, -0.8189);
    GMAMXE = pow(SGBRTE, 1.5) / (495.0 + 0.0594 * pow(SGBRTE, 1.5));
    ADE = 133 * pow(SGBRTE, -0.7913);
    GMAOPE = GMAMXE * pow( (BETBAR/BETOPE), ADE) * exp(ADE * (1.0 - (BETBAR / BETOPE)));

    WTMCDE = (F1E * MC1) + (F10E * MC10) + (F100E * MC100) + (F1000E * MC1000);
    WTMCLE = (FHERBE * MCHERB) + (FWOODE * MCWOOD);
    DEDRTE = WTMCDE / MXD;
    LIVRTE = WTMCLE / MXL;
    ETAMDE = 1.0 - 2.0 * DEDRTE + 1.5 * pow(DEDRTE,2.0) - 0.5 * pow(DEDRTE, 3.0);
    ETAMLE = 1.0 - 2.0 * LIVRTE + 1.5 * pow(LIVRTE,2.0) - 0.5 * pow(LIVRTE, 3.0);
    if (ETAMDE < 0) ETAMDE = 0;
    if (ETAMDE > 1) ETAMDE = 1;
    if (ETAMLE < 0) ETAMLE = 0;
    if (ETAMLE > 1) ETAMLE = 1;

    IRE = (FDEADE * WDEDNE * HD * ETASD * ETAMDE);
    /* HL = HD */
    IRE = GMAOPE * (IRE + (FLIVEE * WLIVNE * (double) (HD) * ETASL * ETAMLE));
    TAU = 384.0 / SGBRT;
    *fERC = 0.04 * IRE * TAU;
    //cout << F1E  << " " << SG1 << " " << F10E << " " <<  SG10 << " " << F100E << " " <<SG100 << " " << F1000E << " " << SG1000 << endl;
    //cout << FDEADE << " " << SGBRDE << " " << FLIVEE << " " <<  SGBRLE << endl;
    //cout << IRE << " " << TAU << " " << GMAOPE << " " << pow(SGBRTE, 1.5) << " " << SGBRTE <<  endl;
    //cout << BETBAR << " " << BETOPE << endl;
    *fBI = (.301 * pow((*fSC * *fERC), 0.46)) * 10.0;
    ERC = *fERC;
    BI = *fBI;
    SC = *fSC;
    // Finally, calculate the Igntion Component
    double TMPPRM = 0.0, PNORM1 = 0.00232, PNORM2 = 0.99767;
    double QIGN = 0.0, CHI = 0.0, PI = 0.0, SCN = 0.0, PFI = 0.0;
	IC = 0.0;
    if (SCM <= 0) IC = 0;

    // Replace iTemp with the Nelson-derived fuel surface temperature
	TMPPRM = FuelTemperature;
	//TMPPRM = OneHourFM.meanWtdTemperature();
    QIGN = 144.5 - (0.266 * TMPPRM) - (0.00058 * TMPPRM * TMPPRM)
             - (0.01 * TMPPRM * MC1)
          + 18.54 * (1.0 - exp(-0.151 * MC1))
          + 6.4 * MC1;

    if (QIGN >= 344.0) IC = 0;

    CHI = (344.0 - QIGN) / 10.0;
    if ((pow(CHI, 3.66) * 0.000923 / 50) <= PNORM1) IC = 0;

    PI = ((pow(CHI, 3.66) * 0.000923 / 50) - PNORM1) * 100.0 / PNORM2;
    if (PI < 0) PI = 0;
    if (PI > 100) PI = 100;
    SCN = 100.0 * SC / SCM;
    if (SCN > 100.0) SCN = 100.0;
    PFI = pow(SCN, 0.5);
    IC = 0.10 * PI * PFI;

    if (SC < 0.00001) IC = 0;
    *fIC = IC;

    /* Reset Just To Be Safe */
    W1 = L1 * CTA;
    W10 = L10 * CTA;
    W100 = L100 * CTA;
    W1000 = L1000 * CTA;
    WWOOD = LWOOD * CTA;
    WDROUGHT = LDROUGHT * CTA;
    return (1);

}
/* begin iCalcKBDI **********************************************************
*
*
************************************************************************/
int NFDR2016Calc::iCalcKBDI (double fPrecipAmt, int iMaxTemp,
                double fCummPrecip, int iYKBDI, double fAvgPrecip)
{
    int net = 0, idq = 0;
    double pptnet = 0.00,xkbdi = 0.00,xtemp = 0.00;

    CummPrecip = fCummPrecip;
    KBDI = iYKBDI;

    if(fPrecipAmt == 0.0)
    {
        CummPrecip = 0;
    }
    else
    {
        if(CummPrecip > 0.20)
        {
            pptnet = fPrecipAmt;
            CummPrecip = CummPrecip + fPrecipAmt;

        }
        else
        {
            CummPrecip = CummPrecip + fPrecipAmt;
            if(CummPrecip > 0.20) pptnet = CummPrecip - 0.20;
        }
        net = (100.0 * pptnet) + 0.0005;
        net = KBDI - net;
        KBDI = (net > 0) ? net : 0;

}

  if(iMaxTemp > 50)
    {xkbdi = (int)KBDI;
     xtemp = (double)iMaxTemp;
     idq = (800.0 - xkbdi) * (0.9679 * exp((double) 0.0486 * xtemp) -
           8.299) * 0.001 / (1.0 + 10.88 *
           exp((double)-0.04409 * fAvgPrecip)) + 0.5;
     KBDI = KBDI + idq;
     }
    return (KBDI);
}

/* begin XferHerb	*********************************************************
*
*
***************************************************************************/
//double NFDR2016Calc::Cure(double fMCHerb, double fGSI, double fGreenupThreshold, double fGSIMax)
double NFDR2016Calc::Cure(double fGSI, double fGreenupThreshold, double fGSIMax)
{
   double GreenupThreshold,MaxGSI,MinLFMVal,MaxLFMVal;
    
   double GSI = 0.0; 
   if(fGSI >= 0 && fGSI <= 1){ 
	   GSI = fGSI; 
	   GreenupThreshold = fGreenupThreshold;
	   MaxGSI = fGSIMax;
	   GreenupThreshold = fGreenupThreshold;
   }
   else
   {
	   HerbFM.GetLFMParameters(&MaxGSI, &GreenupThreshold, &MinLFMVal, &MaxLFMVal);
	   GSI = HerbFM.CalcRunningAvgGSI();
   }
   (GSI < GreenupThreshold)? fctCur = 1 : fctCur = -1.0 /(1.0 - GreenupThreshold) * (GSI/MaxGSI) + 1.0/(1.0 - GreenupThreshold);

   if (fctCur < 0) fctCur = 0.0;
   if (fctCur > 1) fctCur = 1.0;

   W1P = W1 + WHERB * fctCur;
   WHERBP = WHERB * (1 - fctCur);
   return (fctCur);
}

/* begin iCalcIC  *********************************************************
*
*
***************************************************************************/
//int NFDR2016Calc::iCalcIC (int iTemp, int iSOW,CHANGED SB
 //        double fMC1, int iSC, int* iIC)
// iFuelTemp: Fuel surface temperature (C)
// fMC1: Fuel Moisture Content
//
/*int NFDR2016Calc::iCalcIC (int iFuelTemp, double fMC1, double fSC, int* iIC)
{
   double tfact = 0.0, TMPPRM = 0.0;
   double PNORM1 = 0.00232, PNORM2 = 0.99767;//, PNORM3 = 0.0000185;
   double QIGN = 0.0, CHI = 0.0, PI = 0.0, SCN = 0.0, PFI = 0.0;

   if (SCM <= 0)
    {
      *iIC = 0;
      return(0);
      }

    // Replace iTemp with the Nelson-derived fuel surface temperature
    TMPPRM = iFuelTemp;
   QIGN = 144.5 - (0.266 * TMPPRM) - (0.00058 * TMPPRM * TMPPRM)
            - (0.01 * TMPPRM * fMC1)
         + 18.54 * (1.0 - exp(-0.151 * fMC1))
         + 6.4 * fMC1;
   if (QIGN >= 344.0)
    {
      *iIC = 0;
      return(1);
      }

   CHI = (344.0 - QIGN) / 10.0;
   if ((pow(CHI, 3.66) * 0.000923 / 50) <= PNORM1)
    {
      *iIC = 0;
      return(1);
      }

   PI = ((pow(CHI, 3.66) * 0.000923 / 50) - PNORM1) * 100.0 / PNORM2;
   if (PI < 0) PI = 0;
   if (PI > 100) PI = 100;
   SCN = 100.0 * fSC / SCM;
   if (SCN > 100.0) SCN = 100.0;
   PFI = pow(SCN, 0.5);
   *iIC = 0.10 * PI * PFI;

   if ((SOW >= 5) && (SOW <= 7))
    *iIC = 0;

   return (1);
}*/
void NFDR2016Calc::SetGSIParams(double MaxGSI, double GreenupThreshold, double TminMin /*= -2.0*/, double TminMax /*= 5.0*/, double VPDMin /*= 900 */, double VPDMax /*= 4100 */,
	double DaylMin /*= 36000*/, double DaylMax /*= 39600*/, unsigned int MAPeriod/* = 21U*/, bool UseVPDAvg, unsigned int nPrecipDays/* = 30*/, double rtPrecipMin /*= 0.5*/, 
    double rtPrecipMax /*= 1.5*/, bool UseRTPrecip /* = false*/)
{
	GsiFM.SetLimits(TminMin, TminMax, VPDMin, VPDMax, DaylMin, DaylMax, rtPrecipMin, rtPrecipMax);
	GsiFM.SetMAPeriod(MAPeriod);
	GsiFM.SetUseVPDAvg(UseVPDAvg);
	GsiFM.SetLFMParameters(MaxGSI, GreenupThreshold, 30, 250);
	GsiFM.SetNumPrecipDays(nPrecipDays);
    GsiFM.SetUseRTPrecip(UseRTPrecip);
}

void NFDR2016Calc::SetHerbGSIparams(double MaxGSI, double GreenupThreshold, double TminMin /*= -2.0*/, double TminMax /*= 5.0*/ , double VPDMin /*= 900 */ , double VPDMax /*= 4100 */ ,
	double DaylMin /*= 36000*/, double DaylMax /*= 39600*/, unsigned int MAPeriod/* = 21U*/, bool UseVPDAvg, unsigned int nPrecipDays/* = 30*/, double rtPrecipMin /*= 0.5*/, 
    double rtPrecipMax /*= 1.5*/, bool UseRTPrecip /* = false*/,
    double herbMin /*= 30.0*/, double herbMax/* = 250.0*/)
{
	HerbFM.SetLimits(TminMin, TminMax, VPDMin, VPDMax, DaylMin, DaylMax, rtPrecipMin, rtPrecipMax);
	HerbFM.SetMAPeriod(MAPeriod);
	HerbFM.SetUseVPDAvg(UseVPDAvg);
	HerbFM.SetLFMParameters(MaxGSI, GreenupThreshold, herbMin, herbMax);
	HerbFM.SetNumPrecipDays(nPrecipDays);
    HerbFM.SetUseRTPrecip(UseRTPrecip);
    if (MCHERB < herbMin)
        MCHERB = herbMin;
    if (MCHERB > herbMax)
        MCHERB = herbMax;
}

void NFDR2016Calc::SetWoodyGSIparams(double MaxGSI, double GreenupThreshold, double TminMin /*= -2.0*/ , double TminMax /*= 5.0*/ , double VPDMin /*= 900 */ , double VPDMax /*= 4100 */ ,
	double DaylMin /*= 36000*/, double DaylMax /*= 39600*/, unsigned int MAPeriod /* = 21U*/, bool UseVPDAvg, unsigned int nPrecipDays/* = 30*/, double rtPrecipMin /*= 0.5*/, 
    double rtPrecipMax /*= 1.5*/, bool UseRTPrecip /* = false*/,
    double woodyMin /*= 60.0*/, double woodyMax /*= 200.0*/)
{
	WoodyFM.SetLimits(TminMin, TminMax, VPDMin, VPDMax, DaylMin, DaylMax, rtPrecipMin, rtPrecipMax);
	WoodyFM.SetMAPeriod(MAPeriod);
	WoodyFM.SetUseVPDAvg(UseVPDAvg);
	WoodyFM.SetLFMParameters(MaxGSI, GreenupThreshold, woodyMin, woodyMax);
	WoodyFM.SetNumPrecipDays(nPrecipDays);
    WoodyFM.SetUseRTPrecip(UseRTPrecip);
    if (MCWOOD < woodyMin)
        MCWOOD = woodyMin;
    if (MCWOOD > woodyMax)
        MCWOOD = woodyMax;
}


void NFDR2016Calc::SetStartKBDI(int sKBDI)
{
	YKBDI = KBDI = StartKBDI = sKBDI;
}

int NFDR2016Calc::GetStartKBDI()
{
	return StartKBDI;
}

double NFDR2016Calc::GetXDaysPrecipitation(int nDays)
{
	double startVal = 0.0, val = 0.0;
	if (nDays >= qPrecip.size())
		val = std::accumulate(qPrecip.begin(), qPrecip.end(), startVal);
	else
	{
		//return std::accumulate(qPrecip._Get_container()., qPrecip._Get_container().end(), startVal);
		int startDay = qPrecip.size() - nDays;
		for (int d = startDay; d < qPrecip.size(); d++)
		{
			val += qPrecip.at(d);
		}
	}
	return val;
}

bool NFDR2016Calc::ReadState(string fileName)
{
	NFDR2016CalcState state;
	bool loadStatus = state.LoadState(fileName);
	if(loadStatus)
		return LoadState(state);
	return loadStatus;
}

bool NFDR2016Calc::SaveState(string fileName)
{
	NFDR2016CalcState state(this);
	return state.SaveState(fileName);
}

bool NFDR2016Calc::LoadState(NFDR2016CalcState state)
{
	NFDRSVersion = state.m_NFDRSVersion;
	Lat = state.m_Lat;
	FuelModel = state.m_FuelModel;
	SlopeClass = state.m_SlopeClass;
	AvgPrecip = state.m_AvgPrecip;
	UseCuring = state.m_UseCuring;
	UseLoadTransfer = state.m_UseLoadTransfer;
	KBDIThreshold = state.m_KBDIThreshold;
	Init(Lat, FuelModel, SlopeClass, AvgPrecip, UseLoadTransfer, UseCuring, state.gsiState.m_IsAnnual, KBDIThreshold);


	BI = state.m_BI;
	CummPrecip = state.m_CummPrecip;
	ERC = state.m_ERC;
	m_GSI = state.m_GSI;
	IC = state.m_IC;
	KBDI = state.m_KBDI;
    KBDIThreshold = state.m_KBDIThreshold;
	lastUtcUpdateTime = state.m_lastUtcUpdateTime;
    lastDailyUpdateTime = state.m_lastDailyUpdateTime;
	MC1 = state.m_MC1;
	MC10 = state.m_MC10;
	MC100 = state.m_MC100;
	MC1000 = state.m_MC1000;
	MCHERB = state.m_MCHERB;
	MCWOOD = state.m_MCWOOD;
	nConsectiveSnowDays = state.m_nConsectiveSnowDays;
	num_updates = state.m_num_updates;
	PrevYear = state.m_PrevYear;
	SC = state.m_SC;
	StartKBDI = state.m_StartKBDI;
	YesterdayJDay = state.m_YesterdayJDay;
	YKBDI = state.m_YKBDI;
	for (int i = 0; i < state.m_qPrecip.size(); i++)
	{
		qPrecip.push_back(state.m_qPrecip.at(i));
	}
    //added 2021/01/26 - Hourly temp RH and precip deques
    qHourlyPrecip.clear();
    qHourlyRH.clear();
    qHourlyTemp.clear();
    for (int h = 0; h < nHoursPerDay; h++)
    {
        qHourlyTemp.push_back(state.m_qHourlyTemp[h]);
        qHourlyRH.push_back(state.m_qHourlyRH[h]);
        qHourlyPrecip.push_back(state.m_qHourlyPrecip[h]);
    }
    //end 2021/01/26 additions
	OneHourFM.SetState(state.fm1State);
	TenHourFM.SetState(state.fm10State);
	HundredHourFM.SetState(state.fm100State);
	ThousandHourFM.SetState(state.fm1000State);
	HerbFM.SetState(state.herbState);
	WoodyFM.SetState(state.woodyState);
	GsiFM.SetState(state.gsiState);

	return true;
}

double NFDR2016Calc::GetMinTemp()
{
    double minTemp = NORECORD;
    for (std::deque<double>::iterator it = qHourlyTemp.begin(); it != qHourlyTemp.end(); ++it)
    {
        if ((*it) != NORECORD)
        {
            if (minTemp == NORECORD)
                minTemp = *it;
            else
                minTemp = min(minTemp, *it);
        }
    }
    return minTemp;
}

double NFDR2016Calc::GetMaxTemp()
{
    double maxTemp = NORECORD;
    for (std::deque<double>::iterator it = qHourlyTemp.begin(); it != qHourlyTemp.end(); ++it)
    {
        if ((*it) != NORECORD)
        {
            if (maxTemp == NORECORD)
                maxTemp = *it;
            else
                maxTemp = max(maxTemp, *it);
        }
    }
    return maxTemp;
}
double NFDR2016Calc::GetMinRH()
{
    double minRH = NORECORD;
    for (std::deque<double>::iterator it = qHourlyRH.begin(); it != qHourlyRH.end(); ++it)
    {
        if ((*it) != NORECORD)
        {
            if (minRH == NORECORD)
                minRH = *it;
            else
                minRH = min(minRH, *it);
        }
    }
    return minRH;
}
double NFDR2016Calc::GetPcp24()
{
    double pcp24 = 0.0;
    for (std::deque<double>::iterator it = qHourlyPrecip.begin(); it != qHourlyPrecip.end(); ++it)
    {
        if (*it != NORECORD)
            pcp24 += *it;
    }
    return pcp24;
}
