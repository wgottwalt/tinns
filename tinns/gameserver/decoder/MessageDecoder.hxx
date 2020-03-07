#pragma once

#include <cstdint>
#include <sstream>
#include <string>

struct PGameState;
class PClient;
class PMessage;
class PUdpMsgAnalyser;

#define DECODE_UNDEF 0
#define DECODE_ERROR 1 // one error occured on the current decoding step
#define DECODE_UNKNOWN 2 // the current decoded (sub)message is unkown
#define DECODE_MORE 4 // more decoding needed for the (sub)message
#define DECODE_FINISHED 8 // decoding if finished for the whole message
#define DECODE_ACTION_READY 16 // an action is ready to be done
#define DECODE_ACTION_DONE 32 // the action triggered hasn't to be triggered again
#define DECODE_ACTION_IGNORED 64 // the action wasn't performed for some (good) reason (to combine or not with DONE)
#define DECODE_ACTION_FAILED 128 // the action failed totally or partially (to combine or not with DONE)

struct PMsgDecodeData {
    PMessage *mMessage;
    PClient *mClient;
    uint8_t mState;
    uint8_t mUnknownType;
    bool mHandling0x13Sub;
    uint16_t Sub0x13Start;
    uint16_t Sub0x13StartNext;
    std::stringstream mName;
    std::string mErrorDetail;
    bool mTraceKnownMsg;
    bool mTraceUnknownMsg;
    bool mTraceDump;
    PGameState *mClientState; // Temporary until State is put back in Client object
};

// UDP Message decoder
// boolean methods return true if successful

class PUdpMsgDecoder {
private:
    PMsgDecodeData mDecodeData;
    PUdpMsgAnalyser *mCurrentAnalyser;
    std::string mPacketName;
    std::string mTmpName;

    void Init(PMessage *nMessage, PClient *nClient);

public:
    PUdpMsgDecoder();
    PUdpMsgDecoder(PMessage *nMessage, PClient *nClient);
    ~PUdpMsgDecoder();

    bool Analyse();
    bool Analyse(PMessage *nMessage, PClient *nClient); // Can be used on non initialized or already used object
    // Temporary form until State is put back in Client object
    void Init(PMessage *nMessage, PClient *nClient, PGameState *nClientState);
    uint8_t GetState();
    bool IsError();
    bool IsKnown();
    bool MoreSubMsg();
    bool IsActionReady();
    bool IsActionDone();
    bool IsTraceKnownMsg();
    bool IsTraceUnknownMsg();
    bool IsTraceDump();
    void DumpMsg();
    std::string const &GetName();
    std::string const &GetError();
    bool DoAction();
    void Reset();
};
