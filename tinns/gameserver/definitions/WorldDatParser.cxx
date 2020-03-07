#include <cmath>
#include <cstring>
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"

const uint16_t nonDiscardUseFlags = ufTouchable | ufUsable | ufChair | ufToolTarget ; // furniture always to keep even if function type = 0

PWorldDatParser::PWorldDatParser()
{
    f = nullptr;
}

PWorldDatParser::~PWorldDatParser()
{

}

int PWorldDatParser::LoadDatFile( const std::string& nFilename, PWorldDataTemplate* nWorld, const bool nDiscardPassiveObjects, const bool nTestAccesOnly )
{
    PWorldFileHeader FileHeader;
    PSectionHeader SectionHeader;
    PSec2ElemHeader Sec2ElemHeader;

    uint32_t FileLen;
    uint32_t NextSectionOffset = 0;
    uint32_t NextElementOffset;
    bool ProcessOK;

    mWorld = nWorld;
    mDiscardPassiveObjects = nDiscardPassiveObjects;

    if ( mNCDataPath == "" )
    {
        mNCDataPath = Config->GetOption( "nc_data_path" );
    }

    if ( gDevDebug )
        Console->Print( "Reading file %s/%s", mNCDataPath.c_str(), nFilename.c_str() );
    f = Filesystem->Open( "", nFilename.c_str(), mNCDataPath );

    if ( nTestAccesOnly )
    {
        ProcessOK = ( bool )f;
        Filesystem->Close( f );
        return ( ProcessOK ? 0 : -1 );
    }

    if ( f )
    {
        FileLen = f->GetSize();

        // Section 1
        if (gDevDebug)
            Console->Print( "Reading file header (section 1) ... " );
        f->Read( &FileHeader, sizeof( PWorldFileHeader ) );
        if (( FileHeader.mHeaderSize != 0x00000008 )
                || ( FileHeader.mHeaderSig != 0x000fcfcf )
                || ( FileHeader.mSection != 0x00000001 ) )
        {
            if (gDevDebug)
                Console->Print( "Read header: %08x / %08x / %08x",  FileHeader.mHeaderSize, FileHeader.mHeaderSig, FileHeader.mSection);
            Filesystem->Close( f );
            return -2;
        }
        NextSectionOffset += FileHeader.mHeaderSize + 4;

        // Other Sections
        //  Header
        while ( ! f->Eof() )
        {
            f->Seek( NextSectionOffset ); // Make sure we are at the computed offset
            if (gDevDebug)
                Console->Print( "Reading next section header ... " );
            if (( uint32_t )( f->Read( &SectionHeader, sizeof( PSectionHeader ) ) ) < sizeof( PSectionHeader ) )
            {
                Filesystem->Close( f );
                return -3;
            }

            if (( SectionHeader.mHeaderSize != 0x0000000c ) || ( SectionHeader.mHeaderSig != 0x0000ffcf ) )
            {
                Filesystem->Close( f );
                return -2;
            }

            if ( SectionHeader.mSection == 0 )
            {
                if (gDevDebug)
                    Console->Print( "Ending section reached" );
                break;
            }

            NextElementOffset = NextSectionOffset + SectionHeader.mHeaderSize + 4;
            NextSectionOffset = NextElementOffset + SectionHeader.mDataSize;
            if (gDevDebug)
                Console->Print( "Processing section %d (size %d)", SectionHeader.mSection, SectionHeader.mDataSize );

            if ( SectionHeader.mSection == 2 )
            {
                //int cnt=0;
                if ( gDevDebug )
                    Console->Print( "Element Type 3 size: %d or %d", sizeof( PSec2ElemType3a ), sizeof( PSec2ElemType3a ) + sizeof( PSec2ElemType3b ) );
                while ( NextElementOffset < NextSectionOffset )
                {
                    f->Seek( NextElementOffset ); // Make sure we are at the computed offset

                    //if ( gDevDebug )
                    //    Console->Print( "Reading next element header ... " );
                    if (( uint32_t )( f->Read( &Sec2ElemHeader, sizeof( PSec2ElemHeader ) ) ) < sizeof( PSec2ElemHeader ) )
                    {
                        Filesystem->Close( f );
                        return -3;
                    }
                    if (( Sec2ElemHeader.mHeaderSize != 0x0000000c ) || ( Sec2ElemHeader.mHeaderSig != 0x0ffefef1 ) )
                    {
                        Filesystem->Close( f );
                        return -2;
                    }
                    NextElementOffset += ( Sec2ElemHeader.mHeaderSize + 4 + Sec2ElemHeader.mDataSize );
                    //if (gDevDebug) Console->Print("Found element %d of type %d, size %d", ++cnt, Sec2ElemHeader.mElementType, Sec2ElemHeader.mDataSize);
                    switch ( Sec2ElemHeader.mElementType )
                    {
                    case 1000003:
                    {
                        ProcessOK = ProcessSec2ElemType3( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    case 1000005:
                    {
                        ProcessOK = ProcessSec2ElemType5( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    case 1000006:
                    {
                        ProcessOK = ProcessSec2NPCEntry( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    default:
                    {
                        if (gDevDebug) Console->Print( "Ignoring SectionID %d, not supportet yet", Sec2ElemHeader.mElementType );
                        ProcessOK = true;
                        break;
                    }
                    }

                    if ( !ProcessOK )
                        return -4;
                }
            }
            else
            {
                if ( gDevDebug )
                    Console->Print( "Section %d ignored", SectionHeader.mSection );
                continue;
            }

        }

        Filesystem->Close( f );
    }
    else
    {
        return -1;
    }

    return 0;
}

bool PWorldDatParser::ProcessSec2ElemType3( uint32_t nSize ) // furniture
{
    PSec2ElemType3a DataA;
    PSec2ElemType3b DataB;
    const PDefWorldModel* nWorldModel;
    std::string nName;
    const uint32_t sza = sizeof( PSec2ElemType3a );
    const uint32_t szb = sizeof( PSec2ElemType3a ) + sizeof( PSec2ElemType3b );

    if (( nSize != szb ) && ( nSize != sza ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType3 (%d read vs %d or %d needed", nSize, sza, szb );
        return false;
    }
    if (( uint32_t )( f->Read( &DataA, sza ) ) < sza )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3a" );
        return false;
    }
    if ( nSize == szb )
    {
        if (( uint32_t )( f->Read( &DataB, sizeof( PSec2ElemType3b ) ) ) < sizeof( PSec2ElemType3b ) )
        {
            Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3b" );
            return false;
        }
    }
    else
    {
        DataB.mBoxLowerY = DataB.mBoxLowerZ = DataB.mBoxLowerX = 0;
        DataB.mBoxUpperY = DataB.mBoxUpperZ = DataB.mBoxUpperX = 0;
    }

    if ( DataA.mWorldmodelID )
    {
        nWorldModel = GameDefs->WorldModels()->GetDef( DataA.mWorldmodelID );
        if ( nWorldModel )
            nName = nWorldModel->GetName();
        else
            nName = "UNKNOWN";
    }
    else
    {
        nName = "PASSIVE";
        nWorldModel = nullptr;
    }

    /* if (gDevDebug) {
    Console->Print("-------------------------------------------------------");
    Console->Print("%s (%d) : ID %d", nName.c_str(), DataA.mWorldmodelID, DataA.mObjectID);
    if (!nWorldModel) Console->Print("y:%f z:%f x:%f model %d", DataA.mPosY , DataA.mPosZ, DataA.mPosX, DataA.mModelID);
    Console->Print("Scale:%f Uk2:0x%08x Uk3:0x%08x", DataA.mScale, DataA.mUnknown2, DataA.mUnknown3);
    Console->Print("Uk4:0x%08x Uk5:0x%04x", DataA.mUnknown4, DataA.mUnknown5);
    //Console->Print("Ly:%f Lz:%f Lx:%f", DataB.mBoxLowerY, DataB.mBoxLowerZ, DataB.mBoxLowerX);
    //Console->Print("Uy:%f Uz:%f Ux:%f", DataB.mBoxUpperY, DataB.mBoxUpperZ, DataB.mBoxUpperX);
    }*/

    if (( !nWorldModel || ( !nWorldModel->GetFunctionType() && !( nWorldModel->GetUseFlags() & nonDiscardUseFlags ) ) ) && mDiscardPassiveObjects )
    {
        //if ( gDevDebug )
        //    Console->Print( "Discarded" );
        return true;
    }

    PFurnitureItemTemplate* nItem = new PFurnitureItemTemplate;
    nItem->mObjectID = DataA.mObjectID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    nItem->mPosY = DataA.mPosY; // float pos values are kept 0-centered
    nItem->mPosZ = DataA.mPosZ;
    nItem->mPosX = DataA.mPosX;
//  nItem->mRotY = DataA.mRotY;
    nItem->mRotZ = DataA.mRotZ;
//  nItem->mRotX = DataA.mRotX;
//  nItem->mScale = DataA.mScale;
//  nItem->mUnknown2 = DataA.mUnknown2;
    nItem->mModelID = DataA.mModelID;
//  nItem->mUnknown3 = DataA.mUnknown3;
//  nItem->mUnknown4 = DataA.mUnknown4;
    nItem->mWorldmodelID = DataA.mWorldmodelID;
//  nItem->mUnknown5 = DataA.mUnknown5;

//  nItem->mBoxLowerY = DataB.mBoxLowerY;
//  nItem->mBoxLowerZ = DataB.mBoxLowerZ;
//  nItem->mBoxLowerX = DataB.mBoxLowerX;
//  nItem->mBoxUpperY = DataB.mBoxUpperY;
//  nItem->mBoxUpperZ = DataB.mBoxUpperZ;
//  nItem->mBoxUpperX = DataB.mBoxUpperX;

    nItem->mDefWorldModel = nWorldModel;
    /*uint16_t func=nWorldModel->GetFunctionType();
    if((func==18) || (func==20) || (func==29))
    Console->Print("gate model: %d", DataA.mWorldmodelID);*/

    float Angle = ( 180 + DataA.mRotZ ) * 3.14159 / 180;
    float Radius = abs(( int )(( DataB.mBoxUpperX - DataB.mBoxLowerX ) / 2 ) );
    if ( Radius == 0 )
    {
        Radius = 10;
    }
    Radius *= DataA.mScale;
    Radius += 5;

    // int pos values are change to match char pos scale (32000 centered)
    nItem->mFrontPosY = ( uint16_t )( 32000 + DataA.mPosY + Radius * sinf( Angle ) );
    nItem->mFrontPosZ = ( uint16_t )( 32000 + DataA.mPosZ );
    nItem->mFrontPosX = ( uint16_t )( 32000 + DataA.mPosX + Radius * cosf( Angle ) );
    nItem->mFrontLR = ( uint8_t )( 0.5 * ( DataA.mRotZ + ( DataA.mRotZ < 0 ? 360 : 0 ) ) );

    mWorld->AddFurnitureItem( nItem );

    return true;
}

bool PWorldDatParser::ProcessSec2ElemType5( uint32_t nSize ) // doors
{
    PSec2ElemType5Start Data;
    char StringData[64];

    const PDefWorldModel* nWorldModel;
    std::string nName;
    char* ActorString;
    char* ParamString;

    const uint32_t sza = sizeof( PSec2ElemType5Start );

    if (( nSize < sza ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType5 (%d read vs %d needed", nSize, sza );
        return false;
    }
    if (( uint32_t )( f->Read( &Data, sza ) ) < sza )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType5start" );
        return false;
    }
    uint32_t szb = Data.mActorStringSize  + Data.mParamStringSize;

    if ( nSize != ( sza + szb ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType5 (%d read vs %d needed", nSize, sza + szb );
        return false;
    }
    else
    {
        if ( szb > 64 )
        {
            Console->Print( RED, BLACK, "[Warning] String data too long in Sec2ElemType5 End String. End will be ignored" );
            szb = 64;
        }
        if (( uint32_t )( f->Read( StringData, szb ) ) < szb )
        {
            Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType5 End Strings" );
            return false;
        }
    }

    if ( Data.mWorldmodelID )
    {
        nWorldModel = GameDefs->WorldModels()->GetDef( Data.mWorldmodelID );
        if ( nWorldModel )
            nName = nWorldModel->GetName();
        else
            nName = "UNKNOWN";
    }
    else
    {
        nName = "PASSIVE";
        nWorldModel = nullptr;
    }

    StringData[Data.mActorStringSize - 1] = 0;
    ActorString = StringData;
    StringData[szb - 1] = 0;
    ParamString = StringData + Data.mActorStringSize;
    /*
        if ( gDevDebug )
        {
            Console->Print( "-------------------------------------------------------" );
            Console->Print( "Door %s (%d) : ID %d", nName.c_str(), Data.mWorldmodelID, Data.mDoorID );
            Console->Print( "y:%f z:%f x:%f", Data.mPosY , Data.mPosZ, Data.mPosX );
            Console->Print( "Uk1:0x%04x Uk1bis:0x%04x Uk5:0x%04x", Data.mUnknown1, Data.mUnknown1bis, Data.mUnknown5 );
            Console->Print( "Type=%s Param=%s", ActorString, ParamString );
        }
    */
// Let's keep knowledge of doors even without models !
    /*  if ((!nWorldModel || (!nWorldModel->GetFunctionType() && !(nWorldModel->GetUseFlags() & nonDiscardUseFlags))) && mDiscardPassiveObjects)
      {
    if (gDevDebug) Console->Print("Discarded");
    if (gDevDebug)
    {
    Console->Print("Door %s (%d) : ID %d", nName.c_str(), Data.mWorldmodelID, Data.mDoorID);
    Console->Print("Type=%s Param=%s", ActorString, ParamString);
    }
        return true;
      }*/

    PDoorTemplate* nDoor = new PDoorTemplate;
    nDoor->mDoorID = Data.mDoorID;

    //nDoor->mUnknown1 = Data.mUnknown1; //18 00
    //nDoor->mUnknown1bis = Data.mUnknown1bis; //00 00 ? varies
    nDoor->mPosY = Data.mPosY;
    nDoor->mPosZ = Data.mPosZ;
    nDoor->mPosX = Data.mPosX;
    //nDoor->mUnknown5 = Data.mUnknown5; //00 00 ? second byte varies
    nDoor->mWorldmodelID = Data.mWorldmodelID; //door type from worldmodel.def
    nDoor->mDefWorldModel = nWorldModel;

    nDoor->SetDoorTypeName( ActorString );
    nDoor->SetDoorParameters( ParamString );

    mWorld->AddDoor( nDoor );

    return true;
}

bool PWorldDatParser::ProcessSec2NPCEntry( uint32_t nSize )
{
    PSec2NPC_EntryPart1         tNPCPartA;
    PSec2NPC_EntryPart2         tNPCPartB;
    std::string                 tActorName;
    std::string                 tAngle;
    char                        tStrBuffer[64];

    uint32_t tSizeOfA = sizeof(tNPCPartA);
    uint32_t tSizeOfB = sizeof(tNPCPartB);

    // Are we able to read enough bytes from the file? means: CAN we safely read our entire struct from the file?
    if ( nSize < tSizeOfA )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for PSec2NPC_EntryPart1 (%d read vs %d needed", nSize, tSizeOfA );
        return false;
    }
    // yes we can! So read it now. If we reach EOF, break
    if (( uint32_t )( f->Read( &tNPCPartA, tSizeOfA ) ) < tSizeOfA )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry" );
        return false;
    }
    // Now we have the header. lets check if we have some waypoints for this NPC
    // Size of entire NPC entry in file
    uint32_t tCompleteNPCSize = tSizeOfA + tNPCPartA.mActorStringSize + tNPCPartA.mAngleStringSize;
    if ( tNPCPartA.mHasAdditionalCoords > 0 )
    {
        // It has additional coords, add 'em
        tCompleteNPCSize += tSizeOfB*tNPCPartA.mHasAdditionalCoords;
    }

    // Do a last check if we're on the correct size
    if ( nSize != tCompleteNPCSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for PSec2NPC_Entry (%d available vs %d expected", nSize, tCompleteNPCSize );
        Console->Print( RED, BLACK, "NPC ID was: %d", tNPCPartA.mNpcID);
        return false;
    }
    // We are. Continue reading!
    // Assign the 2 strings and watch out for EOF!
    memset(tStrBuffer, 0, 64);
    if (( uint32_t )( f->Read( tStrBuffer, tNPCPartA.mActorStringSize ) ) < tNPCPartA.mActorStringSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry mActorStringSize" );
        return false;
    }
    tActorName = tStrBuffer;

    memset(tStrBuffer, 0, 64);
    if (( uint32_t )( f->Read( tStrBuffer, tNPCPartA.mAngleStringSize ) ) < tNPCPartA.mAngleStringSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry mAngleStringSize" );
        return false;
    }
    tAngle = tStrBuffer;

    // We're done. Now create new NPC entry
    PNPCTemplate* tNPC = new PNPCTemplate;

    // and assing all that stuff
    tNPC->SetUnknown1(tNPCPartA.mUnknown1);
    tNPC->SetPosX(tNPCPartA.mPosX);
    tNPC->SetPosY(tNPCPartA.mPosY);
    tNPC->SetPosZ(tNPCPartA.mPosZ);
    tNPC->SetNPCTypeID(tNPCPartA.mNPCTypeID);
    tNPC->SetActorStrSize(tNPCPartA.mActorStringSize);
    tNPC->SetAngleStrSize(tNPCPartA.mAngleStringSize);
    tNPC->SetNpcID(tNPCPartA.mNpcID);
    tNPC->SetUnknown2a(tNPCPartA.mUnknown2a);
    tNPC->SetUnknown2b(tNPCPartA.mUnknown2b);
    tNPC->SetUnknown2c(tNPCPartA.mUnknown2c);
    tNPC->SetTradeID/*SetUnknown3*/(tNPCPartA.mTradeID/*mUnknown3*/);
    tNPC->SetUnknown4(tNPCPartA.mUnknown4);

    tNPC->SetActorName(tActorName);
    tNPC->SetAngle(tAngle);

    // Read additional Waypoints if available
    uint8_t tCurrWayP = 0;
    if ( tNPCPartA.mHasAdditionalCoords > 0 )
    {
        while ( tCurrWayP < tNPCPartA.mHasAdditionalCoords )
        {
            memset(&tNPCPartB, 0, tSizeOfB);
            if (( uint32_t )( f->Read( &tNPCPartB, tSizeOfB ) ) < tSizeOfB )
            {
                Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry while reading WayPoints" );
                return false;
            }
            tNPC->AddWayPoint(tNPCPartB.mPosX, tNPCPartB.mPosY, tNPCPartB.mPosZ, tCurrWayP);
            tCurrWayP++;
        }
    }

    if (gDevDebug) Console->Print("Added NPC ID %d", tNPCPartA.mNpcID);

    mWorld->AddNPC(tNPC);
    return true;
}
