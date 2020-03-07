#pragma once

#include <cstdint>
#include <map>

class PClient;
struct PPatchState;

class PPatchServer {
private:
    int mNumClients;
    int mNumFileTransfers;
    typedef std::map<PClient *, PPatchState *> PatchStateMap;
    PatchStateMap ClientStates;

protected:
    bool ProcessClient(PClient *Client, PPatchState *State = nullptr);
//    uint32_t StartPatch(PClient *Client, PPatchState *State);
    uint32_t StartPatch(PPatchState *State);
    bool SendPatchData(PClient *Client, PPatchState *State) const;
//    u32 StartFile(PClient *Client, PPatchState *State);
    uint32_t StartFile(PPatchState *State);
    bool SendFileData(PClient *Client, PPatchState *State) const;
    void FinalizeClient(PClient *Client, PPatchState *State);
    void FinalizeClientDelayed(PClient *Client, PPatchState *State);

    bool HandleHandshake(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize);
    bool HandleVersionRequest(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize);
    bool HandleFileRequests(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize);

public:
    PPatchServer();
    ~PPatchServer();

    void Start();
    void Update();
    void ClientDisconnected(PClient *Client);
};
