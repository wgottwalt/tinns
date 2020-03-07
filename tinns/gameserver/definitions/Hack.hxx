#pragma once

#include "gameserver/definitions/Definition.hxx"

class PDefHack : public PDef {
private:
    //int mIndex;
    //qui aggiungere valori
public:
    PDefHack();
    //~PDefHack();

    bool LoadFromDef(PTokenList *Tokens);

    //qui aggiungere funzioni per i gets
};
