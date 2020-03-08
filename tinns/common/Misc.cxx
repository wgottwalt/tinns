#include <cstdarg>
#include "common/Includes.hxx"

namespace Misc
{
    struct IPv4 {
        union {
            uint32_t val;
            uint8_t raw[sizeof (uint32_t)];
            struct {
                uint8_t a, b, c, d;
            };
        };
    };

    uint32_t ip4StringToUint32(const std::string &ip)
    {
        if (!ip.empty())
        {
            std::string tmp = "";
            uint32_t result = 0;

            for (auto chr : ip)
            {
                switch (chr)
                {
                    case '.':
                        result <<= 8;
                        result |= std::stoul(tmp);
                        tmp.clear();
                        continue;
                    default:
                        tmp += chr;
                }
            }
            result <<= 8;
            result |= std::stoul(tmp);

            return result;
        }

        return 0;
    }

    const std::string uint32ToIp4String(const uint32_t ip)
    {
        const IPv4 nip = {.val = ip};
        std::string result = "0.0.0.0";

        if (Endian::Order::Native == Endian::Order::Little)
            result = std::to_string(nip.d) + "." + std::to_string(nip.c) + "." +
                     std::to_string(nip.b) + "." + std::to_string(nip.a);
        else if (Endian::Order::Native == Endian::Order::Big)
            result = std::to_string(nip.a) + "." + std::to_string(nip.b) + "." +
                     std::to_string(nip.c) + "." + std::to_string(nip.d);

        return result;
    }

    //NEW
    //this function allow to print a packet
    //just for tracking values
    void PrintPacket( uint8_t *Packet, int PacketSize )
    {
      Console->Print( "inside : PrintPacket" );

      if ( PacketSize < 1 ){Console->Print( "Wrong packet" );}//do nothing
      else
      {
        Console->Print( "PacketSize is : %d", PacketSize );

        uint8_t value = 0;
        for ( int i = 0;i < PacketSize;i++ )
        {
          value = *( uint8_t* ) & Packet[i];
          Console->Print( "value[%d] is : %x", i, value );
        }
      }
    }

    void cleanUpString(std::string &str)
    {
        if (str.length() > 3)
        {
            auto pos = str.find('\\');

            while (pos != std::string::npos)
            {
                str.replace(pos, 1, " ");
                pos = str.find('\\', pos + 1);
            }

            if (str == "   ")
                str.clear();
            else
                str = trim(str);
        }
        else
            str.clear();
    }

    std::string trim(const std::string &data, const char delim, const bool at_start,
                     const bool at_end)
    {
        std::string tmp(data);

        if (at_start)
            while (!tmp.empty() && (*(tmp.begin())) == delim)
                tmp.erase(tmp.begin(), tmp.begin() + 1);
        if (at_end)
            while (!tmp.empty() && (*(tmp.end() - 1)) == delim)
                tmp.erase(tmp.end() - 1, tmp.end());

        return tmp;
    }

    std::string trim(const std::string &data, const bool at_start, const bool at_end)
    {
        std::string tmp(data);

        if (at_start)
            while (!tmp.empty() && std::isspace(*(tmp.begin())))
                tmp.erase(tmp.begin(), tmp.begin() + 1);
        if (at_end)
            while (!tmp.empty() && std::isspace(*(tmp.end() - 1)))
                tmp.erase(tmp.end() - 1, tmp.end());

        return tmp;
    }

    std::string GetAccessString( int level )
    {
      switch ( level )
      {
        case 0: return "Unregistered";
        case 1: return "Registered";
        case 30: return "Volunteer";
        case 50: return "Gamemaster";
        case 100: return "Administrator";
      }
      return "Error";
    }

    std::string &Ssprintf( const char *fmt, ... )
    {
      static std::string tmpstring;
      char buff[1024];
      va_list args;

      va_start( args, fmt );
      vsnprintf( buff, 1024, fmt, args );
      va_end( args );
      buff[1023] = '\0';
      tmpstring = buff;
      return tmpstring;
    }

    uint16_t DistanceApprox( const uint16_t x1, const uint16_t y1, const uint16_t z1, const uint16_t x2, const uint16_t y2, const uint16_t z2 )
    {
      uint16_t DX, DY, DZ, DMax;
      uint32_t DMinSum, DApprox;

      DMax = DX = ( x1 >= x2 ) ? x1 - x2 : x2 - x1;
      DMinSum = DY = ( y1 >= y2 ) ? y1 - y2 : y2 - y1;
      DZ = ( z1 >= z2 ) ? z1 - z2 : z2 - z1;
      if ( DX < DY )
      {
        DMax = DY;
        DMinSum = DX;
      }
      if ( DMax < DZ )
      {
        DMinSum += DMax;
        DMax = DZ;
      }
      else
      {
        DMinSum += DZ;
      }

      DApprox = DMax + ( uint32_t )( 0.33 * DMinSum );
      if ( DApprox > 65535 )
      {
        DApprox = 65535;
      }

      // We use Dapprox = max(dx, dy) + K * min(dx, dy)
      // Dapprox = (DX > DY) ? DX + (DY >> 1) : DY + (DX >> 1); // Fastest, but max error on dist is around 10% real dist
      //Dapprox = (u16)((DX > DY) ? DX + 0.33*DY : DY + 0.33*DX); // max error on dist is around 5%, which should be enough
      /*
       // for dist approx debug
      float fDX, fDY, fDZ, fDist;
      fDX=(x1 - x2);
      fDY=(y1 - y2);
      fDZ=(z1 - z2);
      fDist=sqrt(fDX*fDX + fDY*fDY + fDZ*fDZ);
      if (fDist != 0) Console->Print("Dist: %f\tApprox: %d\tError: %d (%d%)", fDist, DApprox, (int)(DApprox-fDist), (int)(100*(DApprox-fDist)/fDist));
      */

      return ( uint16_t )DApprox;
    }

    /*** Portable pseudo-random number generator ***/
    // until native standardized C++ lib support

    uint32_t mInternalRand = 1;

    void InitRandom( uint32_t nInitialisationValue )
    {
      mInternalRand = nInitialisationValue;
    }

    uint16_t GetRandom( uint16_t MaxVal, uint16_t MinVal )
    {
      mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
      return ( uint16_t )( MinVal + (( mInternalRand >> 16 ) % 32768 % ( MaxVal - MinVal + 1 ) ) );
    }

    float GetRandomFloat()
    {
      mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
      return (( float )(( mInternalRand >> 16 ) % 32768 ) / ( float )32768 );
    }
}
