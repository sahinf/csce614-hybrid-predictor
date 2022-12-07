// my_predictor.h
// This file contains a sample my_predictor class.
// It has a simple 32,768-entry gshare with a history length of 15 and a
// simple direct-mapped branch target buffer for indirect branch prediction.

//https://github.com/taraeicher/HybridBranchPredictor source for choice table perceptron

#include "predictors/tage/TAGEPredictor.h"
#include "predictors/piecewise/piecewise_predictor.h"

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	15
#define TABLE_BITS	15
	my_update u;
	branch_info bi;
	unsigned int history;
	unsigned char tab[1<<TABLE_BITS];
	unsigned int targets[1<<TABLE_BITS];
	int choiceTable[25000];
	bitset<20> GHR;
	int tage_or_pwl; // tage used = 1, pwl used = 0
	bool equalPredictions;
	PREDICTOR* tage;
	Piecewise* pwl;
	int pwl_miss;
	int tage_miss;

	my_predictor (void) : history(0) { 
		memset (tab, 0, sizeof (tab));
		memset (targets, 0, sizeof (targets));
		GHR = bitset<20>();
		tage_or_pwl = 0;
		tage = new PREDICTOR();
		pwl = new Piecewise();
		equalPredictions = 0;
		pwl_miss = 0;
		tage_miss = 0;
		  // Initialize the choice table.
	for(UINT32 iii=0; iii < 25000; iii++){
		choiceTable[iii] = 0;
	}

	}

	branch_update *predict (branch_info & b) {

		//equalPredictions = 0;


		if(b.br_flags & BR_CONDITIONAL)
		{
			u.index = (history << (TABLE_BITS - HISTORY_LENGTH)) ^ (b.address & ((1<<TABLE_BITS)-1));
            int gshare_predict = tab[u.index] >> 1;

			bool tage_predict= tage->GetPrediction(b.address);

			bool piecewise_predict = pwl->predict(b.address);
			int piecewise_confidence = pwl->getLastOutput();



			equalPredictions = 0;
			if(!(tage_predict ^ piecewise_predict))
			{
				//both predicted the same thing dont need to choose
				u.direction_prediction(tage_predict);
				equalPredictions = 1;
				tage_or_pwl = 1;
			}
			else // predictions differ
			{
				int tableIdx = b.address % 25000;// HashPC(b.address,16000);
				if(gshare_predict > 0 and piecewise_predict > 0 and abs(tage_predict) < 3)
				{
					tage_or_pwl = 0;
					u.direction_prediction(piecewise_predict);
				}
				else if(choiceTable[tableIdx] < 2 or piecewise_confidence < 100)
				{
					tage_or_pwl = 1;
					u.direction_prediction(tage_predict);
				}
				else{
					tage_or_pwl = 0;
					u.direction_prediction(piecewise_predict);
				}
				/*
				if(1)
				{
					tage_or_pwl = 0;
					u.direction_prediction(piecewise_predict);
				}
				else
				{
					tage_or_pwl = 1;
					u.direction_prediction(tage_predict);
				}*/
				//tage_or_pwl = 1;
				//u.direction_prediction(tage_predict);
			
				/*
				if(tage->getChoseBasic())
				{
					u.direction_prediction(tage_predict);
					tage_or_pwl = 1;
				}
				else if(abs(piecewise_confidence) < 20)
				{
					u.direction_prediction(tage_predict);
					tage_or_pwl = 1;
				}
				
				else{
					u.direction_prediction(piecewise_predict);
					tage_or_pwl = 0;
				}

				if(tage_or_pwl) {tage_preds+=1;} else {pwl_preds +=1;}

				if (tage_preds > 100 and pwl_preds > 100)
				{
					if(tage_miss/tage_preds > pwl_miss/pwl_preds)
					{
						tage_or_pwl = 1;
						u.direction_prediction(tage_predict);
					}
					else
					{
						tage_or_pwl = 0;
						u.direction_prediction(piecewise_predict);
					}
				}
				*/
				
			}

		}
		else {
			u.direction_prediction (true);
		}

		bi = b;

		if (b.br_flags & BR_INDIRECT) {
			u.target_prediction (targets[b.address & ((1<<TABLE_BITS)-1)]);
			//u.target_prediction (0);
		}
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			//update gshare
            unsigned char *c = &tab[((my_update*)u)->index];
            if (taken) {if (*c < 3) (*c)++;} else {if (*c > 0) (*c)--;}
                history <<= 1;
                history |= taken;
                history &= (1<<HISTORY_LENGTH)-1;

			int tableIdx = bi.address % 25000;//HashPC(bi.address,16000);
			if((taken != u->direction_prediction()) and choiceTable[tableIdx] < 2)
			{
				if(choiceTable[tableIdx < 128])
				{
					choiceTable[tableIdx] += 1;
				}
				
			}
			else if((taken != u->direction_prediction()) and choiceTable[tableIdx] >= 2)
			{
				if(choiceTable[tableIdx > -128])
				{
					choiceTable[tableIdx] -= 1;
				}
			}
			
			if(taken == u->direction_prediction() and choiceTable[tableIdx] < 2)
			{
				if(choiceTable[tableIdx > -128])
				{
					choiceTable[tableIdx] -= 1;
				}
			}
			else if (taken == u->direction_prediction() and choiceTable[tableIdx] >= 2)
			{
				if(choiceTable[tableIdx < 128])
				{
					choiceTable[tableIdx] += 1;
				}
			}
			/*if((choiceTable[tableIdx][1] == 0 and u->target_prediction() == taken) or (choiceTable[tableIdx][1] = 1 and taken != u->target_prediction()))
			{
				if(choiceTable[tableIdx][1] == 1 && choiceTable[tableIdx][0] == 1){
					choiceTable[tableIdx].set(0,0);
				}
				else if(choiceTable[tableIdx][1] == 1 && choiceTable[tableIdx][0] == 0){
					choiceTable[tableIdx].set(0,1);
					choiceTable[tableIdx].set(1,0);
				}
				else if(choiceTable[tableIdx][1] == 0 && choiceTable[tableIdx][0] == 1){
					choiceTable[tableIdx].set(0,0);
		 		}
			}
			else{
					  if ((choiceTable[tableIdx][1] == 1 && u->target_prediction() == taken) || (choiceTable[tableIdx][1] == 0 && u->target_prediction() != taken)){
						if(choiceTable[tableIdx][1] == 1 && choiceTable[tableIdx][0] == 0){
							choiceTable[tableIdx].set(0,1);
						}
						else if(choiceTable[tableIdx][1] == 0 && choiceTable[tableIdx][0] == 1){
							choiceTable[tableIdx].set(1,1);
							choiceTable[tableIdx].set(0,0);
						}
						else if(choiceTable[tableIdx][1] == 0 && choiceTable[tableIdx][0] == 0){
							choiceTable[tableIdx].set(0,1);
						}
					}
			}*/
			// we should consider updating both ???
			if(equalPredictions)
			{
				tage->UpdatePredictor(bi.address,taken,u->direction_prediction(),target);
				pwl->update(bi.address,u->direction_prediction(),taken,target);
			}
			else if(tage_or_pwl)
			{
				if(taken != u->direction_prediction()) {tage_miss+=1;}
				tage->UpdatePredictor(bi.address,taken,u->direction_prediction(),target);
			}
			else{
				if(taken != u->direction_prediction()) {pwl_miss +=1;}
				pwl->update(bi.address,u->direction_prediction(),taken,target);
			}
		}
		if (bi.br_flags & BR_INDIRECT) {
			targets[bi.address & ((1<<TABLE_BITS)-1)] = target;
		}
	}
	UINT32 HashPC(UINT32 PC, UINT32 hashLimit){
	
	// Hash the PC so that it can be used as an index for the perceptron table.
		
		UINT32 PCend = PC % hashLimit;
		UINT32 ghrend = ((UINT32)GHR.to_ulong()) % hashLimit;
		return PCend ^ ghrend;
	}
};
