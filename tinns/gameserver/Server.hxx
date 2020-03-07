#pragma once

#include <cstdint>
#include <vector>

class PClient;

class PServer{
private:
    int32_t mMaxClients;
    int32_t mGMSlots;
    int32_t mNumClients;
    std::vector<PClient*> mClients;

public:
    PServer();
    ~PServer();

    inline int32_t GetMaxClients() const { return mMaxClients; }
    inline int32_t GetGMSlots() const { return mGMSlots; }
    inline int32_t GetNumClients() const { return mNumClients; }
    int NewClient();
    PClient *GetClient(int Client) const;
    void Update();
    void Shutdown();
};
