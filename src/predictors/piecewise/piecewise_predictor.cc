/* Based on the psuedo code from 
Jimenez, D.A. “Piecewise Linear Branch Prediction.” 32nd International Symposium on Computer Architecture (ISCA'05), 4 June 2005,
 https://doi.org/10.1109/isca.2005.40.*/

#include "piecewise_predictor.h"
#include <string.h>
#include <cmath>
Piecewise::Piecewise()
{
    memset (GA,0,sizeof(GA));
	memset (GHR,0,sizeof(GHR));
	memset (W,0,sizeof(W));
}

bool Piecewise::predict(unsigned int address)
{
    int output = W[address%n][0][0];
	bool predict=0;
	for(unsigned int i=1;i<=h;i++)
	{
		if(GHR[i] == true)
		{
			output += W[address % n][GA[i] % m][i];
		}
		else {
			output -= W[address % n][GA[i] % m][i];
		}

	}
	if (output >= 0)
	{
		predict = 1;
	}
	last_output = output;
    return predict;
}

void Piecewise::update(unsigned int address,bool direction,bool taken, unsigned int target)
{
    int output = W[address % n][0][0];

    if (taken != direction || std::abs(output) < theta)
		{
			if(taken == true)
			{
				W[address % n][0][0] += 1;
			}
			else{
				W[address % n][0][0] -= 1;
			}
			for(unsigned int i=1;i<h+1;i++)
			{
				if(GHR[i] == taken)
				{
					W[address % n][GA[i] % m][i] += 1;
				}
				else
				{
					W[address % n][GA[i] % m][i] -= 1;
				}
			}
		}
			
		memmove(&GA[2],&GA[1],(h-1)*sizeof(int));
		GA[1] = address;
		memmove(&GHR[2],&GHR[1],(h-1)*sizeof(int));
		GHR[1]= taken;
}