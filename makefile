INCLUDES=  -I ../FunctionalUtilities  -I ../HullWhite -I../GaussNewton -I../TupleUtilities -I../AutoDiff -I../nelder_mead -I ../levy-functions -I../CharacteristicFunctions -I../RungeKutta -I../FFTOptionPricing -I../FangOost -I../cuckoo_search -I../MonteCarlo
GCCVAL=g++


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCVAL=g++-7
endif

test:test.o
	${GCCVAL} -std=c++14 -O3 -pthread --coverage test.o $(INCLUDES) -o test -fopenmp
test.o:test.cpp BSImpliedVolatility.h
	${GCCVAL} -std=c++14 -O3 -pthread --coverage -c test.cpp $(INCLUDES) -fopenmp
clean:
	-rm *.o *.out test