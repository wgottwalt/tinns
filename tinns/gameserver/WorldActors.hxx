#pragma once

#include <cstdint>

class PClient;

/*
Current known WorldActors:

- Furniture
1       Door
2       Wooden Door
3       Tech Door
5       Lift
6       Shutter
9       Airlock
10      Chair
11      Wooden Chair
12      Bench
13      Table
14      Recreation Unit

- Interactions
21      Gen rep
22      Go Guardian
99      Outfitter
23/29   Venture Warp
90      City Com
98      Home term
104     Home term
160-195 Hack term
900     Carport Term

- Cabinets
150     Cabinet
101     Secure Cabinet
105     Safe
106     High Security Safe

- Hack Boxes
700     Light Weapon Box
703     Medium
720     Ammo Box
740     Armour Box
750     Equipment Box
780     Psi Box

- Containers
134     Old Bag
120     Trashcan
125     Old Bag
126     Mutant Cooking Pot
127     Rotten Box
128     Rusty Box
133     Steel Box
130     Bunch of Bones
136     Dead Technician (Loot)
137     Corpse (Loot)

- Others
131     Pile of Stones
8       Rock
510     Tree Trunk
790     Burning Barrel
800     Explosive Barrel

- Costs
15      Pay 20nc
16      Pay 50nc
17      Pay 100nc
18      Pay 150nc
19      Pay 200nc
20      Pay 500nc

*/

// Start from this offset (00 00 80 00)
#define DYNACTORIDSTART 8388608

class PWorldActors {
    private:
    // SQL Layout
        enum
        {
            wa_id,
            wa_actor_id,
            wa_actor_map,
            wa_actor_model,
            wa_actor_type,
            wa_posX,
            wa_posY,
            wa_posZ,
            wa_rotX,
            wa_rotY,
            wa_rotZ,
            wa_option1,
            wa_option2,
            wa_option3
        };

        void SpawnWA(uint32_t nWorld, uint16_t nActorID, uint16_t nFunctionID, uint32_t nWOID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ);
        void VanishWA(uint32_t nWorld, uint32_t nWAid);

        // Get next availeable WorldactorID. First, try to find ID in known .dat files, then
        // Get the currently highest ID from SQL and add +1
        // select distinct wi_worlditem_id from world_items order by wi_worlditem_id desc limit 1;
        uint32_t GetNextFreeWAID();

    public:
        PWorldActors();
        ~PWorldActors();

        // Sends initial zone setup to client (after zoning-in)
        void InitWorld(PClient* nClient);

        // Add new worldactor to database and spawn it. Returns created WorldID
        // Position is taken from nClient, same as the worldID and calls the mainfunction
        // The function values are optional (used for interactive objects, gogo or genrep)
        uint32_t AddWorldActor(PClient* nClient, uint16_t nActorID, uint16_t nFuncID, uint16_t nOpt1 = 0, uint16_t nOpt2 = 0, uint16_t nOpt3 = 0);

        // Add new worldactor to database and spawn it. Returns created WorldID
        // The function values are optional (used for interactive objects, gogo or genrep)
        uint32_t AddWorldActor(uint32_t nWorldID, uint16_t nActorID, uint16_t nFuncID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ, uint16_t nOpt1 = 0, uint16_t nOpt2 = 0, uint16_t nOpt3 = 0);

        // Remove worldactor in given world from SQL and game
        void DelWorldActor(PClient* nClient, uint32_t nWAid);

        // Get functionvalues for worldactor
        void GetWAoption(uint32_t nWAid, uint16_t nWorld, uint16_t &nValue1, uint16_t &nValue2, uint16_t &nValue3);

        // Get SQL ID from world and worldID
        int GetWASQLID(uint32_t nWAid, uint32_t nWorld);

        // Check if actorID is dynamic
        bool IsDynamicActor(uint32_t nWAid);

        // Not needed. Better re-spawn the actor
        //bool EditWorldActor(uint32_t nWorldID, int nOption1 = -1, int nOption2 = -1, int nOption3 = -1);

        int GetWorldActorFunctionID(uint32_t nWAid);

        void GetFrontPos(uint32_t nWAID, uint16_t* mX, uint16_t* mY, uint16_t* mZ); // For chairs
        int GetLinkedObjectID(uint32_t nWAID); // OptionValue 1 is used for linked object!

        // Check if given functionID does exist
        bool IsValidWAFunction(int nFunctionID);

        // Checks if this functionID required an linked object or not
        bool RequiresLinkedObject(int nFunctionID);

        // Checks if the given worldobjectID is valid for this type of functionID
        bool IsValidLinkedObject(PClient *nClient, uint16_t nOption1, int nFunctionID);

        // Checks for double actor-IDs and deletes them from DB
        void DoActorCheck();
};
