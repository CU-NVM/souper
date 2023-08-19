#include "souper/Inst/Inst.h"
#include "souper/Infer/Interpreter.h"
#include "souper/Infer/ModAnalysis.h"
#include <bitset>
using namespace souper;
using namespace std;

// prints the instructions tree
void ModAnalysis::OpsTree(Inst* I, int depth) {
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
  llvm::outs() << indent << k;
  if (k=="const") 
    llvm::outs() << " " << I->Val;
  else if (k=="var") {
    // can check for input or reservedconst using SynthesisConstID
    std::string s = I->SynthesisConstID == 0 ? " (input)" : " (reservedconst)";
    llvm::outs() << " " << I->Name << s;
  }
  llvm::outs() << "\n";
  // print children
  for (int i = 0; i < I->Ops.size(); i++) {
    OpsTree(I->Ops[i], depth+1);
  }
}
bool isPowerOf2(int16_t n) {
    return (n & (n - 1)) == 0;
}

// Finding the maximum value to find the mod value from valid inputs
void ModAnalysis::ModMaxVarVal(Inst* LHS, ValueCache Input){
    std::string KindName = LHS->getKindName(LHS->K);
    if (KindName == "var"){
      if(Input[LHS].hasValue()){
      if (Input[LHS].getValue().getLimitedValue() > mod_val){
        
        mod_val = Input[LHS].getValue().getLimitedValue();

    }
    }
    for (int i = 0; i<LHS->Ops.size();i++)
      ModMaxVarVal(LHS->Ops[i],Input);
  }
}


//Find the Mod value vased on the mod_val
int32_t ModAnalysis::FindModVal(Inst* I, ValueCache Input){

  std::string KindName = I->getKindName(I->K);
  // llvm::outs() << "Name : "<<I->Name <<" Kind:"<< I->getKindName(I->K) << " val :"<<I->Val<<"\n";
      
      if (KindName == "var"){
        if(Input[I].hasValue()){
          result = Input[I].getValue().getLimitedValue() % mod_val;
        }
        else{
          result = 100 % mod_val;//defaults to this if we run out of generated Input values
        }
          }
      else if (KindName == "const"){
            result = I->Val.urem(mod_val);
      }
      else if (KindName == "add" || KindName == "sub"){
        int left_op = FindModVal(I->Ops[0],Input);
        int right_op = FindModVal(I->Ops[1],Input);
        if (left_op < 0 || right_op < 0){
          result = -mod_val-10;
          }
        else{
          if (KindName == "add"){
          result = (left_op + right_op) % mod_val;
          }
          else{
            int result_var = (left_op - right_op);
            if (result_var < 0){
              while(result_var < 0)
                result_var+=mod_val;
            }
            result = result_var % mod_val;
          }
        }
        }
      else if (KindName == "mul"){
        int left_op = FindModVal(I->Ops[0],Input);
        int right_op = FindModVal(I->Ops[1],Input);
        
        if (left_op >= 0 && right_op >= 0)
          result = (left_op * right_op) % mod_val;
        else
          result = -mod_val-10;
      }
      else if (KindName == "zext"){
        result = FindModVal(I->Ops[0],Input);
      }
      else if (KindName  == "trunc"){
        if (mod_val == 2)
          result = FindModVal(I->Ops[0],Input);
        else
          result = -mod_val-10;
      }
      else if (KindName == "shl"){
        if (isPowerOf2(mod_val)){ 
          if (FindModVal(I->Ops[0],Input) >= 0)
           return (FindModVal(I->Ops[0],Input) << I->Ops[1]->Val.getLimitedValue()) % mod_val;
        
        }
      }
      else
        result = -mod_val-10; // Random negative value to indicate no-pruning
  return result;
}

int32_t ModAnalysis::ModAnalysisVal(Inst* LHS,Inst* RHS ,ValueCache Input){
  
  mod_val = -1;
  ModMaxVarVal(LHS,Input);
  if (mod_val <= 1)
    mod_val = 3;
  result = -mod_val-10;

  // llvm::outs()<<"Mod val found is "<<mod_val<<"\n";
  result = -mod_val;
  int32_t left_val = FindModVal(LHS,Input);

  // llvm::outs()<<"RHS \n";
  result = -mod_val;
  int32_t right_val = FindModVal(RHS, Input);

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

int32_t ModAnalysis::FindOtherModVal(Inst* LHS,Inst* RHS,ValueCache V){
  int32_t var_val = 2;
  int32_t left_val;
  int32_t right_val;
  for (;var_val <= 20; var_val+=1){
    mod_val = var_val;
    result = -mod_val-10;

  left_val = FindModVal(LHS,V);
  right_val = FindModVal(RHS,V);

  if (left_val >= 0 && right_val >=0){
     if (left_val != right_val){
      // llvm::outs()<<"var_val "<<var_val<<"result "<<result<<"lv "<<left_val<<"rv "<<right_val<<"\n";
      return MOD_ANALYSIS_SUCCESSFUL;
     }
  }
  }
  llvm::outs() <<"Cannot Infer Mod value\n";
  return MOD_ANALYSIS_UNSUCCESSFUL;
}



bool ModAnalysis::hasphinode(Inst* I){
  
  string Kindname = I->getKindName(I->K);

  if(Kindname == "phi")
    return true;
  
  for (int i = 0; i<I->Ops.size();i++){
    if(hasphinode(I->Ops[i]))
      return true;
  }
  return false;

}

/*
This function runs the mod analysis for all valid inputs generted by souper and 
compares the values for LHS and RHS.
if they don't work, it takes a bunch of values for mod bases and then tries to 
infer LHS and RHS.
*/

int32_t ModAnalysis::ModForAllInputs(Inst* LHS,Inst* RHS, vector<ValueCache> Inputs){
  int16_t ModVal = 0;
  for (auto Input: Inputs){
    ModVal = ModAnalysisVal(LHS,RHS,Input);
    if (ModVal){
      return MOD_ANALYSIS_SUCCESSFUL;
    }
  }
  ValueCache V ;
/*
This is a incomplete implementation of the phiset concept.
We try to create a set of values based on the phi nodes and
compare LHS and RHS.
*/
  if (hasphinode(LHS)){
    for (auto input: Inputs){
      FormPhiset(LHS,input);
      InterpretPhi(LHS,input);
      vector<int32_t> lhsinfer = phimap[LHS->Name];
      for(auto in:lhsinfer)
        llvm::outs()<<in<<" ";

    }
  }

  if (FindOtherModVal(LHS,RHS, V)){
    return MOD_ANALYSIS_SUCCESSFUL;
  }
  return MOD_ANALYSIS_UNSUCCESSFUL;
}

/*
 NOTE: Incomplete Implementation
This is an attempt to concrete interpret value for LHS and candidate by assuming some value 
for vars.
*/

int32_t ModAnalysis::ConcInterpretInst(Inst* I, ValueCache var){
      string Kindname = I->getKindName(I->K);

      if (Kindname == "var"){
        if(var[I].hasValue()){
          return var[I].getValue().getLimitedValue();
        }
        return 100 ;
      }
      
      else if(Kindname == "const")
        return I->Val.getLimitedValue();

      else if (Kindname == "add" || Kindname == "addnsw")
        return ConcInterpretInst(I->Ops[0],var) + ConcInterpretInst(I->Ops[1],var);

      else if (Kindname == "sub" || Kindname == "subnsw")
        return ConcInterpretInst(I->Ops[0],var) - ConcInterpretInst(I->Ops[1],var);
      
      else if (Kindname == "mul" || Kindname == "mulnsw")
        return ConcInterpretInst(I->Ops[0],var) * ConcInterpretInst(I->Ops[1],var);

      else if (Kindname == "slt" || Kindname == "ult")
        return ConcInterpretInst(I->Ops[0],var) < ConcInterpretInst(I->Ops[1],var);
      
      else if (Kindname == "eq")
        return ConcInterpretInst(I->Ops[0],var) == ConcInterpretInst(I->Ops[1],var);
      
      else if (Kindname == "ne")
        return ConcInterpretInst(I->Ops[0],var) != ConcInterpretInst(I->Ops[1],var);

      else 
        return -1;
}

/*
NOTE: Incomplete Implementation
Function is used to create a hash map for the phi node present in the IR.
*/
vector<int32_t> ModAnalysis::Phiset(Inst* I,ValueCache Input){
  vector<int32_t> phiset;
  for (int i = 1; i<I->Ops.size();i++){
    phiset.push_back(ConcInterpretInst(I->Ops[i],Input));
  }
  return phiset;
}

/*
NOTE: Incomplete Implementation
Function is used to create a hash map for the phi node present in the IR.
*/
void ModAnalysis::FormPhiset(Inst* I,ValueCache Input){
  string Kindname = I->getKindName(I->K);

    if (Kindname == "phi"){
    vector<int32_t> pset = Phiset(I,Input);
    phimap[I->Name] = pset;
  }

  for (int i=0; i<I->Ops.size();i++){
    FormPhiset(I->Ops[i],Input);
  }

}
/*

*/

void ModAnalysis::InterpretPhi(Inst* I, ValueCache Input){
  
  string Kindname = I->getKindName(I->K);

    if (phimap.find(I->Ops[0]->Name) != phimap.end()){
      llvm::outs()<<"inside phimap\n";
      vector<int32_t> inputs = phimap[I->Ops[0]->Name];
        if (Kindname == "add"){
          for(auto input:inputs){
            input+=I->Ops[1]->Val.getLimitedValue();
          }
        }
        if (Kindname == "sub"){
          for(auto input:inputs)
            input-=I->Ops[1]->Val.getLimitedValue();
        }
        if (Kindname == "slt"){
          for(auto input:inputs){
            int32_t val = input < I->Ops[1]->Val.getLimitedValue();
            input = val;
          }
        }
        phimap[I->Name] = inputs;
    }
  
}
