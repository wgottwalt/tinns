#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

PWorldDataTemplate::PWorldDataTemplate()
{
    mUseCount = 0;
    for ( int i = 0; i < WORLDDATATEMPLATE_MAXPOSITEMS; ++i )
    {
        mPositionItems[i] = nullptr;
    }
}

PWorldDataTemplate::~PWorldDataTemplate()
{
    DatFileDataCleanup();
}

void PWorldDataTemplate::DatFileDataCleanup()
{
    for ( PFurnitureItemsMap::iterator i = mFurnitureItems.begin(); i != mFurnitureItems.end(); i++ )
        delete i->second;
    for ( PDoorsMap::iterator i = mDoors.begin(); i != mDoors.end(); i++ )
        delete i->second;
    for ( PNPCsMap::iterator i = mNPCs.begin(); i != mNPCs.end(); i++ )
        delete i->second;
}

bool PWorldDataTemplate::LoadDatFile( const std::string& WorldTemplateName, const std::string& nFilename, const bool nTestAccesOnly )
{
    PWorldDatParser WDatLoader;
    int LoadResult;

    DatFileDataCleanup();
    if ( gDevDebug ) Console->Print( "%s Loading %s", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nFilename.c_str() );
    LoadResult = WDatLoader.LoadDatFile( nFilename, this, true, nTestAccesOnly ) ; // We want to discard passive objects for now
    switch ( LoadResult )
    {
    case 0:
    {
        mName = nFilename;
        mBspName = WorldTemplateName;
        if ( !nTestAccesOnly )
        {
            SetLinkedObjects(); // temp until better solution found from .dat & .bsp files
        }
        return true;
    }

    case -1:
    {
        if ( !nTestAccesOnly )
            Console->Print( "%s Can't read file %s", Console->ColorText( RED, BLACK, "[ERROR]" ), nFilename.c_str() );
        break;
    }

    case - 2:
    {
        Console->Print( "%s Bad data", Console->ColorText( RED, BLACK, "[ERROR]" ) );
        break;
    }

    case - 3:
    {
        Console->Print( "%s Unexpected end of file", Console->ColorText( RED, BLACK, "[ERROR]" ) );
        break;
    }

    default:
        Console->Print( "%s Unknown error %d", Console->ColorText( RED, BLACK, "[ERROR]" ), LoadResult );
    }
    return false;
}

uint32_t PWorldDataTemplate::AddFurnitureItem( PFurnitureItemTemplate* nItem )
{
    if ( nItem )
    {
        if ( mFurnitureItems.insert( std::make_pair( nItem->GetID(), nItem ) ).second )
        {
            if ( gDevDebug ) Console->Print( "%s Furniture item %d added to world template", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nItem->GetID() );

            if ( nItem->GetFunctionType() == 21 )
            {
                int v = nItem->GetFunctionValue();
                if (( v >= 0 ) && ( v < WORLDDATATEMPLATE_MAXPOSITEMS ) )
                {
                    if ( mPositionItems[v] )
                    {
                        if (( v == WORLDDATATEMPLATE_MAXPOSITEMS - 2 ) && !mPositionItems[v+1] ) // We allow that only for Pos 9 in order not to mess with other pos
                        {
                            Console->Print( "%s Same position %d for two position items ID %d and %d. Last one will be put on next position.", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), v, mPositionItems[v]->GetID(), nItem->GetID() );
                            ++v;
                        }
                        else
                        {
                            Console->Print( "%s Same position %d for two position items ID %d and %d. Only last one kept.", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), v, mPositionItems[v]->GetID(), nItem->GetID() );
                        }
                    }

                    mPositionItems[v] = nItem;
                    /*
                    Console->Print("Position entity %d (id 0x%x) added to world template", v, nItem->GetID());
                    float fpX, fpY, fpZ;
                    uint16_t pX, pY, pZ;
                    nItem->GetPos(&fpX, &fpY, &fpZ);
                    pX = (uint16_t) (fpX + 32000);
                    pY = (uint16_t) (fpY + 32000);
                    pZ = (uint16_t) (fpZ + 32000);
                    Console->Print("Position Y=%f (0x%04x) Z=%f (0x%04x) X=%f (0x%04x)", fpY, pY, fpZ, pZ, fpX, pX);
                    */
                }
                else
                {
                    Console->Print( "%s Invalid position %d for position item ID %d. Position ignored.", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), v, nItem->GetID() );
                }
            }
            return nItem->GetID();
        }
        else
        {
            Console->Print( "%s Duplicate furniture item ID %d !!! Not added to world template", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), nItem->GetID() );
        }
    }
    return 0;
}

const PFurnitureItemTemplate* PWorldDataTemplate::GetFurnitureItem( uint32_t ItemID )
{
    PFurnitureItemsMap::const_iterator it = mFurnitureItems.find( ItemID );
    if ( it == mFurnitureItems.end() )
        return nullptr;
    else
        return it->second;
}

bool PWorldDataTemplate::getPositionItemPosition( uint8_t PosID, float* pX, float* pY, float* pZ )
{
    if (( PosID < WORLDDATATEMPLATE_MAXPOSITEMS ) && mPositionItems[PosID] )
    {
        mPositionItems[PosID]->GetPos( pX, pY, pZ ) ;
        return true;
    }
    return false;
}

uint32_t PWorldDataTemplate::AddDoor( PDoorTemplate* nDoor )
{
    if ( nDoor )
    {
        if ( mDoors.insert( std::make_pair( nDoor->GetID(), nDoor ) ).second )
        {
            if ( gDevDebug ) Console->Print( "%s Door %d added to world template", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nDoor->GetID() );
            return nDoor->GetID();
        }
        else
        {
            Console->Print( "%s Duplicate Door ID %d !!! Not added to world template", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), nDoor->GetID() );
        }
    }
    return 0;
}

const PDoorTemplate* PWorldDataTemplate::GetDoor( uint32_t DoorID )
{
    PDoorsMap::const_iterator it = mDoors.find( DoorID );
    if ( it == mDoors.end() )
        return nullptr;
    else
        return it->second;
}


uint32_t PWorldDataTemplate::AddNPC( PNPCTemplate* nNPC )
{
    if ( nNPC )
    {
        if ( mNPCs.insert( std::make_pair( nNPC->GetNpcID(), nNPC ) ).second )
        {
            if ( gDevDebug ) Console->Print( "%s NPC %d added to world template", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nNPC->GetNpcID() );
            return nNPC->GetNpcID();
        }
        else
        {
            Console->Print( "%s Duplicate NPC ID %d !!! Not added to world template", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), nNPC->GetNpcID() );
        }
    }
    return 0;
}

const PNPCTemplate* PWorldDataTemplate::GetNPC( uint32_t NPCID )
{
    PNPCsMap::const_iterator it = mNPCs.find( NPCID );
    if ( it == mNPCs.end() )
        return nullptr;
    else
        return it->second;
}

void PWorldDataTemplate::SetLinkedObjects()
{
    float xI, yI, zI;
    float xD, yD, zD;
    float D2, minD2;
    uint32_t minObjID;
    uint16_t fnctType;
    uint16_t tGROrder = 0;

    for ( PFurnitureItemsMap::iterator it = mFurnitureItems.begin(); it != mFurnitureItems.end(); it++ )
    {
        fnctType = it->second->GetFunctionType();
        if (( fnctType == 11 ) || ( fnctType == 12 ) || ( fnctType == 13 ) || ( fnctType == 23 ) ) // if function is apt entry button, door access if, hack button or money button
        {
            it->second->GetPos( &xI, &yI, &zI );
//Console->Print("Button pos: %0.0f %0.0f %0.0f", xI, yI, zI);
            minD2 = 1e9;
            minObjID = 0;
            for ( PDoorsMap::iterator dit = mDoors.begin(); dit != mDoors.end(); dit++ )
            {
//Console->Print("%s Found door %d (%s) : %s triggered, %s", Console->ColorText(GREEN, BLACK, "[Debug]"), dit->first, dit->second->GetName().c_str(), (dit->second->IsTriggeredDoor()?"":"not"), (dit->second->IsDoubleDoor()?"double":"single") );
                if ( dit->second->IsTriggeredDoor() )
                {
                    dit->second->GetPos( &xD, &yD, &zD );
//Console->Print("Door pos: %0.0f %0.0f %0.0f", xD, yD, zD);
                    D2 = ( xI - xD ) * ( xI - xD ) + ( yI - yD ) * ( yI - yD ) + ( zI - zD ) * ( zI - zD );
//Console->Print("Dist D2:%0.0f minD2:%0.0f", D2, minD2);
                    if ( D2 < minD2 )
                    {
                        minD2 = D2;
                        minObjID = 1 + dit->first;
                    }
                }
            }
            if ( minObjID-- )
            {
                it->second->SetLinkedObjectID( minObjID );
                if ( gDevDebug ) Console->Print( "%s Found triggered door %d (%s) for button %d (%s)", Console->ColorText( GREEN, BLACK, "[Debug]" ), minObjID, GetDoor( minObjID )->GetName().c_str(), it->first, it->second->GetName().c_str() );

            }
            else
            {
                Console->Print( "%s No triggered door found for button %d (%s) in World data template %s", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), it->first, it->second->GetName().c_str(), this->GetName().c_str() );
            }
        }
        else if ( fnctType == 6 ) // if function is genrep
        {
            it->second->SetLinkedObjectID( ++tGROrder );
        }
    }

}
