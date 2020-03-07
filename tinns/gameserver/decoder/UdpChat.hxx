#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpChatLocal : public PUdpMsgAnalyser {
public:
    PUdpChatLocal(PMsgDecodeData *nDecodeData);
    //~PUdpChatLocal();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpChatGlobal : public PUdpMsgAnalyser {
public:
    PUdpChatGlobal(PMsgDecodeData *nDecodeData);
    //~PUdpChatGlobal();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpChatListAdd : public PUdpMsgAnalyser {
private:
    uint8_t mChatList; // 1 = Direct, 2 = Buddy List
    char *mAddedCharname;

public:
    PUdpChatListAdd(PMsgDecodeData *nDecodeData);
    //~PUdpChatListAdd();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpChatListRemove : public PUdpMsgAnalyser {
private:
    uint8_t mChatList;
    uint32_t mRemovedCharID;

public:
    PUdpChatListRemove(PMsgDecodeData *nDecodeData);
    //~PUdpChatListRemove();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpChatChannels : public PUdpMsgAnalyser {
private:
    uint32_t mChannelFlags; // 1 bit per custom channel, starting from LSB, in same order as in chat i/f

public:
    PUdpChatChannels(PMsgDecodeData *nDecodeData);
    //~PUdpChatChannels();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
