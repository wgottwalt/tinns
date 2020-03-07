#pragma once

#include "gameserver/definitions/Definition.hxx"

// Implants Id 1 to 5 correspond to OP Zone bonus: mine, factory, lab, fort and comlink respectively

class PDefImplant : public PDefDrug {
public:
    bool LoadFromDef( PTokenList *Tokens );
};
