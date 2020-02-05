#include "NFDR2016CalcState.h"
#include "nfdrs2016.h"


NFDR2016CalcState::NFDR2016CalcState()
{
}


NFDR2016CalcState::~NFDR2016CalcState()
{
}

NFDR2016CalcState::NFDR2016CalcState(const NFDR2016CalcState& rhs)
{
	m_NFDRSVersion = rhs.m_NFDRSVersion;
	fm1State = rhs.fm1State;
	fm10State = rhs.fm10State;
	fm100State = rhs.fm100State;
	fm1000State = rhs.fm1000State;
	gsiState = rhs.gsiState;
	herbState = rhs.herbState;
	woodyState = rhs.woodyState;
	m_Lat = rhs.m_Lat;
	m_num_updates = rhs.m_num_updates;
	m_YesterdayJDay = rhs.m_YesterdayJDay;
	m_SlopeClass = rhs.m_SlopeClass;
	m_FuelModel = rhs.m_FuelModel;
	//m_MC1 = rhs.m_MC1;
	//m_MC10 = rhs.m_MC10;
	//m_MC100 = rhs.m_MC100;
	//m_MC1000 = rhs.m_MC1000;
	//m_MCHERB = rhs.m_MCHERB;
	//m_MCWOOD = rhs.m_MCWOOD;
	m_PrevYear = rhs.m_PrevYear;
	m_KBDI = rhs.m_KBDI;
	m_YKBDI = rhs.m_YKBDI;
	m_StartKBDI = rhs.m_StartKBDI;
	m_CummPrecip = rhs.m_CummPrecip;
	m_AvgPrecip = rhs.m_AvgPrecip;
	m_UseCuring = rhs.m_UseCuring;
	m_UseLoadTransfer = rhs.m_UseLoadTransfer;
	m_FuelTemperature = rhs.m_FuelTemperature;
	m_BI = rhs.m_BI;
	m_ERC = rhs.m_ERC;
	m_SC = rhs.m_SC;
	m_IC = rhs.m_IC;
	m_GSI = rhs.m_GSI;
	m_nConsectiveSnowDays = rhs.m_nConsectiveSnowDays;
	m_lastUpdateTime = rhs.m_lastUpdateTime;
	/*m_qPrecip = rhs.m_qPrecip;*/

}

NFDR2016CalcState::NFDR2016CalcState(NFDR2016Calc *pNFDRS)
{
	m_AvgPrecip = pNFDRS->AvgPrecip;
	m_BI = pNFDRS->BI;
	m_CummPrecip = pNFDRS->CummPrecip;
	m_ERC = pNFDRS->ERC;
	m_FuelModel = pNFDRS->FuelModel;
	m_FuelTemperature = pNFDRS->FuelTemperature;
	m_GSI = pNFDRS->m_GSI;
	m_IC = pNFDRS->IC;
	m_KBDI = pNFDRS->KBDI;
	m_lastUpdateTime = pNFDRS->lastUpdateTime;
	m_Lat = pNFDRS->Lat;
	//m_MC1 = pNFDRS->MC1;
	//m_MC10 = pNFDRS->MC10;
	//m_MC100 = pNFDRS->MC100;
	//m_MC1000 = pNFDRS->MC1000;
	//m_MCHERB = pNFDRS->MCHERB;
	//m_MCWOOD = pNFDRS->MCWOOD;
	m_nConsectiveSnowDays = pNFDRS->nConsectiveSnowDays;
	m_NFDRSVersion = pNFDRS->NFDRSVersion;
	m_num_updates = pNFDRS->num_updates;
	m_PrevYear = pNFDRS->PrevYear;
	m_SC = pNFDRS->SC;
	m_SlopeClass = pNFDRS->SlopeClass;
	m_StartKBDI = pNFDRS->StartKBDI;
	m_UseCuring = pNFDRS->UseCuring;
	m_UseLoadTransfer = pNFDRS->UseLoadTransfer;
	m_YesterdayJDay = pNFDRS->YesterdayJDay;
	m_YKBDI = pNFDRS->YKBDI;
	/*float tVal;
	std::vector<float> qCopy = pNFDRS->qPrecip;
	while(qCopy.size() > 0)
	{
		tVal = qCopy.front();
		m_qPrecip.push_back(tVal);
		qCopy.pop_back();
	}*/
	fm1State = pNFDRS->OneHourFM.GetState();
	fm10State = pNFDRS->TenHourFM.GetState();
	fm100State = pNFDRS->HundredHourFM.GetState();
	fm1000State = pNFDRS->ThousandHourFM.GetState();
	gsiState = pNFDRS->GsiFM.GetState();
	herbState = pNFDRS->HerbFM.GetState();
	woodyState = pNFDRS->WoodyFM.GetState();
}


bool NFDR2016CalcState::LoadState(std::string fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");
	if (!in)
		return false;

	bool status;
	int nRead = fread(&m_NFDRSVersion, sizeof(m_NFDRSVersion), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}

	status = fm1State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm10State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm100State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm1000State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = gsiState.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = herbState.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = woodyState.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}

	nRead = fread(&m_Lat, sizeof(m_Lat), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_num_updates, sizeof(m_num_updates), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_YesterdayJDay, sizeof(m_YesterdayJDay), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_SlopeClass, sizeof(m_SlopeClass), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_FuelModel, sizeof(m_FuelModel), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
/*
	nRead = fread(&m_MC1, sizeof(m_MC1), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC10, sizeof(m_MC10), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC100, sizeof(m_MC100), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC1000, sizeof(m_MC1000), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MCWOOD, sizeof(m_MCWOOD), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MCHERB, sizeof(m_MCHERB), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
*/
	nRead = fread(&m_PrevYear, sizeof(m_PrevYear), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_KBDI, sizeof(m_KBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_YKBDI, sizeof(m_YKBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_StartKBDI, sizeof(m_StartKBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_CummPrecip, sizeof(m_CummPrecip), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_AvgPrecip, sizeof(m_AvgPrecip), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_UseLoadTransfer, sizeof(m_UseLoadTransfer), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_UseCuring, sizeof(m_UseCuring), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_FuelTemperature, sizeof(m_FuelTemperature), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_BI, sizeof(m_BI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_ERC, sizeof(m_ERC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_SC, sizeof(m_SC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_IC, sizeof(m_IC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_GSI, sizeof(m_GSI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	/*
	int nPcp;
	nRead = fread(&nPcp, sizeof(nPcp), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	for (int i = 0; i < nPcp; i++)
	{
		float tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
		{
			fclose(in);
			return false;
		}
		m_qPrecip.push_back(tVal);
	}
	*/
	fclose(in);
	return true;
}

bool NFDR2016CalcState::SaveState(std::string fileName)
{
	FILE *out = fopen(fileName.c_str(), "wb");
	if (!out)
		return false;

	bool status;
	//write version first
	int nWrite = fwrite(&m_NFDRSVersion, sizeof(m_NFDRSVersion), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	status = fm1State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm10State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm100State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm1000State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = gsiState.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = herbState.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = woodyState.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_Lat, sizeof(m_Lat), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_num_updates, sizeof(m_num_updates), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_YesterdayJDay, sizeof(m_YesterdayJDay), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_SlopeClass, sizeof(m_SlopeClass), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_FuelModel, sizeof(m_FuelModel), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
/*
	nWrite = fwrite(&m_MC1, sizeof(m_MC1), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC10, sizeof(m_MC10), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC100, sizeof(m_MC100), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC1000, sizeof(m_MC1000), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MCWOOD, sizeof(m_MCWOOD), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MCHERB, sizeof(m_MCHERB), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
*/
	nWrite = fwrite(&m_PrevYear, sizeof(m_PrevYear), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_KBDI, sizeof(m_KBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_YKBDI, sizeof(m_YKBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_StartKBDI, sizeof(m_StartKBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_CummPrecip, sizeof(m_CummPrecip), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_AvgPrecip, sizeof(m_AvgPrecip), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_UseLoadTransfer, sizeof(m_UseLoadTransfer), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_UseCuring, sizeof(m_UseCuring), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_FuelTemperature, sizeof(m_FuelTemperature), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_BI, sizeof(m_BI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_ERC, sizeof(m_ERC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_SC, sizeof(m_SC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_IC, sizeof(m_IC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_GSI, sizeof(m_GSI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}

	/*
	int nPcp = m_qPrecip.size();
	nWrite = fwrite(&nPcp, sizeof(nPcp), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	for (int i = 0; i < nPcp; i++)
	{
		float tVal = m_qPrecip[i];
		nWrite = fwrite(&tVal, sizeof(float), 1, out);
		if (nWrite != 1)
			return false;
	}
	*/

	fclose(out);
	return true;
}

bool NFDR2016CalcState::SaveState(ofstream& fout)
{




	fout.write( (char *) &m_NFDRSVersion, sizeof(m_NFDRSVersion));
	fm1State.SaveState(fout);
	fm10State.SaveState(fout);
	fm100State.SaveState(fout);
	fm1000State.SaveState(fout);
	gsiState.SaveState(fout);
	herbState.SaveState(fout);
	woodyState.SaveState(fout);
	fout.write( (char *)&m_Lat, sizeof(m_Lat));
	fout.write( (char *)&m_num_updates, sizeof(m_num_updates));
	fout.write( (char *)&m_YesterdayJDay, sizeof(m_YesterdayJDay));
	fout.write( (char *)&m_SlopeClass, sizeof(m_SlopeClass));
	fout.write( (char *)&m_FuelModel, sizeof(m_FuelModel));
	fout.write( (char *)&m_PrevYear, sizeof(m_PrevYear));
	fout.write( (char *)&m_KBDI, sizeof(m_KBDI));
	fout.write( (char *)&m_YKBDI, sizeof(m_YKBDI));
	fout.write( (char *)&m_StartKBDI, sizeof(m_StartKBDI));
	fout.write( (char *)&m_CummPrecip, sizeof(m_CummPrecip));
	fout.write( (char *)&m_AvgPrecip, sizeof(m_AvgPrecip));
	fout.write( (char *)&m_UseLoadTransfer, sizeof(m_UseLoadTransfer));
	fout.write( (char *)&m_UseCuring, sizeof(m_UseCuring));
	fout.write( (char *)&m_FuelTemperature, sizeof(m_FuelTemperature));
	fout.write( (char *)&m_BI, sizeof(m_BI));
	fout.write( (char *)&m_ERC, sizeof(m_ERC));
	fout.write( (char *)&m_SC, sizeof(m_SC));
	fout.write( (char *)&m_IC, sizeof(m_IC));
	fout.write( (char *)&m_GSI, sizeof(m_GSI));
	fout.write( (char *)&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays));
	fout.write( (char *)&m_lastUpdateTime, sizeof(m_lastUpdateTime));
	
	return true;
}


bool NFDR2016CalcState::LoadState(ifstream& fin)
{

	fin.read( (char *) &m_NFDRSVersion, sizeof(m_NFDRSVersion));
	fm1State.ReadState(fin);
	fm10State.ReadState(fin);
	fm100State.ReadState(fin);
	fm1000State.ReadState(fin);
	gsiState.ReadState(fin);
	herbState.ReadState(fin);
	woodyState.ReadState(fin);
	fin.read( (char *)&m_Lat, sizeof(m_Lat));
	fin.read( (char *)&m_num_updates, sizeof(m_num_updates));
	fin.read ((char *)&m_YesterdayJDay, sizeof(m_YesterdayJDay));
	fin.read( (char *)&m_SlopeClass, sizeof(m_SlopeClass));
	fin.read( (char *)&m_FuelModel, sizeof(m_FuelModel));
	fin.read( (char *)&m_PrevYear, sizeof(m_PrevYear));
	fin.read( (char *)&m_KBDI, sizeof(m_KBDI));
	fin.read( (char *)&m_YKBDI, sizeof(m_YKBDI));
	fin.read( (char *)&m_StartKBDI, sizeof(m_StartKBDI));
	fin.read( (char *)&m_CummPrecip, sizeof(m_CummPrecip));
	fin.read( (char *)&m_AvgPrecip, sizeof(m_AvgPrecip));
	fin.read( (char *)&m_UseLoadTransfer, sizeof(m_UseLoadTransfer));
	fin.read( (char *)&m_UseCuring, sizeof(m_UseCuring));
	fin.read( (char *)&m_FuelTemperature, sizeof(m_FuelTemperature));
	fin.read( (char *)&m_BI, sizeof(m_BI));
	fin.read( (char *)&m_ERC, sizeof(m_ERC));
	fin.read( (char *)&m_SC, sizeof(m_SC));
	fin.read ((char *)&m_IC, sizeof(m_IC));
	fin.read( (char *)&m_GSI, sizeof(m_GSI));

	fin.read( (char *)&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays));
	fin.read( (char *)&m_lastUpdateTime, sizeof(m_lastUpdateTime));
	return true;
}

