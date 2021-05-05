#include "station.h"

// simple class: each station gets its own object
// which consists of the initial calculator values
// for that station
//
// and then a list to the Wx records

Wx::Wx()
{

}

Wx::~Wx()
{

}

Station::Station(){


    // hardcoded defaults
    m_GreenJulian = 100;
    m_FreezeJulian = 300;
    m_Use88 = 88;
    m_ClimateCls = 1;
    m_LatDegrees = 46;
    m_HerbAnnual = 1;
    m_Deciduous = 1;
    m_FM1Eq10 = 1;

    m_Start1000 = 10;
    m_StartKBDI = 25;
    m_AvgPrecip = 20;

    m_NFDRSFM = "G";

    m_LFIdaysAvg = 21;
    m_TminMin = -2.0;
    m_TminMax = 5.0;
    m_VPDMin = 900.0;
    m_VPDMax = 4100.0;
    m_DaylenMin = 36000;
    m_DaylenMax = 39600;
    m_HerbMaxGSI = 1.0;
    m_HerbGreenup = 0.5;
    m_HerbMax = 250.0;
    m_HerbMin = 30;
    m_WoodyMaxGSI = 1.0;
    m_WoodyGreenup = 0.5;
    m_WoodyMax = 200.0;
    m_WoodyMin = 60.0;
    m_HerbSlope = 1.0;
    m_HerbIntercept = 1.0;
    m_WoodySlope = 1.0;
    m_WoodyIntercept = 1.0;

    m_Conditional = 0;
    m_BinSize = 1;

    // set moisture methods to default 'modified' (or 'new', 'custom', etc.)
    // note that we always produce a baseline 'old' set of results
    m_Method1h = 1;
    m_Method10h = 1;
    m_Method100h = 1;
    m_Method1000h = 1;
    m_MethodHerb = 1;
    m_MethodWoody = 1;

    m_RegObsHour = 13;

    m_StatsVar1 = 0;    // default to ERC

    m_UseSnowFlag = 0;   // useSnowFlag option
}
Station::~Station(){}

// set hardcoded defaults
// which are then overridden
// by optional .INI file
void Station::setDefaults(Station *defaultStation)
{

    // hardcoded defaults
    m_GreenJulian = 100;
    m_FreezeJulian = 300;
    m_Use88 = 88;
    m_ClimateCls = 1;
    m_LatDegrees = 46;
    m_HerbAnnual = 1;
    m_Deciduous = 1;
    m_FM1Eq10 = 1;

    m_Start1000 = 10;
    m_StartKBDI = 25;
    m_AvgPrecip = 20;

    m_NFDRSFM = "G";

    m_LFIdaysAvg = 21;
    m_TminMin = -2.0;
    m_TminMax = 5.0;
    m_VPDMin = 900.0;
    m_VPDMax = 4100.0;
    m_DaylenMin = 36000;
    m_DaylenMax = 39600;
    m_HerbMaxGSI = 1.0;
    m_HerbGreenup = 0.5;
    m_HerbMax = 250.0;
    m_HerbMin = 30;
    m_WoodyMaxGSI = 1.0;
    m_WoodyGreenup = 0.5;
    m_WoodyMax = 200.0;
    m_WoodyMin = 60.0;
    m_HerbSlope = 1.0;
    m_HerbIntercept = 1.0;
    m_WoodySlope = 1.0;
    m_WoodyIntercept = 1.0;

    // set moisture methods to 'new'
    m_Method1h = 1;
    m_Method10h = 1;
    m_Method100h = 1;
    m_Method1000h = 1;
    m_MethodHerb = 1;
    m_MethodWoody = 1;

    m_RegObsHour = 13;

    m_StatsVar1 = 0;  // default to ERC

    m_Conditional = 0;
    m_BinSize = 1;

    m_UseSnowFlag = 0;

    // copy attributes from the default station
    if (defaultStation)
    {
        m_GreenJulian = defaultStation->m_GreenJulian;
        m_FreezeJulian = defaultStation->m_FreezeJulian;
        m_Use88 = defaultStation->m_Use88;
        m_ClimateCls = defaultStation->m_ClimateCls;
        m_SlopeCls = defaultStation->m_SlopeCls;
        m_Aspect = defaultStation->m_Aspect;
        m_SlopePos = defaultStation->m_SlopePos;
        m_LatDegrees = defaultStation->m_LatDegrees;
        m_HerbAnnual = defaultStation->m_HerbAnnual;
        m_Deciduous = defaultStation->m_Deciduous;
        m_FM1Eq10 = defaultStation->m_FM1Eq10;

        m_Start1000 = defaultStation->m_Start1000;
        m_StartKBDI = defaultStation->m_StartKBDI;
        m_AvgPrecip = defaultStation->m_AvgPrecip;

        m_NFDRSFM = defaultStation->m_NFDRSFM;

        // following are LFI values
        m_LFIdaysAvg = defaultStation->m_LFIdaysAvg;
        m_TminMin = defaultStation->m_TminMin;
        m_TminMax = defaultStation->m_TminMax;
        m_VPDMin = defaultStation->m_VPDMin;
        m_VPDMax = defaultStation->m_VPDMax;
        m_DaylenMin = defaultStation->m_DaylenMin;
        m_DaylenMax = defaultStation->m_DaylenMax;
        m_HerbMaxGSI = defaultStation->m_HerbMaxGSI;
        m_HerbGreenup = defaultStation->m_HerbGreenup;
        m_HerbMax = defaultStation->m_HerbMax;
        m_HerbMin = defaultStation->m_HerbMin;
        m_WoodyMaxGSI = defaultStation->m_WoodyMaxGSI;
        m_WoodyGreenup = defaultStation->m_WoodyGreenup;
        m_WoodyMax = defaultStation->m_WoodyMax;
        m_WoodyMin = defaultStation->m_WoodyMin;
        m_HerbSlope = defaultStation->m_HerbSlope;
        m_HerbIntercept = defaultStation->m_HerbIntercept;
        m_WoodySlope = defaultStation->m_WoodySlope;
        m_WoodyIntercept = defaultStation->m_WoodyIntercept;

        // set moisture methods to default/'traditional'
        m_Method1h = defaultStation->m_Method1h;
        m_Method10h = defaultStation->m_Method10h;
        m_Method100h = defaultStation->m_Method100h;
        m_Method1000h = defaultStation->m_Method1000h;
        m_MethodHerb = defaultStation->m_MethodHerb;
        m_MethodWoody = defaultStation->m_MethodWoody;

        m_RegObsHour = defaultStation->m_RegObsHour;

        m_StatsVar1 = defaultStation->m_StatsVar1;

        m_Conditional = defaultStation->m_Conditional;
        m_BinSize = defaultStation->m_BinSize;

        m_UseSnowFlag = defaultStation->m_UseSnowFlag;
    }

}

//
// read an FW13 file, and return a list of station objects
// (each with its own list of Wx records)
//
// this code was copied (nearly verbatim) from FFP4/ImportDialog.cpp
//
int Station::ReadFW13(string fileName)
{
    int status = -1,xlim = 0;  // return number of records read, or -1 for error
    long updates = 0, daily = 0,raws = 0, fcast = 0, other = 0, nonwx = 0, rejects = 0, lineNo = 0,errors=0;
    bool isRaws, isNFDRS;
    //

    long x = 0,i=0;
    ifstream fin(fileName.c_str());
    double mymax = -1e6;
    if(fin.is_open())
    {
        string str;

        while(getline(fin, str))
        {

            std::size_t found = str.find("<!");
            if (found!=std::string::npos){  xlim = 19; continue;}
            found = str.find("</pre>");
            if (found!=std::string::npos){ break;}


            if(str.size() > 70 && i >= xlim)
            {

                istringstream ss(str);
                string temp;

                // Record type (W13). All records begin with this record type identifier code.
                // Station Number
                string StationID = str.substr(3,6);
                // Observation date (YYYYMMDD).
                string ObsDate = str.substr(9,8);
                // Observation time (0000-2359).
                string ObsTime = str.substr(17,4);
                time_t loctime;
                struct tm timeinfo, *loctimeinfo;
                int year, month, day,hour;
                year = atoi(ObsDate.substr(0,4).c_str());
                month = atoi(ObsDate.substr(4,2).c_str());
                day = atoi(ObsDate.substr(6,2).c_str());
                hour = atoi(ObsTime.substr(0,2).c_str() );
                memset(&timeinfo,0, sizeof(struct tm));
                timeinfo.tm_isdst = 0;  /* Allow mktime to determine DST setting. */
                timeinfo.tm_mon   = month -1;
                timeinfo.tm_mday = day;
                timeinfo.tm_year = year - 1900;
                timeinfo.tm_hour = hour;
                timeinfo.tm_min = 0;
                loctime = mktime (&timeinfo);
                loctimeinfo = localtime(&loctime);
                char MyFormattedDateTime [80];
                strftime (MyFormattedDateTime,80,"%Y%m%d%H:%M:%S",loctimeinfo);
                char JDay[80];
                strftime (JDay,80,"%j",loctimeinfo);

                // Observation type (O=Published NFDRS, R=RAWS, F=Forecast, X=Other).
                // If type is R and observation time is the station’s standard NFDRS
                // observation time(RS) and the State of weather code is not blank,
                // then State of weather code and Wet Flag for this observation were
                // estimated by the WIMS RAWS Gateway routines, not from the
                // WIMS Published NFDRS Observation.
                string OType = str.substr(21,1);

                // State of weather code.
                string SOW = str.substr(22,1);

                // Dry bulb temperature (degrees Fahrenheit or degrees Celsius based on Measurement Type code [col. 63]).
                string DryBulbTemp = str.substr(23,3);

                // Atmospheric moisture (wet bulb temperature, relative humidity (percent), or dewpoint temperature based on Moisture Type code [col. 62]).
                string RH = str.substr(26,3);

                // Wind direction azimuth measured from true north. 0 (zero) means no wind direction, 360 is north.
                string WindDir = str.substr(29,3);

                // Average windspeed over a 10-minute period (miles or kilometers per hour based on Measurement Type code).
                string WindSpeed = str.substr(32,3);

                // Measured 10-hour time lag fuel moisture
                string MeasTenHrFM = str.substr(35,2);

                // Maximum Temperature (degrees Fahrenheit or degrees Celsius based on Measurement Type code [col. 63]).
                string MaxTemp = str.substr(37,3);

                // Minimum Temperature (degrees Fahrenheit or degrees Celsius based on Measurement Type code [col. 63]).
                string MinTemp = str.substr(40,3);

                // Maximum relative humidity (percent).
                string MaxRH = str.substr(43,3);

                // Minimum relative humidity (percent)
                string MinRH = str.substr(46,3);

                // Precipitation duration (hours).
                string PrecipDur = str.substr(49,2);

                // Precipitation amount based on Measurement Type code [col. 63]. Blanks=no precipitation.
                // U.S. measurement: inches with implied decimal nn.nnn format; trace shown as 00005. Metric
                // measurement: measured in millimeters, no implied decimal; trace shown as 00001.
                string PrecipAmnt = str.substr(51,5);

                // Wet flag (Y/N).  Signals fuels over the fire danger rating are wet from widespread or long duration rain.
                string WetFlag = str.substr(56,1);

                // Herbaceous greenness factor (0-20).
                string HerbGF = str.substr(57,2);

                // Shrub greenness factor (0-20).
                string ShrubGF = str.substr(59,2);

                // Moisture Type code (1=Wet bulb, 2=Relative Humidity, 3=Dewpoint).
                string MoistureType = str.substr(61,1);

                // Measurement Type code: 1=U.S.,2=Metric. Affects temperature (Fahrenheit or Celsius), wind (miles or kilometers per hour), and precipitation (decimal inches or millimeters).
                string MeasurementType = str.substr(62,1);

                // Season code (1=Winter, 2=Spring, 3=Summer, 4=Fall).
                string SeasonCode = str.substr(63,1);

                // Solar radiation (watts per square meter).
                string SolarRad = str.substr(64,4);

                // Wind direction, degrees of Peak Gust during the hour. Zero means no wind direction, 360 is north.
                string WindGustDir = str.substr(68,3);

                // Speed of Peak Gust during the hour. (miles or kilometers per hour based on Measurement Type code).
                string WindGustSpeed = str.substr(71,3);

                // Snow Flag (Y/N). Signals fuels over the fire danger rating area are snow covered.
                string SnowFlag = str.substr(74,1);

                Wx wxRec;
                wxRec.m_ObsDate = *loctimeinfo;
                wxRec.m_ObsType = OType;
                wxRec.m_JDay = atoi(JDay);
                wxRec.m_Year = year;
                wxRec.m_Month = month;
                wxRec.m_Day = day;
                wxRec.m_Hour = hour;
                wxRec.m_RH = atof(RH.c_str());

                wxRec.m_Temp = atof(DryBulbTemp.c_str());
                wxRec.m_TmpMax = atof(MaxTemp.c_str());
                wxRec.m_TmpMin = atof(MinTemp.c_str());
                wxRec.m_RHMax = atof(MaxRH.c_str());
                wxRec.m_RHMin = atof(MinRH.c_str());
                wxRec.m_PPTDUR = atoi(PrecipDur.c_str());
                wxRec.m_PPTDUR24 = atoi(PrecipDur.c_str());


                if(PrecipAmnt.find(".") != std::string::npos)
                {
                    wxRec.m_PPTAMT = atof(PrecipAmnt.c_str());
                    wxRec.m_PPTAMT24 = atof(PrecipAmnt.c_str());

                }
                else
                {

                    wxRec.m_PPTAMT = atof(PrecipAmnt.c_str()) / 1000.0;
                    wxRec.m_PPTAMT24 = atof(PrecipAmnt.c_str()) / 1000.0;

                }

                // Test for possible snow conditions
                if(wxRec.m_TmpMax < 28 && wxRec.m_PPTAMT24 > 0)
                {
                    wxRec.m_SnowDay = 1;
                }
                else
                {
                    wxRec.m_SnowDay = 0;
                }

                wxRec.m_SolarRadiation = atof(SolarRad.c_str());
                wxRec.m_WS = atof(WindSpeed.c_str());

                if(x > 0)
                {
                    float delta = wxRec.m_PPTAMT - wxList[x-1].m_PPTAMT;

                    if(delta > 0){
                        wxRec.m_PPTACC = wxList[x-1].m_PPTACC + delta;

                    } else {
                        wxRec.m_PPTACC = wxList[x-1].m_PPTACC;
                    }
                    //cout << year << " " << JDay << " "  << wxRec.m_PPTACC << endl;
                }
                else
                {
                    if(wxRec.m_PPTAMT >= 0)
                    {
                        wxRec.m_PPTACC = wxRec.m_PPTAMT;
                        wxRec.m_PPTACCSNOW = wxRec.m_PPTAMT;
                    }
                    else
                    {
                        wxRec.m_PPTACC = 0.0;
                        wxRec.m_PPTACCSNOW = 0.0;
                    }
                }

                if(wxRec.m_SnowDay == 1) {
                    wxRec.m_PPTACCSNOW += 1/2.54;

                }

                wxRec.m_WetFlag = atoi(WetFlag.c_str());
                // Before appending, need to add some QC here
                if(wxRec.m_RH > 0) {
                   wxList.push_back(wxRec);x++;}
                //cout << wxRec.m_PPTACC << endl;
                if(SnowFlag == "Y")
                {
                    wxRec.m_SnowFlag = 1;
                }
                else
                {
                    wxRec.m_SnowFlag = 0;
                }

            }
            i++;
        }

     }

     return EXIT_SUCCESS;
}


