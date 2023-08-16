#ifndef SOUPER_MODANALYSIS_H
#define SOUPER_MODANALYSIS_H

#include "souper/Inst/Inst.h"
#include "souper/Infer/Interpreter.h"
using namespace souper;
using namespace std;


// prints the instructions tree
void OpsTree(Inst* I, int depth);

class ModAnalysis{
  private:
    souper::Inst* LHS;
    souper::Inst* RHS;
    vector<souper::ValueCache> Inputs; 
    int32_t result;
    int32_t mod_val;
    // uint16_t var_val;
    enum {
    MOD_ANALYSIS_UNSUCCESSFUL,
    MOD_ANALYSIS_SUCCESSFUL
    };
    int32_t FindModVal(Inst* I, ValueCache Input);
    void ModMaxVarVal(Inst* LHS,ValueCache Input);
  public:
     ModAnalysis(Inst* LHS,Inst* RHS):LHS(LHS),RHS(RHS){};
     void getinput(vector<ValueCache> &InputsVal){
        Inputs = InputsVal;
     }
    void OpsTree(Inst* I, int depth);

    int32_t ModAnalysisVal(Inst* LHS, Inst* RHS, ValueCache Input);
    
    int32_t ModForAllInputs(Inst* LHS,Inst* RHS, vector<ValueCache> Inputs);
    
    int32_t RunModAnalysis(){
      return ModForAllInputs(LHS,RHS,Inputs); 
    }
    int32_t FindOtherModVal(Inst* LHS,Inst* RHS,ValueCache V);
    
    //Indicate the presence of phi node
    bool lhshasphinode = false;
    bool rhshasphinode = false;

    unordered_map<string,vector<int32_t>> phimap;

    vector<int32_t> Phiset(Inst* I,ValueCache Input);
    
    void InterpretPhi(Inst* I,ValueCache Input);

    int32_t ConcInterpretInst(Inst* I, ValueCache var);

    void FormPhiset(Inst* I,ValueCache Input);
    
    bool hasphinode(Inst* I);
};



#endif