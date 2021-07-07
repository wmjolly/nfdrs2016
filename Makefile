CC=g++
CFLAGS=-g -fopenmp -I/usr/local/include 
DEPS = nfdrs2016.h
TARGET = nfdrs2016

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nfdrs2016: main.cpp nfdrs2016.o utctime.o deadfuelmoisture.o livefuelmoisture.o DFMCalcState.o LFMCalcState.o NFDR2016CalcState.o
	$(CC) $(CFLAGS) main.cpp nfdrs2016.o utctime.o deadfuelmoisture.o livefuelmoisture.o DFMCalcState.o LFMCalcState.o NFDR2016CalcState.o -o $@ -L/usr/local/lib -I/usr/local/include -lnetcdf_c++4 -lnetcdf

nfdrs2016.o: nfdrs2016.cpp nfdrs2016.h utctime.h
	$(CC) $(CFLAGS) -c nfdrs2016.cpp

deadfuelmoisture.o: deadfuelmoisture.cpp deadfuelmoisture.h
	$(CC) $(CFLAGS) -c deadfuelmoisture.cpp

utctime.o: utctime.cpp utctime.h
	$(CC) $(CFLAGS) -c utctime.cpp

livefuelmoisture.o: livefuelmoisture.cpp livefuelmoisture.h
	$(CC) $(CFLAGS) -c livefuelmoisture.cpp

DFMCalcState.o: DFMCalcState.cpp DFMCalcState.h
	$(CC) $(CFLAGS) -c DFMCalcState.cpp

LFMCalcState.o: LFMCalcState.cpp LFMCalcState.h
	$(CC) $(CFLAGS) -c LFMCalcState.cpp

NFDR2016CalcState.o: NFDR2016CalcState.cpp NFDR2016CalcState.h
	$(CC) $(CFLAGS) -c NFDR2016CalcState.cpp
clean:
	rm -f *.o core



