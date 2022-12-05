CXX = g++
CXXFLAGS = -g -O3 -Wall

all: predict

predict: predict.cc trace.cc predictor.h branch.h trace.h my_predictor.h
	$(CXX) $(CXXFLAGS) -o predict predict.cc trace.cc

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
