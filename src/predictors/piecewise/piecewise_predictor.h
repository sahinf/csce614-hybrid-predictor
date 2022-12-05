// piecewise_predictor.h
// This file contains a sample piecewise_predictor class.
// It has a simple 32,768-entry gshare with a history length of 15 and a
// simple direct-mapped branch target buffer for indirect branch prediction.

#ifndef PIECEWISE_PREDICTOR_H
#define PIECEWISE_PREDICTOR_H

#include "../../branch.h"
#include "../../predictor.h"
#include <cmath>
#include <cstdio>
#include <string.h>

class piecewise_update : public branch_update {
public:
  unsigned int index;
};

class piecewise_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH 15
#define TABLE_BITS 15
  piecewise_update u;
  branch_info bi;
  unsigned int history;
  unsigned char tab[1 << TABLE_BITS];
  unsigned int targets[1 << TABLE_BITS];
  static const unsigned int n = 8;
  static const unsigned int m = 151;
  static const unsigned int h = 43;
  static const unsigned int theta = 2.14 * (h + 1) + 20.58;
  unsigned int W[n][m][h + 1];
  unsigned int GA[h + 1];
  unsigned int GHR[h + 1];
  piecewise_predictor(void) : history(0) {
    printf("Constructing piecewise predictor\n");
    memset(tab, 0, sizeof(tab));
    memset(targets, 0, sizeof(targets));
    memset(GA, 0, sizeof(GA));
    memset(GHR, 0, sizeof(GHR));
    memset(W, 0, sizeof(W));
  }

  branch_update *predict(branch_info &b) {
    int output = W[b.address % n][0][0];
    bool predict = 0;
    for (unsigned int i = 1; i <= h; i++) {
      if (GHR[i] == true) {
        output += W[b.address % n][GA[i] % m][i];
      } else {
        output -= W[b.address % n][GA[i] % m][i];
      }
    }
    if (output >= 0) {
      predict = 1;
    }
    bi = b;

    if (b.br_flags & BR_CONDITIONAL) {
      u.direction_prediction(predict);
    }
    /*if (b.br_flags & BR_CONDITIONAL) {
            u.index =
                      (history << (TABLE_BITS - HISTORY_LENGTH))
                    ^ (b.address & ((1<<TABLE_BITS)-1));
            u.direction_prediction (tab[u.index] >> 1);
    } */
    else {
      u.direction_prediction(true);
    }
    if (b.br_flags & BR_INDIRECT) {
      u.target_prediction(targets[b.address & ((1 << TABLE_BITS) - 1)]);
      // u.target_prediction (0);
    }
    return &u;
  }

  void update(branch_update *u, bool taken, unsigned int target) {
    if (bi.br_flags & BR_CONDITIONAL) {
      int output = W[bi.address % n][0][0];

      if (taken != u->direction_prediction() || abs(output) < theta) {
        if (taken == true) {
          W[bi.address % n][0][0] += 1;
        } else {
          W[bi.address % n][0][0] -= 1;
        }
        for (unsigned int i = 1; i < h + 1; i++) {
          if (GHR[i] == taken) {
            W[bi.address % n][GA[i] % m][i] += 1;
          } else {
            W[bi.address % n][GA[i] % m][i] -= 1;
          }
        }
      }

      memmove(&GA[2], &GA[1], (h - 1) * sizeof(int));
      GA[1] = bi.address;
      memmove(&GHR[2], &GHR[1], (h - 1) * sizeof(int));
      GHR[1] = taken;
    }
    /*
    if (bi.br_flags & BR_CONDITIONAL) {
            unsigned char *c = &tab[((piecewise_update*)u)->index];
            if (taken) {
                    if (*c < 3) (*c)++;
            } else {
                    if (*c > 0) (*c)--;
            }
            history <<= 1;
            history |= taken;
            history &= (1<<HISTORY_LENGTH)-1;
    }*/
    if (bi.br_flags & BR_INDIRECT) {
      targets[bi.address & ((1 << TABLE_BITS) - 1)] = target;
    }
  }
};

#endif