CXX = g++
CXXFLAGS = -g -O3 -Wall

PREDICTORS_DIR := predictors
PIECEWISE_DIR := ${PREDICTORS_DIR}/piecewise
TAGE_DIR := ${PREDICTORS_DIR}/tage

TRACE_DIR := ../traces

all: predict

piecewise: ${PIECEWISE_DIR}

tage:

predict: piecewise predict.cc trace.cc predictor.h branch.h trace.h 
	$(CXX) $(CXXFLAGS) -o predict predict.cc trace.cc

test: predict
	./predict ${TRACE_DIR}/164.gzip/gzip.trace.bz2

# Run a specific test
# example:
# make 164.gzip
# %: predict
# 	./predict ../traces/$@


testing: branch.h trace.h
	@echo "Testing"
	@echo "filename target (\$\@): $@"
	@echo "first prerequisites (\$\<): $<"
	@echo "all prerequisites (\$\^): $^"

clean:
	rm -f predict
