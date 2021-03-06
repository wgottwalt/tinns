#include "gameserver/Includes.hxx"

void PSkillHandler::SetMainSkill(MAIN_SKILLS Skill, int value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTR = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEX = value;
        break;

    case MS_CON:
        m_Skills.Main->mCON = value;
        break;

    case MS_INT:
        m_Skills.Main->mINT = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSI = value;
        break;
    };
}

void PSkillHandler::SetSubSkill(SUB_SKILLS Skill, int value)
{
    switch (Skill)
    {
// STR Skills ------------------------
    case SK_MC:
        m_Skills.STR->mMC = value;
        break;

    case SK_HC:
        m_Skills.STR->mHC = value;
        break;

    case SK_TRA:
        m_Skills.STR->mTRA = value;
        break;

    case SK_FOR:
        m_Skills.STR->mFOR = value;
        break;
// DEX Skills ------------------------
    case SK_PC:
        m_Skills.DEX->mPC = value;
        break;

    case SK_RC:
        m_Skills.DEX->mRC = value;
        break;

    case SK_TC:
        m_Skills.DEX->mTC = value;
        break;

    case SK_VHC:
        m_Skills.DEX->mVHC = value;
        break;

    case SK_AGL:
        m_Skills.DEX->mAGL = value;
        break;

    case SK_REP:
        m_Skills.DEX->mREP = value;
        break;

    case SK_REC:
        m_Skills.DEX->mREC = value;
        break;

    case SK_RCL:
        m_Skills.DEX->mRCL = value;
        break;
// CON Skills ------------------------
    case SK_ATL:
        m_Skills.CON->mATL = value;
        break;

    case SK_END:
        m_Skills.CON->mEND = value;
        break;

    case SK_FIR:
        m_Skills.CON->mFIR = value;
        break;

    case SK_ENR:
        m_Skills.CON->mENR = value;
        break;

    case SK_XRR:
        m_Skills.CON->mXRR = value;
        break;

    case SK_POR:
        m_Skills.CON->mPOR = value;
        break;

    case SK_HLT:
        m_Skills.CON->mHLT = value;
        break;
// INT Skills ------------------------
    case SK_HCK:
        m_Skills.INT->mHCK = value;
        break;

    case SK_BRT:
        m_Skills.INT->mBRT = value;
        break;

    case SK_PSU:
        m_Skills.INT->mPSU = value;
        break;

    case SK_WEP:
        m_Skills.INT->mWEP = value;
        break;

    case SK_CST:
        m_Skills.INT->mCST = value;
        break;

    case SK_RES:
        m_Skills.INT->mRES = value;
        break;

    case SK_IMP:
        m_Skills.INT->mIMP = value;
        break;

    case SK_WPW:
        m_Skills.INT->mWPW = value;
        break;
// PSI Skills ------------------------
    case SK_PPU:
        m_Skills.PSI->mPPU = value;
        break;

    case SK_APU:
        m_Skills.PSI->mAPU = value;
        break;

    case SK_MST:
        m_Skills.PSI->mMST = value;
        break;

    case SK_PPW:
        m_Skills.PSI->mPPW = value;
        break;

    case SK_PSR:
        m_Skills.PSI->mPSR = value;
        break;
    };
}

void PSkillHandler::SetXP(MAIN_SKILLS Skill, float value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTRxp = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEXxp = value;
        break;

    case MS_CON:
        m_Skills.Main->mCONxp = value;
        break;

    case MS_INT:
        m_Skills.Main->mINTxp = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSIxp = value;
        break;
    };
}

void PSkillHandler::SetSP(MAIN_SKILLS Skill, short value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTRsp = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEXsp = value;
        break;

    case MS_CON:
        m_Skills.Main->mCONsp = value;
        break;

    case MS_INT:
        m_Skills.Main->mINTsp = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSIsp = value;
        break;
    };
}

int PSkillHandler::GetMainSkill(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTR;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEX;
        break;

    case MS_CON:
        return m_Skills.Main->mCON;
        break;

    case MS_INT:
        return m_Skills.Main->mINT;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSI;
        break;
    }
    return 0;
}

int PSkillHandler::GetSubSkill(SUB_SKILLS Skill)
{
    switch (Skill)
    {
// STR Skills ------------------------
    case SK_MC:
        return m_Skills.STR->mMC;
        break;

    case SK_HC:
        return m_Skills.STR->mHC;
        break;

    case SK_TRA:
        return m_Skills.STR->mTRA;
        break;

    case SK_FOR:
        return m_Skills.STR->mFOR;
        break;
// DEX Skills ------------------------
    case SK_PC:
        return m_Skills.DEX->mPC;
        break;

    case SK_RC:
        return m_Skills.DEX->mRC;
        break;

    case SK_TC:
        return m_Skills.DEX->mTC;
        break;

    case SK_VHC:
        return m_Skills.DEX->mVHC;
        break;

    case SK_AGL:
        return m_Skills.DEX->mAGL;
        break;

    case SK_REP:
        return m_Skills.DEX->mREP;
        break;

    case SK_REC:
        return m_Skills.DEX->mREC;
        break;

    case SK_RCL:
        return m_Skills.DEX->mRCL;
        break;
// CON Skills ------------------------
    case SK_ATL:
        return m_Skills.CON->mATL;
        break;

    case SK_END:
        return m_Skills.CON->mEND;
        break;

    case SK_FIR:
        return m_Skills.CON->mFIR;
        break;

    case SK_ENR:
        return m_Skills.CON->mENR;
        break;

    case SK_XRR:
        return m_Skills.CON->mXRR;
        break;

    case SK_POR:
        return m_Skills.CON->mPOR;
        break;

    case SK_HLT:
        return m_Skills.CON->mHLT;
        break;
// INT Skills ------------------------
    case SK_HCK:
        return m_Skills.INT->mHCK;
        break;

    case SK_BRT:
        return m_Skills.INT->mBRT;
        break;

    case SK_PSU:
        return m_Skills.INT->mPSU;
        break;

    case SK_WEP:
        return m_Skills.INT->mWEP;
        break;

    case SK_CST:
        return m_Skills.INT->mCST;
        break;

    case SK_RES:
        return m_Skills.INT->mRES;
        break;

    case SK_IMP:
        return m_Skills.INT->mIMP;
        break;

    case SK_WPW:
        return m_Skills.INT->mWPW;
        break;
// PSI Skills ------------------------
    case SK_PPU:
        return m_Skills.PSI->mPPU;
        break;

    case SK_APU:
        return m_Skills.PSI->mAPU;
        break;

    case SK_MST:
        return m_Skills.PSI->mMST;
        break;

    case SK_PPW:
        return m_Skills.PSI->mPPW;
        break;

    case SK_PSR:
        return m_Skills.PSI->mPSR;
        break;
    };
    return 0;
}

int PSkillHandler::GetSKPCost(SUB_SKILLS Skill)
{
    int skillvalue = GetSubSkill(Skill);

    if (skillvalue < 50)
    {
        return 1;
    }
    else if (skillvalue < 75)
    {
        return 2;
    }
    else if (skillvalue < 100)
    {
        return 3;
    }
    else
    {
        return 5;
    }
}

unsigned short PSkillHandler::GetSP(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTRsp;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEXsp;
        break;

    case MS_CON:
        return m_Skills.Main->mCONsp;
        break;

    case MS_INT:
        return m_Skills.Main->mINTsp;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSIsp;
        break;
    };
    return 0;
}

float PSkillHandler::GetXP(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTRxp;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEXxp;
        break;

    case MS_CON:
        return m_Skills.Main->mCONxp;
        break;

    case MS_INT:
        return m_Skills.Main->mINTxp;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSIxp;
        break;
    };
    return 0;
}

MAIN_SKILLS PSkillHandler::GetMSfromSS(SUB_SKILLS Skill)
{
    switch (Skill)
    {
    case SK_HCK:
    case SK_BRT:
    case SK_PSU:
    case SK_WEP:
    case SK_CST:
    case SK_RES:
    case SK_IMP:
    case SK_WPW:
        return MS_INT;
        break;

    case SK_ATL:
    case SK_END:
    case SK_FIR:
    case SK_ENR:
    case SK_XRR:
    case SK_POR:
    case SK_HLT:
        return MS_CON;
        break;

    case SK_MC:
    case SK_HC:
    case SK_TRA:
    case SK_FOR:
        return MS_STR;
        break;

    case SK_PC:
    case SK_RC:
    case SK_TC:
    case SK_VHC:
    case SK_AGL:
    case SK_REP:
    case SK_REC:
    case SK_RCL:
        return MS_DEX;
        break;

    case SK_PPU:
    case SK_APU:
    case SK_MST:
    case SK_PPW:
    case SK_PSR:
        return MS_PSI;
        break;

    default:
        return 0;
        break;
    }
}

bool PSkillHandler::IncSubSkillPossible(SUB_SKILLS Skill)
{
    // Check if player is able to inc the given subskill (Enough points left to do that)
    MAIN_SKILLS tMainSkill = GetMSfromSS(Skill);

    // Failsafe
    if ( tMainSkill == 0 )
        return false;

    // Compare amount of available skillpoints for given mainskill
    // with the amount of points required to increase the subskill
    if ( GetSP( tMainSkill ) >= GetSKPCost( Skill ))
        return true;
    else
        return false;
}

int PSkillHandler::IncreaseSubSkill(SUB_SKILLS Skill)
{
    // Not enough points left? Then exit here
    if ( IncSubSkillPossible( Skill ) == false)
        return -1;

    MAIN_SKILLS tMainSkill = GetMSfromSS( Skill );
    // Remove skillpoints from availabe ones
    SetSP( tMainSkill, GetSP( tMainSkill ) - GetSKPCost( Skill ));

    // Set new value
    SetSubSkill(Skill, GetSubSkill( Skill ) + 1 );
    return GetSP( tMainSkill );
}

void PSkillHandler::ZeroMSSubSkills(MAIN_SKILLS Skill)
{
    // Set all SubSkills for the given Mainskill to Zero
    switch ( Skill )
    {
    case MS_STR:
        SetSubSkill(SK_MC,0);
        SetSubSkill(SK_HC,0);
        SetSubSkill(SK_TRA,0);
        SetSubSkill(SK_FOR,0);
        break;

    case MS_DEX:
        SetSubSkill(SK_PC,0);
        SetSubSkill(SK_RC,0);
        SetSubSkill(SK_TC,0);
        SetSubSkill(SK_VHC,0);
        SetSubSkill(SK_AGL,0);
        SetSubSkill(SK_REP,0);
        SetSubSkill(SK_REC,0);
        SetSubSkill(SK_RCL,0);
        break;

    case MS_CON:
        SetSubSkill(SK_ATL,0);
        SetSubSkill(SK_END,0);
        SetSubSkill(SK_FIR,0);
        SetSubSkill(SK_ENR,0);
        SetSubSkill(SK_XRR,0);
        SetSubSkill(SK_POR,0);
        SetSubSkill(SK_HLT,0);
        break;

    case MS_INT:
        SetSubSkill(SK_HCK,0);
        SetSubSkill(SK_BRT,0);
        SetSubSkill(SK_PSU,0);
        SetSubSkill(SK_WEP,0);
        SetSubSkill(SK_CST,0);
        SetSubSkill(SK_RES,0);
        SetSubSkill(SK_IMP,0);
        SetSubSkill(SK_WPW,0);
        break;

    case MS_PSI:
        SetSubSkill(SK_PPU,0);
        SetSubSkill(SK_APU,0);
        SetSubSkill(SK_MST,0);
        SetSubSkill(SK_PPW,0);
        SetSubSkill(SK_PSR,0);
        break;
    }
}

bool PSkillHandler::IsValidSubSkill(SUB_SKILLS Skill)
{
    switch (Skill)
    {
    case SK_HCK:
    case SK_BRT:
    case SK_PSU:
    case SK_WEP:
    case SK_CST:
    case SK_RES:
    case SK_IMP:
    case SK_WPW:
    case SK_ATL:
    case SK_END:
    case SK_FIR:
    case SK_ENR:
    case SK_XRR:
    case SK_POR:
    case SK_HLT:
    case SK_MC:
    case SK_HC:
    case SK_TRA:
    case SK_FOR:
    case SK_PC:
    case SK_RC:
    case SK_TC:
    case SK_VHC:
    case SK_AGL:
    case SK_REP:
    case SK_REC:
    case SK_RCL:
    case SK_PPU:
    case SK_APU:
    case SK_MST:
    case SK_PPW:
    case SK_PSR:
        return true;
        break;

    default:
        return false;
        break;
    }
}

bool PSkillHandler::IsValidMainSkill(MAIN_SKILLS Skill)
{
    switch ( Skill )
    {
    case MS_STR:
    case MS_DEX:
    case MS_CON:
    case MS_INT:
    case MS_PSI:
        return true;
        break;
    default:
        return false;
        break;
    }
}
