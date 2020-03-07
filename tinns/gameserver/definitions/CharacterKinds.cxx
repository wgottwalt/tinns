#include <cstring>
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"

// charkinds are character templates used for player char creation

PDefCharKind::PDefCharKind()
{
    mSkillInfo = 0;
    memset(mInventory, 0, sizeof(mInventory));
}

PDefCharKind::~PDefCharKind()
{
    delete [] mSkillInfo;
    for(PSkillPtsMap::const_iterator i=mSkillPts.begin(); i!=mSkillPts.end(); i++)
        delete i->second;
    for(PSubSkillPtsMap::const_iterator i=mSubSkillPts.begin(); i!=mSubSkillPts.end(); i++)
        delete i->second;
    for(PStartLevelMap::const_iterator i=mStartLevels.begin(); i!=mStartLevels.end(); i++)
        delete i->second;
}

bool PDefCharKind::LoadFromDef(PTokenList *Tokens)
{
    int NumSkills = GameDefs->Skills()->GetNumDefs();
    //int NumSubSkills = GameDefs->SubSkills()->GetNumDefs();

    int SkillInfoStart = 4;
    int TrainPtsStart = SkillInfoStart+NumSkills*3;
    int LevelsStart = TrainPtsStart + 32*2;
    int MoneyStart = LevelsStart + 16*2;
    int InventoryStart = MoneyStart+1;

    mSkillInfo = new PSkillInfo[NumSkills];

    int Idx=0;
    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        switch(Idx)
        {
            case 0 :    // setentry
                continue;

            case 1 :
                mIndex = atoi(i->c_str()); break;

            case 2 :
                mName = *i; break;

            case 3 :
                mType = atoi(i->c_str()); break;

        }

        if((Idx >= SkillInfoStart) && (Idx < TrainPtsStart)) // skill info
        {
            int SkillIdx = (Idx-SkillInfoStart)/3;
            int Value = atoi(i->c_str());

            // start, max, grow per skill
            switch((Idx-SkillInfoStart)%3)
            {
                case 0 : mSkillInfo[SkillIdx].mStart = Value; break;
                case 1 : mSkillInfo[SkillIdx].mMax = Value; break;
                case 2 : mSkillInfo[SkillIdx].mGrow = Value; break;
            }
        } else
        // 32 skill/subskill train pts
        if((Idx >= TrainPtsStart) && (Idx < LevelsStart))
        {
            static int SkillIndex = 0;
            int Index = Idx-TrainPtsStart;
            if((Index&1)==0)
            {
                SkillIndex = atoi(i->c_str());
            } else
            {
                if(SkillIndex >= 1000)  // skill
                {
                    const PDefSkill *Skill = GameDefs->Skills()->GetDef(SkillIndex-1000);
                    if(Skill)
                    {
                        int Index = Skill->GetIndex()-1;
                        PSkillPtsInfo *CurrentSkillPts = new PSkillPtsInfo();
                        CurrentSkillPts->mSkill = Index;
                        CurrentSkillPts->mPoints = atoi(i->c_str());
                        mSkillPts.insert(std::make_pair(Index, CurrentSkillPts));
                    } else
                    {
                        if(SkillIndex-1000 != 0)
                            Console->Print("Charkind def: invalid skill index %i", SkillIndex-1000);
                    }
                } else  // subskill
                {
                    const PDefSubSkill *SubSkill = GameDefs->SubSkills()->GetDef(SkillIndex);
                    if(SubSkill)
                    {
                        int Index = SubSkill->GetIndex()-1;
                        PSubSkillPtsInfo *CurrentSubSkillPts = new PSubSkillPtsInfo();
                        CurrentSubSkillPts->mSubSkill = Index;
                        CurrentSubSkillPts->mPoints = atoi(i->c_str());
                        mSubSkillPts.insert(std::make_pair(Index, CurrentSubSkillPts));
                    } else
                    {
                        if(SkillIndex != 0)
                            Console->Print("Charkind def: invalid subskill index %i", SkillIndex);
                    }
                }
            }
        } else
        // 16 subskill start levels
        if((Idx >= LevelsStart) && (Idx < MoneyStart))
        {
            static int LevelIndex = 0;
            int Index = Idx-NumSkills*3+4+(32*2);
            if((Index&1)==0)
            {
                LevelIndex = atoi(i->c_str());
            } else
            {
                if(LevelIndex > 0)
                {
                    const PDefSubSkill *SubSkill = GameDefs->SubSkills()->GetDef(LevelIndex);
                    if(SubSkill)
                    {
                        PStartLevelInfo *Level = new PStartLevelInfo();
                        Level->mSubSkill = SubSkill->GetIndex();
                        Level->mLevel = atoi(i->c_str());
                        mStartLevels.insert(std::make_pair(Level->mSubSkill, Level));
                    } else
                    {
                        Console->Print("Charkind def: invalid subskill index %i", LevelIndex);
                    }
                }
            }
        } else
        // money
        if((Idx >= MoneyStart) && (Idx < InventoryStart))
        {
            mMoney = atoi(i->c_str());
        } else
        // inventory
        if((Idx >= InventoryStart) && (Idx < InventoryStart+8))
        {
            mInventory[Idx-InventoryStart] = atoi(i->c_str());
        }
    }
    
    return true;
}

