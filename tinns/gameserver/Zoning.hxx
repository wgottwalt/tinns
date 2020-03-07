#pragma once

#include <cstdint>

class PClient;

//void SendZone(PClient *Client, PGameState *State,int loc);
void SendZone(PClient *Client, uint32_t loc);
