#ifndef _NFDRS2016_H
#define _NFDRS2016_H


// Standard include files
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <numeric>
#include <algorithm>
#include <deque>
#include "deadfuelmoisture.h"
#include "livefuelmoisture.h"
#include "station.h"
#include "utctime.h"

/*using namespace std;
using std::istream;
using std::ostream;
using std::setfill;
using std::setw;
using std::string;
using std::vector;
*/

class NFDR2016CalcState;
/**************************** NFDRDLL.H ***********************************

First Letter of Variable or Function Name:

    i = integer (2-byte)
   f = doubleing Point

Variable Names Used in Function Calls:

    iAdjust			<desired +/- addition to 1-hour fuel moisture>
    iAspect       	1=N, 2=E, 3=S, 4=W
   iBI				Burning Index
   fDEPTH			Fuel Model's depth
   iDoHerbXfer		1=Transfer (appropriate amt of) Herb Load to 1Hr, 0=No
   fERC				Energy Release Component
   iExposure		1=Exposed, 0=Not
   iFIL				Fire Intensity Level
    fFL				Flame Length
   iFM				ASCII Value of Fuel Model ID:  65=A, 66=B, ...
   iHD				Fuel Model's Dead (& Live) Heat of Combustion
   iHour				1, 2, ... 24
   iIC				Ignition Component
   iJulian			Annualized Julian:  1=Jan1,... , 365=Dec31
   iLat				Nearest Degree of Latitude
   iMaxRH			24-Hour Max RH (-999 means missing)
   iMaxTemp			24-Hour Max Temp (F) (-999 means missing)
   fMCn				Fuel Moistures for fuel class "n"
   iMinRH			24-Hour Min RH (-999 means missing)
   iMinTemp			24-Hour Min Temp (F) (-999 means missing)
    iMonth			1=Jan, ... 12=Dec
   iMXD				Fuel Model's Moisture of Extinction (%)
   iNFMAS			1=Use NFMAS Variations, 0=No (Use standard)
   iRH				Observed RH
   fROS				Rate of Spread
   iSAn				Fuel Model's Surface/Area for fuel class "n"
   iSC				Spread Component
   iSCM				Fuel Model's Spread At Which All Ignitiona -> Reportable
   iSite          1=Low, 2=Mid, 3=Upper
   iSlope30       1=30% or less, 2=greater than 30%
   iSlopeCls		NFDRS Slope Class (1-5)
   iSOW				State of Weather (0=Clear, ...) (-1 allowed in iCalcIC)
   iTemp				Observed Temp (F) (Can be fuel temp in iCalcIC)
   fWn				Fuel Model's Loading for fuel class "n"
   fWNDFC			Fuel Model's (or desired) Wind Reduction Factor
   iWS				20-Foot Windspeed


Functions:

    Keywords in function names:

    Calc		Performs a calculation, returns value(s)
    Do			Performs an action
    Get		Returns values from the DLL
      Set		Sets values in the DLL

    Some functions return calculated values. Most, especially integer
   typed functions, return "success status": 1=Success, 0=Failure.

   Most requested values are returned as function arguments.

Visual BASIC Notes:

    integer used throughout, to be compatable with VB Integer Type.
   Strings Avoided to prevent calling issues between VB/C.

   In VB, use "ByVal" unless pointer (*) in DLL.

   Example DECLARE statements and calling conventions:

    DECLARE FUNCTION iSetFuelModel LIB "NFDRCALC.DLL"
            (ByVal intFuelMod) as integer

      DECLARE FUNCTION iGetFuelModel LIB "NFDRCALC.DLL"
            (intFuelModel) as integer

      Dim strFM as string
      dim intFM as integer
      dim intReturn as integer

      strFM = "A"
      intFM = ASC(strFM)
      intReturn = iSetFuelModel (intFM)
      if intReturn = 0 then
        msgbox "Invalid Fuel Model"
      end if

      ...

      intReturn = iGetFuelModel (intFM)
      if intReturn = 0 then
        msgbox "No Fuel Model Loaded Into DLL."
      else
        msgbox "Current Fuel Model Is " & CHR$(intFM)
      end if

***************************************************************************/

//------------------------------------------------------------------------------
/*! \class NFDR2016Calc
    \brief Main calculator for the US National Fire Danger Rating System components
    and indices.
*/
class NFDR2016Calc
{
    public:
		NFDR2016Calc();
        NFDR2016Calc(double Lat,char FuelModel,int SlopeClass, double AvgAnnPrecip,bool LT,bool Cure, bool IsAnnual);
        ~NFDR2016Calc();
        // Member functions
		void Init(double Lat, char FuelModel, int SlopeClass, double AvgAnnPrecip, bool LT, bool Cure, bool isAnnual, int kbdiThreshold, int RegObsHour = 13);
       void Update(Wx);
	   void Update(int Year, int Month, int Day, int Hour, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double MinRH, double PPTAmt, double pcp24, double SolarRad, double WS, bool SnowDay, int RegObsHr);
       void Update(int Year, int Month, int Day, int Hour, double Temp, double RH, double PPTAmt, double SolarRad, double WS, bool SnowDay);
       void UpdateDaily(int Year, int Month, int Day, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double MinRH, double pcp24, double WS, double fMC1, double fMC10, double fMC100, double fMC1000, double fuelTemp, bool SnowDay/* = false*/);
        void iSetFuelModel (char cFM);
        int iSetFuelMoistures (double fMC1, double fMC10,double fMC100, double fMC1000, double fMCWood, double fMCHerb, double fuelTempC);
        int iCalcIndexes (int iWS, int iSlopeCls,double* fSC,double* fERC, double* fBI, double* fIC,double fGSI = -999,double fKBDI = -999);
        int iCalcKBDI (double fPrecipAmt, int iMaxTemp,double fCummPrecip, int iYKBDI, double fAvgPrecip);
		double Cure(double fGSI = -999, double fGreenupThreshold = 0.5, double fGSIMax = 1.0);

		void SetGSIParams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false);
		void SetHerbGSIparams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false,
            double herbMin = 30.0, double herbMax = 250.0);
		void SetWoodyGSIparams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false,
            double woodyMin = 60.0, double woodyMax = 200.0);

		void SetStartKBDI(int sKBDI);
		int GetStartKBDI();

		void SetSCMax(int maxSC);
		int GetSCMax();
		void SetMxdHumid(bool isHumid);
		bool GetMxdHumid();
		double GetFuelTemperature();
		double GetXDaysPrecipitation(int nDays);
		bool ReadState(string fileName);
		bool SaveState(string fileName);
		bool LoadState(NFDR2016CalcState state);
		static const int nPrecipQueueDays = 90;
        static const int nHoursPerDay = 24;
        double GetMinTemp();
        double GetMaxTemp();
        double GetMinRH();
        double GetPcp24();

		double CTA;
        double Lat;
        unsigned long num_updates;
        int NFDRSVersion;
        int YesterdayJDay;
        int SlopeClass;
       // double AvgAnnPrecip;        // Average Annual Precipitation (inches)
        DeadFuelMoisture OneHourFM;
        DeadFuelMoisture TenHourFM;
        DeadFuelMoisture HundredHourFM;
        DeadFuelMoisture ThousandHourFM;
		LiveFuelMoisture GsiFM;
		LiveFuelMoisture HerbFM;
		LiveFuelMoisture WoodyFM;

		int FuelModel;
        int SG1, SG10, SG100, SG1000, SGWOOD, SGHERB;
        double W1, W10, W100, W1000, WWOOD, WHERB, WDROUGHT, W1P, WHERBP,WTOT;
        double L1, L10, L100, L1000, LWOOD, LHERB, LDROUGHT;
		double DroughtUnit, fctCur;
		double DEPTH, WNDFC;
        int MXD, HD, SCM;
        double MC1, MC10, MC100, MC1000, MCWOOD, MCHERB;
		double BI, ERC, SC, IC;
		int PrevYear;
		int KBDI, YKBDI, StartKBDI, KBDIThreshold;
		double CummPrecip, AvgPrecip;
        bool UseLoadTransfer, UseCuring;
        bool SnowCovered;
        double WTMCD, WTMCL,WTMCDE, WTMCLE;
		bool mxdHumid;
		double FuelTemperature;
		double m_GSI;
		int nConsectiveSnowDays;
       // ofstream debug;
		//time_t lastObsTime;
        //list<double> prcp;
        int m_regObsHour;
		//time_t lastUpdateTime;
        time_t utcHourDiff;
        utctime::UTCTime lastUtcUpdateTime;
        utctime::UTCTime lastDailyUpdateTime;
        std::deque<double> qPrecip;
        std::deque<double> qHourlyPrecip;
        std::deque<double> qHourlyTemp;
        std::deque<double> qHourlyRH;
};


#endif
