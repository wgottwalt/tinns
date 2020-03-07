#pragma once

#include <chrono>
#include <cstdint>

#define ISC_VER 2

// Part of mConStatus bitmask
#define ISC_CONNECTED 1
#define ISC_ADMIN 2

class PISC {
    private:
        enum PISC_Status
        {
            ISC_NOTCONNECTED,
            ISC_HANDSHAKE01,  // "Hello" msg sent, awaiting answer
            ISC_HANDSHAKE02,  // PW sent, awaiting answer
            ISC_GSDATA01,     // Servername sent, awaiting answer
            ISC_GSDATA02,     // IP, Port & Patchlevel sent, awaiting blabla..
            ISC_GSDATA03,     // Players / Staff onlinecounter sent, ...
            ISC_GSDATA04,     // Minlevel to connect, minlevel to see Playercount, minlevel to see server sent,....
            ISC_CLIENTIDENT,  // Awaiting client identification answer
            ISC_ADMINLOGIN,   // Awaiting answer to Infoserver control login request
            ISC_ICTRL_AUTOACC,// Infoserver control: Awaiting response to AutoAccount command
            ISC_ICTRL_LOGLV,  // Infoserver control: Awaiting response to LoginLevel command
            ISC_ICTRL_RELOAD, // Infoserver control: Awaiting response to Reload command
            ISC_ICTRL_SHUTD,  // Infoserver control: Awaiting response to Shutdown command
            ISC_ICTRL_HIDE,   // Infoserver control: Awaiting response to HidePeople command
            ISC_IDLE          // Updateloop running
        } mLinkStatus;

        uint16_t mConStatus;     // Bitmask of Connection status

        int isc_method;

        bool use_isc;
        char isc_pwd;

        bool use_mysql;
        int m_server_id;
        time_t mysql_update_intervall;
        time_t mysql_delayed_update_intervall;
        time_t mysql_last_update_time;
        int mysql_last_client_count;
        time_t mysql_last_count_decrease_time;

        void Start_isc();
        void Update_isc();
        void Shutdown_isc();

        void Start_mysql();
        void Update_mysql();
        bool do_mysql_db_update(int players, bool shutdown = false);
        void Shutdown_mysql();

    public:
        PISC();
        ~PISC();

        void Start();
        void Update();
        void Shutdown();
};
