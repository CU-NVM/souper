#include "souper/Inst/Inst.h"
#include "souper/Infer/ModAnalysis.h"

using namespace souper;

namespace ModAnalysis{
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

enum {
  MOD_ANALYSIS_UNSUCCESSFUL,
  MOD_ANALYSIS_SUCCESSFUL
};


static int16_t result = -1;
static uint64_t mod_val = 3;
static uint16_t var_val = 100; //taking some value for var

int16_t FindModVal(Inst* I){

  std::string KindName = I->getKindName(I->K);

  // llvm::outs() << "Name : "<<I->Name <<" Kind:"<< I->getKindName(I->K) << " val :"<<I->Val<<"\n";

      if (KindName == "var"){
          result = var_val % 3;}
      else if (KindName == "const"){
          result = I->Val.urem(mod_val);
          }
      else if (KindName == "add" || KindName == "sub"){
        int left_op = FindModVal(I->Ops[0]);
        int right_op = FindModVal(I->Ops[1]);
        if (left_op == -10 || right_op == -10){
          result = -10;}
        else{
          if (KindName == "add"){
          result = (left_op + right_op)%3;
          }
          else{
            result = (left_op - right_op)%3;
          }
        }}
      else if (KindName == "mul"){
        int left_op = FindModVal(I->Ops[0]);
        int right_op = FindModVal(I->Ops[1]);

        if (left_op == 0 || right_op == 0)
          result = 0;
        else
          result = -10;
      }
      if (result < 0)
        result = -10; // Random negative value to indicate no-pruning
  return result;
}

int16_t ModAnalysisVal(Inst* LHS,Inst* RHS ){
  int16_t left_val = FindModVal(LHS);
  int16_t right_val = FindModVal(RHS);

  llvm::outs() << "Left val " <<left_val <<" right val "<<right_val<<"\n";

  if (left_val >= 0 && right_val >=0){
     if (left_val == right_val){
      llvm::outs()<<"Mod Value same, Cannot be pruned \n";
      return MOD_ANALYSIS_UNSUCCESSFUL;
     }
     else{
      llvm::outs()<<"Pruned using mod analysis \n";
      return MOD_ANALYSIS_SUCCESSFUL;
     }
}
  llvm::outs() <<"Cannot Infer Mod value\n";
  return MOD_ANALYSIS_UNSUCCESSFUL;
} 
}