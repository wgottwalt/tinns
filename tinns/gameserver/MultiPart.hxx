#pragma once

#include <cstdint>
#include <map>
#include <vector>

#define MAX_SEQUENCE_LIFE   10 // In seconds

typedef struct // A single chunk of any multipart sequence
{
    uint16_t smChunkNr;
    //uint16_t smChunkTotal;
    PMessage* smChunk;
} s_MessageChunk;

typedef std::vector<s_MessageChunk> vecMsgChunk; // The vector of an specific multipart sequence, containing all chunks
typedef struct
{
    time_t smTimeStamp;      // To keep track of sequence's lifetimer
    uint16_t smChunkTotal;       // Total amount of chunks for this sequence
    vecMsgChunk smvChunk;       // The chunkvector
    PClient *smClient;      // Required to call terminal class
} s_SequenceEntry;

typedef std::map<uint8_t, s_SequenceEntry> PMultipartMap; // Map of all vectors, indexed by sequencenumber <uint8_t>


class PMultiPart
{
  private:
    PMultipartMap MsgMap;
    void AssembleChunk(uint16_t nSequence);
    void ProcessPacket(PClient *nClient, PMessage *tmpMsg);

// Required to process packets here
    static const uint8_t mMaxOptions = 7;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    uint8_t mOptionsCount;
    uint16_t mDBId;
// ---------------------------------

  public:
    PMultiPart();
    ~PMultiPart();

    void Update(); // Check for complete/timed out sequences
    // Adds an multipart frame
    void AddMultiPartChunk(PClient *nClient, PMessage *nChunk, uint16_t nChunkNumber, uint16_t nChunkTotal, uint8_t nSequence);
};
