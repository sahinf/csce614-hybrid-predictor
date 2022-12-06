/**
 * @file my_predictor.h
 * @author Furkan Sahin, Reid O'Boyle, Satya Sreenadh Meesala
 * @brief This hybrid predictor initializes two predictors, piecewise and TAGE,
 * and decides on a prediction from one of them
 * @version 0.1
 *
 * classes branch_update & branch_predictor come from predictor.h, which is not
 * protected with header guards. It's included in the final translation unit in
 * the file predict.cc so including it here would cause a multiple symbol linker
 * error. We are not allowed to alter either predict.cc or predictor.h so the
 * local language server gets confused (red squigglies).
 */

#ifndef MY_PREDICTOR_H
#define MY_PREDICTOR_H

#include <string.h>

#include "branch.h"
#include "predictors/piecewise/piecewise_predictor.h"
#include "predictors/tage/TAGEPredictor.h"
#include "saturated_counter.h"

#define SAT_RANGE 128

class my_update : public branch_update {
  public:
   unsigned int index;
   saturated_counter<int> tage_counter;
   saturated_counter<int> pwl_counter;

	my_update() : branch_update(), tage_counter(SAT_RANGE), pwl_counter(SAT_RANGE) {}
};

enum predictor_t { tage_e, pwl_e };

class my_predictor : public branch_predictor {
  public:
#define HISTORY_LENGTH 15
#define TABLE_BITS 15
   my_update u;
   branch_info bi;
   unsigned int history;
   unsigned char tab[1 << TABLE_BITS];
   unsigned int targets[1 << TABLE_BITS];
   predictor_t predictor;
	PREDICTOR *tage;
	Piecewise *pwl;
   my_predictor(void) : history(0) {
      memset(tab, 0, sizeof(tab));
      memset(targets, 0, sizeof(targets));
      predictor = tage_e;  // initialize predictor to TAGE
	tage = new PREDICTOR();
	pwl = new Piecewise();
   }

   branch_update *predict(branch_info &b) {
      // Run currently selected predictor
      bool prediction = false;
      switch (predictor) {
         case tage_e:
            prediction = tage->GetPrediction(b.address);
            break;
         case pwl_e:
            prediction = pwl->predict(b.address);
            break;
         default:
            break;
      }

      bi = b;

      if (b.br_flags & BR_CONDITIONAL) {
         u.direction_prediction(prediction);
      }
      else {
         u.direction_prediction(true);
      }
      if (b.br_flags & BR_INDIRECT) {
         u.target_prediction(targets[b.address & ((1 << TABLE_BITS) - 1)]);
         // u.target_prediction (0);
      }
      return &u;
   }

   // Update the state of chosen predictor
   void update(branch_update *u, bool taken, unsigned int target) {
      if (bi.br_flags & BR_CONDITIONAL) {
         // Update global counter based on whether a branch was resolved to be taken
         // Then switch predictors if current predictor falls behind

         my_update *mu = (my_update *)u;
         switch (predictor) {
            case tage_e:
               tage->UpdatePredictor(bi.address, taken, u->target_prediction(), target);
               saturate(mu->tage_counter, taken);
               if (mu->pwl_counter > mu->tage_counter) predictor = pwl_e;
               break;
            case pwl_e:
               pwl->update(bi.address, u->direction_prediction(), taken, target);
               saturate(mu->pwl_counter, taken);
               if (mu->tage_counter > mu->pwl_counter) predictor = tage_e;
               break;
            default:
               break;
         }
      }
      if (bi.br_flags & BR_INDIRECT) {
         targets[bi.address & ((1 << TABLE_BITS) - 1)] = target;
      }
   }

   inline void saturate(saturated_counter<int> &sat, bool taken) {
      if (taken)
         sat++;
      else
         sat--;
   }
};

#endif
