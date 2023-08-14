#include "souper/Inst/Inst.h"

using namespace souper;

// select inputs by finding constants that satisfy cmp statements, so that concrete interpretation can perform better
int FindInputsR(Inst* I, int constant, std::vector<int> &inputs) {
    switch (I->K) {
        case Inst::Const:
            return I->Val.getSExtValue();
        case Inst::Var:
            // if the var is an input, add the constant to inputs since it satisfies a compare
            // 0 is already an input to concrete interpretation so don't include it
            if (I->SynthesisConstID == 0 && constant != NULL)
                inputs.push_back(constant);
            break;
        // reverse arithmetic instructions that use constants to derive the constant that satisfies the compares
        case Inst::Add:
            if (constant)
                return FindInputsR(I->Ops[0], constant - FindInputsR(I->Ops[1], constant, inputs), inputs);
            break;
        case Inst::Sub:
            if (constant)
                return FindInputsR(I->Ops[0], constant + FindInputsR(I->Ops[1], constant, inputs), inputs);
            break;
        // compare instructions
        case Inst::Eq:
        case Inst::Ne: // this is functionally the same as equals since its easy for CR to return true, but not false
            // check if one operand is a const
            if (I->Ops[1]->K == Inst::Const) {
                if (I->Ops[0]->K != Inst::Const) {
                    return FindInputsR(I->Ops[0], I->Ops[1]->Val.getSExtValue(), inputs);
                }
            } else {
                if (I->Ops[0]->K == Inst::Const) {
                    return FindInputsR(I->Ops[1], I->Ops[0]->Val.getSExtValue(), inputs);
                }
            }
            break;
        default:
            break;
    }
    for (int i = 0; i < I->Ops.size(); i++) {
        return FindInputsR(I->Ops[i], constant, inputs);
    }
}

// takes root Inst (currently only of the RHS) and finds inputs that satsify eq statements
std::vector<int> FindInputs(Inst* Root) {
    std::vector<int> inputs;
    FindInputsR(Root,NULL,inputs);
    return inputs;
}

/*
souper-check tests this helps with:
%0:i32 = var
%1:i32 = add %0, 4:i32
%2:i1 = eq 10:i32, %1
%3:i1 = add %2, %2
infer %3
%4:i1 = eq %1, 12:i32
result %4

%0:i32 = var
%1:i32 = add %0, 4:i32
%2:i1 = ne 10:i32, %1
infer %2
%3:i32 = sub %0, 4:i32
%4:i1 = ne %3, 12:i32
result %4
*/