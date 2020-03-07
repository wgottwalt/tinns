#pragma once

#include <cstdint>
#include <map>

typedef std::map<uint32_t, PClient*> PClientMap;

class PClientManager {
private:
    //int       mLastID;
    PClientMap mClientList;

public:
    PClientManager();
    ~PClientManager();

    PClientMap::iterator   getClientListBegin() { return mClientList.begin(); }
    PClientMap::iterator   getClientListEnd() { return mClientList.end(); }

    bool addClientToList( PClient* newClient );
    //void deleteClientFromListByID(int id);
    void deleteClientFromList( uint32_t id );
    //bool deleteClientFromList(PClient* delClient); // maybe no use for this...
    PClient* getClientByID( uint32_t uint32_t ) const; // returns pointer to a client for further use
    PClient* getClientByChar( uint32_t CharID ) const;
    PClient* getClientByChar( const std::string &Name ) const;
    // int getClientID(PClient* _client); do _client->GetLocalID()
    bool IsWorldInUse( uint32_t nWorldID ) const; // Temp until world content fully managed by world
    PClient* GetClientByCharLocalId( uint32_t rawObjectId, uint32_t nWorldID  ) const; // Temp (called by world) until world content fuly managed by world

    // each function return the number of messages sent.
    int UDPBroadcast( PMessage* nMessage, uint32_t nZoneID, uint16_t nX = 0, uint16_t nY = 0, uint16_t nZ = 0, uint16_t nMaxDist = 0, uint32_t nSkipCharId = 0, bool nNPCPing = false );
    int UDPBroadcast( PMessage* nMessage, PClient* nClient, uint16_t nMaxDist = 0, bool nSkipSource = false, bool nNPCPing = false );
    int SendUDPZoneWelcomeToClient( PClient* nClient );
};
