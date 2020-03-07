#pragma once

#include <cstdint>
#include <string>
#include <vector>

class PClient;
class PDefDrug;
class PMessage;
class PContainerEntry;
class PContainer;
class PSpawnedVehicle;

class PMsgBuilder {
public:
    PMsgBuilder() {};
    ~PMsgBuilder() {};

// Following methods for broadcasted messages do NOT include UDP_ID increment
// when needed, nor UDP_ID / SessionID setting
// as these must be set on a destination client basis
    PMessage* BuildCharHelloMsg( PClient* nClient );
    PMessage* BuildCharHealthUpdateMsg( PClient* nClient );
    PMessage* BuildCharDeathMsg( PClient* nClient, uint32_t nKillerCharId = 0 );
    PMessage* BuildCharPosUpdateMsg( PClient* nClient );
    PMessage* BuildCharPosUpdate2Msg( PClient* nClient, uint8_t InfoBitfield = 0x7f );
    //PMessage* BuildCharSittingMsg( PClient* nClient ); // Not used anymore, done by BuildCharPosUpdateMsg
    PMessage* BuildCharExitSeatMsg( PClient* nClient );
    PMessage* BuildDoorOpenMsg( uint32_t nRawItemID, bool nDoubleDoor = false );
    PMessage* BuildCharUseSeatMsg( PClient* nClient, uint32_t nRawObjectId, uint8_t nSeatId = 0 );
    PMessage* BuildCharShowGlowCircleMsg( PClient* nClient );

    PMessage* BuildSpawnWorldObjectMsg( uint16_t nModelID, uint16_t nFunctionID, uint32_t nWOID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ );
    PMessage* BuildRemoveWorldObjectMsg( uint32_t nWOID );
    PMessage* BuildWeatherControlMsg( uint16_t nWeatherId );

    PMessage* BuildSubwaySingleUpdateMsg( uint32_t nVehicleID, uint16_t nPosition, uint8_t nDoorOpened );
    PMessage* BuildVhcHealthUpdateMsg( PSpawnedVehicle* nVehicle );
    PMessage* BuildVhcPosUpdateMsg( PSpawnedVehicle* nVehicle );
    PMessage* BuildVhcPosUpdate2Msg( PSpawnedVehicle* nVehicle );

    PMessage* BuildStartWeaponReloadAnimMsg( PClient* nClient );
    PMessage* BuildHeldItemUseMsg( uint16_t nUserCharLocalId, uint16_t nWeaponId, uint32_t nTargetRawItemID, uint8_t nAiming, uint8_t nTargetedHeight, uint8_t nScore = 0 );
    PMessage* BuildHeldItemUse2Msg( uint16_t nUserCharLocalId, uint32_t nTargetRawItemID );
    PMessage* BuildHeldItemUse4Msg( uint16_t nUserCharLocalId, uint32_t nTargetRawItemID, uint16_t nUnknown1, uint16_t nUnknown2, uint8_t nTargetedHeight );
    PMessage* BuildHeldItemAddonActivationMsg( PClient* nClient, uint8_t nState );

    // Temp. NPC update message for testing
    PMessage* BuildNpcDeathMsg( PClient* nClient, uint32_t nNpcId, uint8_t unknown1 = 0x4a, uint8_t npcAction = 0x1e );

    PMessage* BuildNPCMassInfoMsg( uint32_t nWorldID, uint16_t nTypeID, uint16_t nClothing, uint16_t nNameID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint16_t nHealth, uint16_t nTraderID, std::string *nAngleStr, std::string *nNpcName, std::string *nCustomName);
    PMessage* BuildNPCMassAliveMsg( uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint8_t nAction );
    PMessage* BuildNPCMassUpdateMsg( uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint16_t nTarget, uint8_t nAction );
    // Moved here since its a zone broadcast!
    PMessage* BuildNpcCleanupMsg( uint32_t nNpcId, uint8_t nCmd = 6 ); // see implementation about nCmd

// Following methods for unicast messages DO include UDP_ID increment and
// UDP_ID / SessionID setting when needed (at least for now)
    PMessage* BuildOutpostClanInfoMsg( PClient* nClient, uint32_t nClanID, uint8_t nFaction );
    PMessage* BuildTryAccessAnswerMsg(PClient* nClient, char *nArea, bool nAllowed);
    PMessage* BuildReceiveDBAnswerMsg( PClient* nClient, PMessage* nResultBuffer, std::string* nCommandName, uint16_t nNumRows, uint16_t nNumFields);
    PMessage* BuildYouGotEmailsMsg( PClient* nClient, uint8_t nMailCount );
 
    PMessage* BuildNPCStartDialogMsg( PClient* nClient, uint32_t nNPCWorldID, std::string *nDialogScript  );
    PMessage* BuildNPCDialogReplyMsg( PClient* nClient, uint16_t nNextNode, std::vector<int>*nResultBuffer);
    PMessage* BuildReqNPCScriptAnswerMsg( uint32_t nInfoId, std::string *nNPCScript );
    PMessage* BuildNPCShoppingListMsg( PClient* nClient, PMessage* nContentList, int nWorldID, uint8_t nItemQuality);
    PMessage* BuildNPCBeginAllBuyerTradeMsg( PClient* nClient, int nWorldID );
 
    PMessage* BuildNPCSingleInfoMsg( PClient* nClient, uint32_t nWorldID, uint16_t nTypeID, uint16_t nClothing, uint16_t nNameID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint16_t nHealth, uint16_t nTraderID, std::string *nAngleStr, std::string *nNpcName, std::string *nCustomName);
    PMessage* BuildNPCSingleAliveMsg( PClient* nClient, uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint8_t nAction );
    PMessage* BuildNPCSingleUpdateMsg( PClient* nClient, uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint16_t nTarget, uint8_t nAction );
 
    // NEW for testing. Combined update message
    PMessage* BuildNPCUpdateMsg(uint32_t nWorldID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint8_t nActionBM, uint16_t nHealth, uint8_t nWeaponState, uint8_t nUnknown, uint32_t nTargetID = 0);
 
    PMessage* BuildReqInfoAnswerMsg( PClient* nClient, uint16_t nReqType, uint32_t nInfoId, void* nResponse, uint16_t nResponseLength );

    PMessage* BuildPacket0Msg( PClient* nClient );
    PMessage* BuildPingMsg( PClient* nClient, uint32_t nClientTime );
    PMessage* BuildBaselineMsg( PClient* nClient );
    PMessage* BuildAliveRepMsg( PClient* nClient );

    PMessage* BuildZoning1Msg( PClient* nClient, uint16_t nEntity, uint8_t nUnknown = 0 );
    PMessage* BuildZoningTCPReadyMsg();
    PMessage* BuildSendZoneTCPMsg( uint32_t nLocation, std::string* nWorldName );
    PMessage* BuildZoning2Msg( PClient* nClient, uint32_t nClientTime );
    PMessage* BuildGenrepZoningMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity );

    PMessage* BuildGenrepAddToListMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity );

    PMessage* BuildAptLiftUseMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity, uint8_t nEntityType = 0 );
    PMessage* BuildAptLiftFailedMsg( PClient* nClient );
    PMessage* BuildChangeLocationMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity, uint8_t nEntityType = 0, uint32_t nRawItemID = 0 );
    PMessage* BuildEntityPositionMsg( PClient* nClient, uint16_t pX, uint16_t pY, uint16_t pZ );

    PMessage* BuildCharAptLocInfoMsg( PClient* nClient );

    PMessage* BuildLevelUpMessage( PClient* nClient, uint8_t nMainSkill, uint8_t nNewLevel, uint16_t nFreeSkillPoints);
    PMessage* BuildSubskillIncMsg( PClient* nClient, uint8_t nSubskill, uint16_t nSkillPoints );
    PMessage* BuildChatAddMsg( PClient* nClient, uint32_t nAddedCharID, uint8_t nMode ); // mode = 1 for Direct, 2 for Buddy

    PMessage* BuildText100Msg( PClient* nClient, uint8_t nTxtMsgId, uint32_t nRawObjectID );
    PMessage* BuildTextIniMsg( PClient* nClient, uint8_t nTxtGroupID, uint16_t nTxtID );

    PMessage* BuildCharInteractionMenuMsg( PClient* nClient, uint32_t nRawTargetID );

    PMessage* BuildFurnitureActivateMsg( PClient* nClient, uint32_t nRawObjectID, uint8_t nActionValue );
    PMessage* BuildCharUseFurnitureMsg( PClient* nClient, uint32_t nRawObjectID );
    PMessage* BuildCharUseVhcTerminalMsg( PClient* nClient, uint32_t nRawObjectID );
    PMessage* BuildCharUseGogoMsg( PClient* nClient );
    PMessage* BuildCharUseVentureWarpMsg( PClient* nClient, uint32_t nRawObjectID );
    PMessage* BuildVhcAccessRequestMsg (PClient* nClient, uint32_t nRequestId, uint32_t nRequesterCharId, uint32_t nRequesterLocalId, uint32_t nVhcRawObjectID );
    PMessage* BuildCharUseGenrepMsg( PClient* nClient, uint32_t nRawObjectID, uint32_t nLocation, uint16_t nEntity );
    PMessage* BuildCharUseLiftMsg( PClient* nClient, uint32_t nRawObjectID, uint16_t nAptPlace );
    PMessage* BuildCharUseVhcMsg( PClient* nClient, uint32_t nRawObjectID, uint16_t nVhcType, uint16_t nAvailableSeats );

    PMessage* BuildCharMoneyUpdateMsg( PClient* nClient, uint32_t nCredits );
    PMessage* BuildUndefineduseMsg( PClient* nClient, uint8_t nValue );
    PMessage* BuildCharUseQBSlotMsg2( PClient* nClient, uint16_t nV1 = 100, uint16_t nV2 = 100, uint16_t nV3 = 100, uint16_t nV4 = 100, uint16_t nV5 = 100, uint16_t nV6 = 100, uint16_t nV7 = 0 );
    PMessage* BuildCharUseQBSlotMsg3( PClient* nClient, uint8_t nSlot );
    PMessage* BuildCharUseQBSlotMsg4( PClient* nClient, uint16_t nWeaponId );
    PMessage* BuildContainerContentList( PContainer* nContainer, uint8_t nLocType );
    PMessage* BuildContainerContentEntry( PContainerEntry* nEntry, uint8_t nLocType );

    PMessage* BuildCharOpenContainerMsg( PClient* nClient, uint32_t nContainerID, PContainer* nContainer );
    PMessage* BuildItemMoveMsg( PClient* nClient, uint8_t nSource, uint8_t nSrcX, uint8_t nSrcY, uint8_t nDestination, uint8_t nDestX, uint8_t nDestY, uint8_t nItemCnt );
    PMessage* BuildBoxItemMoveMsg( PClient* nClient, PContainerEntry* nEntry, uint8_t nSrcX, uint8_t nSrcY, uint8_t nDestination, uint8_t nDestX, uint8_t nDestY, uint8_t nItemCnt );
    PMessage* BuildStartWeaponReloadMsg( PClient* nClient );

    PMessage* BuildStartHackGameMsg( PClient* nClient, uint32_t nWorldObjID, uint8_t nHackDifficult );

    PMessage* BuildSubwaySpawnMsg( PClient* nClient, bool IsSecondMessage );
    //PMessage* BuildSubwayFullUpdateMsg(PClient* nClient);
    PMessage* BuildVhcInfoMsg( PClient* nClient, PSpawnedVehicle* nVehicle );

    PMessage* BuildDBRequestStatusMsg( PClient* nClient, std::string* nCommandName, uint8_t nStatus, uint16_t nErrCode );
    PMessage* BuildDBAnswerMsg( PClient* nClient, std::string* nCommandName, std::string* nAnswerData, uint16_t nRows, uint16_t nCols );

    PMessage* BuildTraderItemListMsg( PClient* nClient, uint32_t nTraderNpcID );

    PMessage* BuildHeldItemUse3Msg(  PClient* nClient, uint16_t nUnknown1, uint16_t nUnknown2, uint16_t nUnknown3, uint16_t nUnknown4  );

    PMessage* BuildCharUseTimedDrugMsg( PClient* nClient, const PDefDrug* nDrugDef, uint16_t nItemId );
    PMessage* BuildCharUseInstantDrugMsg( PClient* nClient, const PDefDrug* nDrugDef );
    PMessage* BuildCharUseRecreationUnitMsg( PClient* nClient, uint32_t nObjectId );
};
