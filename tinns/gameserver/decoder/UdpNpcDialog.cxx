#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpNPCDialogClose ****/

PUdpNPCDialogClose::PUdpNPCDialogClose( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
    nDecodeData->mName << "/0x19";
}

PUdpMsgAnalyser* PUdpNPCDialogClose::Analyse()
{
    mDecodeData->mName << "=NPC Dialog closed";

    mPlayerID = mDecodeData->mMessage->U16Data( mDecodeData->Sub0x13Start + 5 );
    mNPCID = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 8 );

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpNPCDialogClose::DoAction()
{
    if ( mDecodeData->mState & DECODE_ACTION_READY )
    {
        if (gDevDebug)
            Console->Print( "%s PUdpNPCDialogClose: Client %d closed dialog with NPC %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mPlayerID, mNPCID );

        // Set DialogNPC for player to 0
        mDecodeData->mClient->GetChar()->SetDialogNPC(0);
        if (gDevDebug) Console->Print("[DEBUG PUdpNPCDialogClose::DoAction] New DialogPartner for Client %d is now %u", mPlayerID, mDecodeData->mClient->GetChar()->GetDialogNPC());

        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        return true;
    }
    else
        return false;
}
/**** PUdpNPCDialogAction ****/

PUdpNPCDialogAction::PUdpNPCDialogAction( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
    nDecodeData->mName << "/0x1a";
}

PUdpMsgAnalyser* PUdpNPCDialogAction::Analyse()
{
    mDecodeData->mName << "=NPC Dialog action";

    mPlayerID = mDecodeData->mMessage->U16Data( mDecodeData->Sub0x13Start + 5 );
    mAnswerNr = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 8 );

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpNPCDialogAction::DoAction()
{
    if ( mDecodeData->mState & DECODE_ACTION_READY )
    {
        if (gDevDebug)
            Console->Print( "%s PUdpNPCDialogAction: Client %d selected dialog answer %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mPlayerID, mAnswerNr );

        PClient* nClient = mDecodeData->mClient;
        PChar* tChar = nClient->GetChar();
        bool tSuccess = false;

        // First check if client has an NPC to talk to set (just to be sure)
        if (tChar->GetDialogNPC() != 0)
        {
            // Player has an NPC to talk to. Next, get the NPC instance
            PNPC* targetNPC = 0;
            PNPCWorld* currentNPCWorld = NPCManager->GetWorld( tChar->GetLocation() );
            if ( currentNPCWorld )
            {
                //Console->Print(">>> Searching NPC");
                targetNPC = currentNPCWorld->GetNPC( tChar->GetDialogNPC() );
                if ( targetNPC )
                {
                    Console->Print( "%s Player talks to NPC %u, answer %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tChar->GetDialogNPC(), mAnswerNr );
                    // Continue conversation with given answer. Current node is stored in tChar* class, and NPC handles next steps
                    // also sends out the required packet
                    targetNPC->DoConversation(nClient, mAnswerNr);
                    tSuccess = true;
                }
            }
        }
        else
        {
            Console->Print( "%s Dialog request received, but player has no active NPC dialog set, dropping", Console->ColorText( RED, BLACK, "[ERROR]" ));
            tSuccess = true;
        }
        if(tSuccess == false)
        {
            Console->Print( "%s Player talks to NPC %u, but npc isnt active anymore. Resetting dialog state for client", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), tChar->GetDialogNPC() );
            tChar->SetDialogNode(0);
            tChar->SetDialogNPC(0);
        }

        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        return true;
    }
    else
        return false;
}
