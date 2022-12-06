// my_predictor.h
// This file contains a sample my_predictor class.
// It has a simple 32,768-entry gshare with a history length of 15 and a
// simple direct-mapped branch target buffer for indirect branch prediction.
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
	int tage_or_pwl = 0; // tage used = 1, pwl used = 0
	bool equalPredictions = 0;
	PREDICTOR* tage = new PREDICTOR();
	Piecewise* pwl = new Piecewise();
	my_predictor (void) : history(0) { 
		memset (tab, 0, sizeof (tab));
		memset (targets, 0, sizeof (targets));

	}

	branch_update *predict (branch_info & b) {

		//equalPredictions = 0;
		if(b.br_flags & BR_CONDITIONAL)
		{
			bool tage_predict= tage->GetPrediction(b.address);

			bool piecewise_predict = pwl->predict(b.address);

			if(!(tage_predict ^ piecewise_predict))
			{
				//both predicted the same thing dont need to choose
				u.direction_prediction(piecewise_predict);
				//equalPredictions = 1;
				tage_or_pwl = 0;
			}
			else // predictions differ
			{
				int piecewise_confidence = pwl->getLastOutput();
				if(tage->getChoseBasic())
				{
					u.direction_prediction(piecewise_predict);
					tage_or_pwl = 0;
				}
				else{
					u.direction_prediction(piecewise_predict);
					tage_or_pwl = 0;

				}
				
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
			// we should consider updating both ???
			/*if(equalPredictions)
			{
				tage->UpdatePredictor(bi.address,taken,u->target_prediction(),target);
				pwl->update(bi.address,u->direction_prediction(),taken,target);
			}
			else */if(tage_or_pwl)
			{
				tage->UpdatePredictor(bi.address,taken,u->target_prediction(),target);
			}
			else{
				pwl->update(bi.address,u->direction_prediction(),taken,target);
			}
		}
		if (bi.br_flags & BR_INDIRECT) {
			targets[bi.address & ((1<<TABLE_BITS)-1)] = target;
		}
	}
};
