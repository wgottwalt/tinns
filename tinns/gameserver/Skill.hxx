#pragma once

#include <cstdint>

#define SUB_SKILLS uint8_t
#define SK_MC 1
#define SK_HC 2
#define SK_TRA 3
#define SK_FOR 22
#define SK_PC 10
#define SK_RC 11
#define SK_TC 12
#define SK_VHC 13
#define SK_AGL 14
#define SK_REP 15
#define SK_REC 16
#define SK_RCL 17
#define SK_ATL 20
#define SK_END 21
#define SK_FIR 23
#define SK_ENR 24
#define SK_XRR 25
#define SK_POR 26
#define SK_HLT 27
#define SK_HCK 30
#define SK_BRT 31
#define SK_PSU 32
#define SK_WEP 33
#define SK_CST 34
#define SK_RES 35
#define SK_IMP 36
#define SK_PPU 40
#define SK_APU 41
#define SK_MST 42
#define SK_PPW 43
#define SK_PSR 44
#define SK_WPW 45

#define MAIN_SKILLS uint8_t
#define MS_STR 1
#define MS_DEX 2
#define MS_CON 3
#define MS_INT 4
#define MS_PSI 5

class PSkillHandler
{
private:
    struct PMainSkills
    {
        int mINT;
        int mCON;
        int mSTR;
        int mDEX;
        int mPSI;
        uint16_t mINTsp;
        uint16_t mCONsp;
        uint16_t mSTRsp;
        uint16_t mDEXsp;
        uint16_t mPSIsp;
        float mINTxp;
        float mCONxp;
        float mSTRxp;
        float mDEXxp;
        float mPSIxp;

        inline PMainSkills()
        {
            mINT = mCON = mSTR = mDEX = mPSI = 0;
            mINTsp = mCONsp = mSTRsp = mDEXsp = mPSIsp = 0;
            mINTxp = mCONxp = mSTRxp = mDEXxp = mPSIxp = 0.f;
        }
    };

    struct PSubSkillINT
    {
        int mHCK;
        int mBRT;
        int mPSU;
        int mWEP;
        int mCST;
        int mRES;
        int mIMP;
        int mWPW;

        inline PSubSkillINT()
        {
            mHCK = mBRT = mPSU = mWEP = mCST = mRES = mIMP = mWPW = 0;
        }
    };

    struct PSubSkillCON
    {
        int mATL;
        int mEND;
        int mFIR;
        int mENR;
        int mXRR;
        int mPOR;
        int mHLT;

        inline PSubSkillCON()
        {
            mATL = mEND = mFIR = mENR = mXRR = mPOR = mHLT = 0;
        }
    };

    struct PSubSkillSTR
    {
        int mMC;
        int mHC;
        int mTRA;
        int mFOR;

        inline PSubSkillSTR()
        {
            mMC = mHC = mTRA = mFOR = 0;
        }
    };

    struct PSubSkillDEX
    {
        int mPC;
        int mRC;
        int mTC;
        int mVHC;
        int mAGL;
        int mREP;
        int mREC;
        int mRCL;

        inline PSubSkillDEX()
        {
            mPC = mRC = mTC = mVHC = mAGL = mREP = mREC = mRCL = 0;
        }
    };

    struct PSubSkillPSI
    {
        int mPPU;
        int mAPU;
        int mMST;
        int mPPW;
        int mPSR;

        inline PSubSkillPSI()
        {
            mPPU = mAPU = mMST = mPPW = mPSR = 0;
        }
    };

    struct PSkills
    {
        PMainSkills *Main;
        PSubSkillINT *INT;
        PSubSkillCON *CON;
        PSubSkillSTR *STR;
        PSubSkillDEX *DEX;
        PSubSkillPSI *PSI;

        inline PSkills()
        {
            Main = new PMainSkills();
            INT = new PSubSkillINT();
            CON = new PSubSkillCON();
            STR = new PSubSkillSTR();
            DEX = new PSubSkillDEX();
            PSI = new PSubSkillPSI();
        }

        inline ~PSkills()
        {
            delete Main;
            delete INT;
            delete CON;
            delete STR;
            delete DEX;
            delete PSI;
        }
    };

    PSkills m_Skills;
    bool IncSubSkillPossible(SUB_SKILLS Skill);

public:

    void SetSubSkill(SUB_SKILLS Skill, int value);
    void SetMainSkill(MAIN_SKILLS Skill, int value);
    void SetXP(MAIN_SKILLS Skill, float value);
    void SetSP(MAIN_SKILLS Skill, short value);

    int GetMainSkill(MAIN_SKILLS Skill);
    int GetSubSkill(SUB_SKILLS Skill);
    int GetSKPCost(SUB_SKILLS Skill);

    // Get Mainskill from given Subskill, client doesn't tell us this
    MAIN_SKILLS GetMSfromSS(SUB_SKILLS Skill);

    bool IsValidSubSkill(SUB_SKILLS Skill);
    bool IsValidMainSkill(MAIN_SKILLS Skill);

    void ZeroMSSubSkills(MAIN_SKILLS Skill);

    int IncreaseSubSkill(SUB_SKILLS Skill);

    uint16_t GetSP(MAIN_SKILLS Skill);
    float GetXP(MAIN_SKILLS Skill);
};
