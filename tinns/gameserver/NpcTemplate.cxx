#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PNPCTemplate::PNPCTemplate()
{
    mUnknown1 = 0;
    mPosY = 0.0f;
    mPosZ = 0.0f;
    mPosX = 0.0f;
    mNPCTypeID = 0;
    mActorStringSize = 0;
    mAngleStringSize = 0;
    mNpcID = 0;
    mHasAdditionalCoords = 0;
    mUnknown2a = 0;
    mUnknown2b = 0;
    mUnknown2c = 0;
    /*mUnknown3*/mTradeID = 0;
    mUnknown4 = 0;
    mActorName = "";
    mAngle = "";
}

PNPCTemplate::~PNPCTemplate()
{
    // Cleanup waypoints
    for ( PWaypointMap::iterator i = mWayPoints.begin(); i != mWayPoints.end(); i++ )
        delete i->second;
}

void PNPCTemplate::AddWayPoint(float nX, float nY, float nZ, uint8_t nID)
{
    struct s_floatcoords *tWayPoint = new s_floatcoords;
    tWayPoint->mX = nX;
    tWayPoint->mY = nY;
    tWayPoint->mZ = nZ;

    if ( mWayPoints.insert( std::make_pair( nID, tWayPoint)).second )
        if ( gDevDebug ) Console->Print( "%s Waypoint %d (X %0.1f Y %0.1f Z %0.1f) added to NPC", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nID, nX, nY, nZ );
}
