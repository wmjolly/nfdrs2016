#include "DFMCalcState.h"
#include "fstream"

using namespace std;

DFMCalcState::DFMCalcState()
{
}

DFMCalcState::DFMCalcState(const DFMCalcState &rhs)
{
	m_JDay = rhs.m_JDay;
	m_Year = rhs.m_Year;
	m_Month = rhs.m_Month;
	m_Day = rhs.m_Day;
	m_Hour = rhs.m_Hour;
	m_Min = rhs.m_Min;
	m_Sec = rhs.m_Sec;
	m_obstime = rhs.m_obstime;

	m_bp1 = rhs.m_bp1;
	m_et = rhs.m_et;
	m_ha1 = rhs.m_ha1;
	m_rc1 = rhs.m_rc1;
	m_sv1 = rhs.m_sv1;
	m_ta1 = rhs.m_ta1;
	m_hf = rhs.m_hf;
	m_wsa = rhs.m_wsa;

	m_rdur = rhs.m_rdur;
	m_ra1 = rhs.m_ra1;

	m_nodes = rhs.m_nodes;
	m_t = rhs.m_t;
	m_s = rhs.m_s;
	//m_d = rhs.m_d;
	m_w = rhs.m_w;
}


DFMCalcState::~DFMCalcState()
{
}

bool DFMCalcState::ReadState(FILE *in)
{
	size_t nRead = fread(&m_JDay, sizeof(m_JDay), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Year, sizeof(m_Year), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Month, sizeof(m_Month), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Day, sizeof(m_Day), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Hour, sizeof(m_Hour), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Min, sizeof(m_Min), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Sec, sizeof(m_Sec), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_obstime, sizeof(m_obstime), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_bp1, sizeof(m_bp1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_et, sizeof(m_et), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_ha1, sizeof(m_ha1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_rc1, sizeof(m_rc1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_sv1, sizeof(m_sv1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_ta1, sizeof(m_ta1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_hf, sizeof(m_hf), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_wsa, sizeof(m_wsa), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_rdur, sizeof(m_rdur), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_ra1, sizeof(m_ra1), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_nodes, sizeof(m_nodes), 1, in);
	if (nRead != 1)
		return false;
	if (m_nodes > 0)
	{
		FP_STORAGE_TYPE tVal;
		for (int i = 0; i < m_nodes; i++)
		{
			nRead = fread(&tVal, sizeof(tVal), 1, in);
			if (nRead != 1)
				return false;
			m_t.push_back(tVal);
		}
		for (int i = 0; i < m_nodes; i++)
		{
			nRead = fread(&tVal, sizeof(tVal), 1, in);
			if (nRead != 1)
				return false;
			m_s.push_back(tVal);
		}
/*
		for (int i = 0; i < m_nodes; i++)
		{
			nRead = fread(&tVal, sizeof(tVal), 1, in);
			if (nRead != 1)
				return false;
			m_d.push_back(tVal);
		}*/
		for (int i = 0; i < m_nodes; i++)
		{
			nRead = fread(&tVal, sizeof(tVal), 1, in);
			if (nRead != 1)
				return false;
			m_w.push_back(tVal);
		}
	}
	return true;
}

bool DFMCalcState::SaveState(FILE *out)
{
	size_t nWrite = fwrite(&m_JDay, sizeof(m_JDay), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Year, sizeof(m_Year), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Month, sizeof(m_Month), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Day, sizeof(m_Day), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Hour, sizeof(m_Hour), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Min, sizeof(m_Min), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Sec, sizeof(m_Sec), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_obstime, sizeof(m_obstime), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_bp1, sizeof(m_bp1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_et, sizeof(m_et), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_ha1, sizeof(m_ha1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_rc1, sizeof(m_rc1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_sv1, sizeof(m_sv1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_ta1, sizeof(m_ta1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_hf, sizeof(m_hf), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_wsa, sizeof(m_wsa), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_rdur, sizeof(m_rdur), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_ra1, sizeof(m_ra1), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_nodes, sizeof(m_nodes), 1, out);
	if (nWrite != 1)
		return false;
	if (m_nodes > 0)
	{
		FP_STORAGE_TYPE tVal;
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_t[i];
			nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
			if (nWrite != 1)
				return false;
		}
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_s[i];
			nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
			if (nWrite != 1)
				return false;
		}
		/*
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_d[i];
			nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
			if (nWrite != 1)
				return false;
		}
		*/
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_w[i];
			nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
			if (nWrite != 1)
				return false;
		}
	}

	return true;
}

bool DFMCalcState::SaveState(ofstream& fout)
{
	fout.write((char *)&m_JDay, sizeof(m_JDay));
	fout.write((char *)&m_Year, sizeof(m_Year));
	fout.write((char *)&m_Month, sizeof(m_Month));
	fout.write((char *)&m_Day, sizeof(m_Day));
	fout.write((char *)&m_Hour, sizeof(m_Hour));
	fout.write((char *)&m_Min, sizeof(m_Min));
	fout.write((char *)&m_Sec, sizeof(m_Sec));
	fout.write((char *)&m_obstime, sizeof(m_obstime));
	fout.write((char *)&m_bp1, sizeof(m_bp1));
	fout.write((char *)&m_et, sizeof(m_et));
	fout.write((char *)&m_ha1, sizeof(m_ha1));
	fout.write((char *)&m_rc1, sizeof(m_rc1));
	fout.write((char *)&m_sv1, sizeof(m_sv1));
	fout.write((char *)&m_ta1, sizeof(m_ta1));
	fout.write((char *)&m_hf, sizeof(m_hf));
	fout.write((char *)&m_wsa, sizeof(m_wsa));
	fout.write((char *)&m_rdur, sizeof(m_rdur));
	fout.write((char *)&m_ra1, sizeof(m_ra1));
	fout.write((char *)&m_nodes, sizeof(m_nodes));
	if (m_nodes > 0)
	{
		FP_STORAGE_TYPE tVal;
		for (int i = 0; i < m_nodes; i++)
		{
			//tVal = m_t[i];
				fout.write((char *)&m_t[i], sizeof(tVal));

		}
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_s[i];
			fout.write((char *)&m_s[i], sizeof(tVal));
			
		}
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_w[i];
			fout.write((char *)&m_w[i], sizeof(tVal));
			
		}
	}

	return true;

}
bool DFMCalcState::ReadState(ifstream& fin)
{
	fin.read((char *)&m_JDay, sizeof(m_JDay));
	fin.read((char *)&m_Year, sizeof(m_Year));
	fin.read((char *)&m_Month, sizeof(m_Month));
	fin.read((char *)&m_Day, sizeof(m_Day));
	fin.read((char *)&m_Hour, sizeof(m_Hour));
	fin.read((char *)&m_Min, sizeof(m_Min));
	fin.read((char *)&m_Sec, sizeof(m_Sec));
	fin.read((char *)&m_obstime, sizeof(m_obstime));
	fin.read((char *)&m_bp1, sizeof(m_bp1));
	fin.read((char *)&m_et, sizeof(m_et));
	fin.read((char *)&m_ha1, sizeof(m_ha1));
	fin.read((char *)&m_rc1, sizeof(m_rc1));
	fin.read((char *)&m_sv1, sizeof(m_sv1));
	fin.read((char *)&m_ta1, sizeof(m_ta1));
	fin.read((char *)&m_hf, sizeof(m_hf));
	fin.read((char *)&m_wsa, sizeof(m_wsa));
	fin.read((char *)&m_rdur, sizeof(m_rdur));
	fin.read((char *)&m_ra1, sizeof(m_ra1));
	fin.read((char *)&m_nodes, sizeof(m_nodes));
	if (m_nodes > 0)
	{
		FP_STORAGE_TYPE tVal;
		for (int i = 0; i < m_nodes; i++)
		{
			//tVal = m_t[i];
				fin.read((char *)&m_t[i], sizeof(tVal));

		}
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_s[i];
			fin.read((char *)&m_s[i], sizeof(tVal));
			
		}
		for (int i = 0; i < m_nodes; i++)
		{
			tVal = m_w[i];
			fin.read((char *)&m_w[i], sizeof(tVal));
			
		}
	}

	return true;
}
