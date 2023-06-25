#include "souper/Inst/Inst.h"

using namespace souper;

// prints the instructions tree
void OpsTree(Inst* I, int depth) {
  // indent for tree depth
  std::string indent = "";
  for (int i = 0; i < depth; i++) {
    if (i == depth-1)
      indent += "|---";
    else
      indent += "    ";

  }
  std::string k = Inst::getKindName(I->K);
  // print kind name and then other relevant info depending on kind
  llvm::errs() << indent << k;
  if (k=="const") 
    llvm::errs() << " " << I->Val;
  else if (k=="var") {
    // can check for input or reservedconst using SynthesisConstID
    std::string s = I->SynthesisConstID == 0 ? " (input)" : " (reservedconst)";
    llvm::errs() << " " << I->Name << s;
  }
  llvm::errs() << "\n";
  // print children
  for (int i = 0; i < I->Ops.size(); i++) {
    OpsTree(I->Ops[i], depth+1);
  }
}


// head recursion remove nodes that dont have anything to do with the mod analysis
Inst* getModExpression(Inst* R) {
    // TODO
    return R;
}

/*
example: 
LHS
|---add
    |---var 0 (input)
    |---const 50
RHS
|---add
    |---var 0 (input)
    |---mul
        |---var 2 (reservedconst)
        |---const 3

if its impossible to get the same value like with (LHS: a + 50, RHS: a + (RC * 3), then it can be pruned
*/
bool ModAnalysis(Inst* LHS, Inst* RHS) {
    Inst* ExprLHS = getModExpression(LHS);
    Inst* ExprRHS = getModExpression(RHS);
    // compare both expressions to see if they could get the same value for any reservedconst
    // i.e at some point it should get to node = constant, verify the constant can be obatined with node
    // TODO
    return false;
}