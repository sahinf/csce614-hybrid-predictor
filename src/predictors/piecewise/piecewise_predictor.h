#ifndef PIECEWISE_PREDICTOR_H
#define PIECEWISE_PREDICTOR_H


#define n 8
#define m 288	
#define h 50
#define theta 2.14*(h+1) + 20.58

class Piecewise
{
private:
	unsigned int W[n][m][h+1];
	unsigned int GA[h+1];
	unsigned int GHR[h+1];
	int last_output;
public:
    Piecewise();
    bool predict(unsigned int address);
	void update(unsigned int address,bool direction,bool taken, unsigned int target);
	int getLastOutput() {return last_output;}
};
#endif