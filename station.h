#ifndef STATION_H
#define STATION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <string.h>

using namespace std;
class Wx
{
public:
    Wx();
    ~Wx();


    // Wx Obs record members
    string m_StationID;
    tm m_ObsDate;
    long m_Hour;
    int m_JDay;
    int m_Year;
    int m_Month;
    int m_Day;


    string m_ObsType;
    int m_SOW;
    double m_Temp;
    double m_RH;
    int m_WAzimuth;
    int m_WDir;
    int m_WS;

    float m_OMC10;
    long m_TmpMax;
    long m_TmpMin;
    long m_RHMax;
    long m_RHMin;
    long m_PPTDUR;
    float m_PPTAMT;
    float m_PPTACC;
    float m_PPTACCSNOW;
    long m_WetFlag;
    long m_GreenHerb;
    long m_GreenShrub;
    long m_Season;
    long m_SolarRadiation;
    long m_GustDir;
    long m_GustSpd;
    long m_SnowFlag;

    // the following values will be calculated
    float m_HourlyPrecip;    // actual hourly precip
    long m_PPTDUR24;         // daily precip duration (1400-1300 etc.)
    float m_PPTAMT24;        // daily precip amt (1400-1300 etc.)
    float m_SnowDay;         // Day with recorded snow

    // optional fires
    int m_Fires;
    int m_LargeFires;
    // optional variables to save for fire stats
    double m_BaseVar1;   // base-computed version of the var
    double m_ModVar1;    // mod-computed version of the var

};

class Station
{
public:
    Station();
    ~Station();

    string m_StationID;
    std::vector<Wx> wxList;

    void setDefaults(Station *defaultStation);
    int ReadFW13(string fileName);
    // values used to initialize NFDRS calc, etc.
    short m_GreenJulian;
    short m_FreezeJulian;
    short m_Use88;
    short m_ClimateCls;
    short m_SlopeCls;
    short m_Aspect;
    short m_SlopePos;
    short m_LatDegrees;
    short m_HerbAnnual;
    short m_Deciduous;
    short m_FM1Eq10;

    short m_Start1000;
    short m_StartKBDI;
    double m_AvgPrecip;

    string m_NFDRSFM;

    // which variables we want to analyze for firestats
    int m_StatsVar1;

    // use snowflag?
    int m_UseSnowFlag;



    // following are LFI-specific values

    int m_LFIdaysAvg;
    double m_TminMin;
    double m_TminMax;
    double m_VPDMin;
    double m_VPDMax;
    double m_DaylenMin;
    double m_DaylenMax;



    double m_HerbMaxGSI;
    double m_HerbGreenup;
    double m_HerbMax;
    double m_HerbMin;
    double m_WoodyMaxGSI;
    double m_WoodyGreenup;
    double m_WoodyMax;
    double m_WoodyMin;
    double m_HerbSlope;
    double m_HerbIntercept;
    double m_WoodySlope;
    double m_WoodyIntercept;

    // methods to use for each of the 6 moisture vars
    // 0 - 'traditional' method (Fosberg / LFI)
    // 1 - 'new' method (Nelson)

    int m_Method1h,
        m_Method10h,
        m_Method100h,
        m_Method1000h,
        m_MethodHerb,
        m_MethodWoody;

    int m_RegObsHour;

    bool m_Conditional;
    int m_BinSize;



};


#endif // STATION_H
