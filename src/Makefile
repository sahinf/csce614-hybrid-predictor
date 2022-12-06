CXX = g++
CXXFLAGS = -g -O3 -Wall

PREDICTORS_DIR := predictors
PIECEWISE_DIR := ${PREDICTORS_DIR}/piecewise
TAGE_DIR := ${PREDICTORS_DIR}/tage

PIECEWISE_FILES := ${PIECEWISE_DIR}/*
TAGE_FILES := ${TAGE_DIR}/*

TRACE_DIR := ../traces

all: predict

predict: ${PIECEWISE_FILES} ${TAGE_FILES} predict.cc trace.cc predictor.h branch.h trace.h my_predictor.h
	$(CXX) $(CXXFLAGS) -o predict predict.cc trace.cc ${PIECEWISE_DIR}/piecewise_predictor.cc ${TAGE_DIR}/TAGEPredictor.cc

run: predict
	@cd ../ && ./run traces

test: predict
	./predict ${TRACE_DIR}/164.gzip/gzip.trace.bz2

testing: branch.h trace.h
	@echo "Testing"
	@echo "filename target (\$\@): $@"
	@echo "first prerequisites (\$\<): $<"
	@echo "all prerequisites (\$\^): $^"

clean:
	rm -f predict
