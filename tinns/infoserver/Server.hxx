#pragma once

#include <cstdint>
#include <vector>

class PClient;

class PServer {
private:
    int32_t mMaxClients;
    int32_t mNumClients;
    std::vector<PClient *> mClients;

public:
    PServer();
    ~PServer();

    int32_t GetNumClients() const;
    int32_t NewClient();
    PClient *GetClient(int32_t Client) const;
    void Update();
    void Shutdown();
};
