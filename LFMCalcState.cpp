#include "LFMCalcState.h"
#include <iostream>
#include <fstream>




LFMCalcState::LFMCalcState()
{
	//m_Year = 0;
	//m_Month = 0;
	//m_Day = 0;
	//m_Hour = 0;
	m_UseVPDAvg = 0;
	m_IsHerb = 0;
	m_IsAnnual = 0;
	m_LFIdaysAvg = 0;
	m_Lat = 0.0;
	m_TminMin = 0.0;
	m_TminMax = 0.0;
	m_VPDMin = 0.0;
	m_VPDMax = 0.0;
	m_DaylenMin = 0.0;
	m_DaylenMax = 0.0;
	m_MaxGSI = 0.0;
	m_GreenupThreshold = 0.0;
	m_MaxLFMVal = 0.0;
	m_MinLFMVal = 0.0;
	m_Slope =0.0;
	m_Intercept = 0.0;
	m_hasGreenedUpThisYear = 0;
	m_hasExceeded120ThisYear = 0;
	m_canIncreaseHerb = 0;
	lastHerbFM = 0;
	m_lastUpdateTime = 0;
}

LFMCalcState::LFMCalcState(const LFMCalcState& rhs)
{
	//m_Year = rhs.m_Year;
	//m_Month = rhs.m_Month;
	//m_Day = rhs.m_Day;
	//m_Hour = rhs.m_Hour;
	m_UseVPDAvg = rhs.m_UseVPDAvg;
	m_IsHerb = rhs.m_IsHerb;
	m_IsAnnual = rhs.m_IsAnnual;
	m_LFIdaysAvg = rhs.m_LFIdaysAvg;
	m_Lat = rhs.m_Lat;
	m_TminMin = rhs.m_TminMin;
	m_TminMax = rhs.m_TminMax;
	m_VPDMin = rhs.m_VPDMin;
	m_VPDMax = rhs.m_VPDMax;
	m_DaylenMin = rhs.m_DaylenMin;
	m_DaylenMax = rhs.m_DaylenMax;
	m_MaxGSI = rhs.m_MaxGSI;
	m_GreenupThreshold = rhs.m_GreenupThreshold;
	m_MaxLFMVal = rhs.m_MaxLFMVal;
	m_MinLFMVal = rhs.m_MinLFMVal;
	m_Slope = rhs.m_Slope;
	m_Intercept = rhs.m_Intercept;
	m_hasGreenedUpThisYear = rhs.m_hasGreenedUpThisYear;
	m_hasExceeded120ThisYear = rhs.m_hasExceeded120ThisYear;
	m_canIncreaseHerb = rhs.m_canIncreaseHerb;
	lastHerbFM = rhs.lastHerbFM;
	m_qGSI = rhs.m_qGSI;
	m_lastUpdateTime = rhs.m_lastUpdateTime;
}

LFMCalcState::~LFMCalcState()
{
}

bool LFMCalcState::ReadState(FILE *in)
{
	size_t nRead = fread(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_UseVPDAvg, sizeof(m_UseVPDAvg), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_IsHerb, sizeof(m_IsHerb), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_IsAnnual, sizeof(m_IsAnnual), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_LFIdaysAvg, sizeof(m_LFIdaysAvg), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Lat, sizeof(m_Lat), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_TminMin, sizeof(m_TminMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_TminMax, sizeof(m_TminMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_VPDMin, sizeof(m_VPDMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_VPDMax, sizeof(m_VPDMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_DaylenMin, sizeof(m_DaylenMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_DaylenMax, sizeof(m_DaylenMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MaxGSI, sizeof(m_MaxGSI), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_GreenupThreshold, sizeof(m_GreenupThreshold), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MaxLFMVal, sizeof(m_MaxLFMVal), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MinLFMVal, sizeof(m_MinLFMVal), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Slope, sizeof(m_Slope), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Intercept, sizeof(m_Intercept), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_canIncreaseHerb, sizeof(m_canIncreaseHerb), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&lastHerbFM, sizeof(lastHerbFM), 1, in);
	if (nRead != 1)
		return false;
	short qSize;
	nRead = fread(&qSize, sizeof(qSize), 1, in);
	if (nRead != 1)
		return false;
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
			return false;
		m_qGSI.push_back(tVal);
	}

	return true;
}

bool LFMCalcState::SaveState(FILE *out)
{
	int nWrite = fwrite(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_UseVPDAvg, sizeof(m_UseVPDAvg), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_IsHerb, sizeof(m_IsHerb), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_IsAnnual, sizeof(m_IsAnnual), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_LFIdaysAvg, sizeof(m_LFIdaysAvg), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Lat, sizeof(m_Lat), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_TminMin, sizeof(m_TminMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_TminMax, sizeof(m_TminMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_VPDMin, sizeof(m_VPDMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_VPDMax, sizeof(m_VPDMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_DaylenMin, sizeof(m_DaylenMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_DaylenMax, sizeof(m_DaylenMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MaxGSI, sizeof(m_MaxGSI), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_GreenupThreshold, sizeof(m_GreenupThreshold), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MaxLFMVal, sizeof(m_MaxLFMVal), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MinLFMVal, sizeof(m_MinLFMVal), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Slope, sizeof(m_Slope), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Intercept, sizeof(m_Intercept), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_canIncreaseHerb, sizeof(m_canIncreaseHerb), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&lastHerbFM, sizeof(lastHerbFM), 1, out);
	if (nWrite != 1)
		return false;
	short qSize = m_qGSI.size();
	nWrite = fwrite(&qSize, sizeof(qSize), 1, out);
	if (nWrite != 1)
		return false;
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal = m_qGSI[i];
		nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
		if (nWrite != 1)
			return false;
	}
	return true;
}

bool LFMCalcState::SaveState(ofstream& fout)
{
	fout.write((char *)&m_lastUpdateTime, sizeof(m_lastUpdateTime));
	fout.write((char *)&m_UseVPDAvg, sizeof(m_UseVPDAvg));
	fout.write((char *)&m_IsHerb, sizeof(m_IsHerb));
	fout.write((char *)&m_IsAnnual, sizeof(m_IsAnnual));
	fout.write((char *)&m_LFIdaysAvg, sizeof(m_LFIdaysAvg));
	fout.write((char *)&m_Lat, sizeof(m_Lat));
	fout.write((char *)&m_TminMin, sizeof(m_TminMin));
	fout.write((char *)&m_TminMax, sizeof(m_TminMax));
	fout.write((char *)&m_VPDMin, sizeof(m_VPDMin));
	fout.write((char *)&m_VPDMax, sizeof(m_VPDMax));
	fout.write((char *)&m_DaylenMin, sizeof(m_DaylenMin));
	fout.write((char *)&m_DaylenMax, sizeof(m_DaylenMax));
	fout.write((char *)&m_MaxGSI, sizeof(m_MaxGSI));
	fout.write((char *)&m_GreenupThreshold, sizeof(m_GreenupThreshold));
	fout.write((char *)&m_MaxLFMVal, sizeof(m_MaxLFMVal));
	fout.write((char *)&m_MinLFMVal, sizeof(m_MinLFMVal));
	fout.write((char *)&m_Slope, sizeof(m_Slope));
	fout.write((char *)&m_Intercept, sizeof(m_Intercept));
	fout.write((char *)&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear));
	fout.write((char *)&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear));
	fout.write((char *)&m_canIncreaseHerb, sizeof(m_canIncreaseHerb));
	fout.write((char *)&lastHerbFM, sizeof(lastHerbFM));
	short qSize = m_qGSI.size();
	fout.write((char *)&qSize, sizeof(qSize));
	
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal = m_qGSI[i];
		fout.write((char *)&tVal, sizeof(tVal));

	}
	return true;
}
bool LFMCalcState::ReadState(ifstream& fin)
{
	fin.read((char *)&m_lastUpdateTime, sizeof(m_lastUpdateTime));
	fin.read((char *)&m_UseVPDAvg, sizeof(m_UseVPDAvg));
	fin.read((char *)&m_IsHerb, sizeof(m_IsHerb));
	fin.read((char *)&m_IsAnnual, sizeof(m_IsAnnual));
	fin.read((char *)&m_LFIdaysAvg, sizeof(m_LFIdaysAvg));
	fin.read((char *)&m_Lat, sizeof(m_Lat));
	fin.read((char *)&m_TminMin, sizeof(m_TminMin));
	fin.read((char *)&m_TminMax, sizeof(m_TminMax));
	fin.read((char *)&m_VPDMin, sizeof(m_VPDMin));
	fin.read((char *)&m_VPDMax, sizeof(m_VPDMax));
	fin.read((char *)&m_DaylenMin, sizeof(m_DaylenMin));
	fin.read((char *)&m_DaylenMax, sizeof(m_DaylenMax));
	fin.read((char *)&m_MaxGSI, sizeof(m_MaxGSI));
	fin.read((char *)&m_GreenupThreshold, sizeof(m_GreenupThreshold));
	fin.read((char *)&m_MaxLFMVal, sizeof(m_MaxLFMVal));
	fin.read((char *)&m_MinLFMVal, sizeof(m_MinLFMVal));
	fin.read((char *)&m_Slope, sizeof(m_Slope));
	fin.read((char *)&m_Intercept, sizeof(m_Intercept));
	fin.read((char *)&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear));
	fin.read((char *)&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear));
	fin.read((char *)&m_canIncreaseHerb, sizeof(m_canIncreaseHerb));
	fin.read((char *)&lastHerbFM, sizeof(lastHerbFM));
	short qSize = m_qGSI.size();
	fin.read((char *)&qSize, sizeof(qSize));
	
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal = m_qGSI[i];
		fin.read((char *)&tVal, sizeof(tVal));

	}
	return true;
}
