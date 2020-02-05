#include "deadfuelmoisture.h"
#include "livefuelmoisture.h"
#include "NFDR2016CalcState.h"
#include "nfdrs2016.h"
#include <iostream>

using namespace std;

int main(void)
{
	double fSC, fERC, fBI, fIC;
	// Make an instance of the NFDRS2016 class
	NFDR2016Calc nfdrs2016(45.0, 'Y',1,12,0,0,0);
	
	// Set the fuel moistures
	nfdrs2016.iSetFuelMoistures (3,4,5,6,60,90,25);
	// Calc the indices
	nfdrs2016.iCalcIndexes(5, 1, &fSC, &fERC, &fBI, &fIC);
	// Print out the ERC (both the class member and the pointer value)	
	cout << nfdrs2016.ERC << " " << fERC << endl;

	// Create a State class
	NFDR2016CalcState nfdrs2016state(&nfdrs2016);
	// Write the state
	nfdrs2016state.SaveState("Test.state");

	// Read the state
	nfdrs2016state.LoadState("Test.state");
	
	// Create a NEW instance of the NFDR clas
	NFDR2016Calc nfdrsnew(45.0, 'Y',1,12,0,0,0);
	
	// Set the fuel moistures
	nfdrsnew.iSetFuelMoistures (5,6,7,8,60,90,25);
	
	// Calc the indices
	nfdrsnew.iCalcIndexes(5, 1, &fSC, &fERC, &fBI, &fIC);
	cout << nfdrsnew.ERC << endl;
	// Load the State Back
	nfdrsnew.LoadState(nfdrs2016state);
	cout << nfdrsnew.ERC << endl;

	ofstream fout("Test.bin", ios::binary);
	
	for(int x = 0; x < 2345; x++)
	{
		if((x % 100) == 0) cout << ((float) x / 2345) * 100 << endl;
		for(int y = 0; y < 1597; y++)
		{
		
			nfdrs2016state.SaveState(fout);
		}
	}
	fout.close();
	ifstream fin("Test.bin", ios::binary);
	
	for(int x = 0; x < 2345; x++)
	{
		if((x % 100) == 0) cout << ((float) x / 2345) * 100 << endl;
		for(int y = 0; y < 1597; y++)
		{
		
			nfdrs2016state.LoadState(fin);
		}
	}

	fin.close();
	NFDR2016Calc nfdrsnewgrid(45.0, 'Y',1,12,0,0,0);
	cout << nfdrsnewgrid.ERC << endl;
	nfdrsnewgrid.LoadState(nfdrs2016state);
	cout << nfdrsnewgrid.ERC << endl;
	return EXIT_SUCCESS;

}
