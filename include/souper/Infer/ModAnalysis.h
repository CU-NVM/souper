#ifndef SOUPER_MODANALYSIS_H
#define SOUPER_MODANALYSIS_H

#include "souper/Inst/Inst.h"
using namespace souper;

namespace ModAnalysis{
// prints the instructions tree
void OpsTree(Inst* I, int depth);

int16_t FindModVal(Inst* I);

int16_t ModAnalysisVal(Inst* LHS, Inst* RHS);
}

#endif