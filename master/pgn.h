#ifndef PGN_H
#define PGN_H

#include <SPI.h>
#include <Wire.h>
#include <mcp2515.h>
#include <string.h>

#define PGN_IS_ACKNOWLEDGEMENT 59392
#define PGN_ISO_REQUEST        55904
#define PGN_ISO_ADDRESS_CLAIM  60928
#define PGN_REQUEST_GRP_FUNC   126208
#define PGN_TRANSMIT_GRP_FUNC  126464
#define PGN_SYSTEM_TIME        126992
#define PGN_HEARTBEAT          126993
#define PGN_PRODUCT_INFO       126996
#define PGN_CONFIG_INFO        126998
#define PGN_VESSEL_HEADING     127250
#define PGN_MAGNETIC_VARIATION 127258
#define PGN_POSITION_RAPID     129025
#define PGN_COG_SOG            129026
#define PGN_GNSS_POS           129029
#define PGN_GNSS_DOP           129539
#define PGN_GNSS_SATS_VIEW     129539

// Custom latitude and longitude for attacks.
extern float current_lat;
extern float current_long;

// Attack interface. Derived classes will implement the details of their attacks.
class PGN_Attack
{
    public:
        virtual void pgnAttack(struct can_frame* recvMsg) = 0;
        virtual String displayName() = 0;
};

class PGN_Print_Atk : public PGN_Attack
{
    public:
        // Simply prints PGNs.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_Aquire_Pos_Atk : public PGN_Attack
{
    public:
        // Stores the real GPS latitude and longitude into two variables (current_lat, current_long).
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_North_Atk : public PGN_Attack
{
    public:
        // Alters current_lat and current_long coordinates to move north.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_East_Atk : public PGN_Attack
{
    public:
        // Alters current_lat and current_long coordinates to move east.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_South_Atk : public PGN_Attack
{
    public:
        // Alters current_lat and current_long coordinates to move south.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_West_Atk : public PGN_Attack
{
    public:
        // Alters current_lat and current_long coordinates to move west.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_Zig_Zag_Atk : public PGN_Attack
{
    public:
        // Alters current_lat and current_long coordinates to move in zig zag.
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_Small_Offset_Atk : public PGN_Attack
{
    private:
    
        // We'll stay at one of three locations for however many "max_frames" we set here.
        // Once we reach the max_frames, we'll move onto the next point.
        int point = 0;
        int max_count = 8;
        int current_count = 0;
        bool forward = true;
        
        const float lat1 = 49.446365;
        const float lat2 = 48.076703;
        const float long1 = -2.578878;
        const float long2 = -4.385740;

        void checkCount();

    public:
        
        // Creates a small offset in the destination of the boat. 
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

class PGN_Capstone_Atk : public PGN_Attack
{
    private:
        int point = 0;
        int max_count = 8;
        int current_count = 0;

        float max_h = 20.000000;
        float half_h = 10.000000;
        float min_h = 0.000000;

        float start = -170.000000;
        float offset = 20.000000;
        
        float points[42][2] = {

            // 'C'
            {max_h, start},
            {max_h, start - offset},
            {min_h, start - offset},
            {min_h, start},

            // 'A'
            {min_h, start + offset},
            {max_h, start + offset},
            {max_h, start + offset * 2},
            {min_h, start + offset * 2},
            {min_h, start + offset},
            {min_h, start + offset * 2},

            // 'P'
            {min_h, start + offset * 3},
            {max_h, start + offset * 3},
            {max_h, start + offset * 4},
            {half_h, start + offset * 4},
            {half_h, start + offset * 3},
            {min_h, start + offset * 3},

            // 'S'
            {min_h, start + offset * 6},
            {half_h, start + offset * 6},
            {half_h, start + offset * 5},
            {max_h, start + offset * 5},
            {max_h, start + offset * 6},

            // 'T'
            {max_h, start + offset * 8},
            {max_h, start + offset * 7.5},
            {min_h, start + offset * 7.5},
            {max_h, start + offset * 7.5},

            // 'O'
            {max_h, start + offset * 10},
            {max_h, start + offset * 9},
            {min_h, start + offset * 9},
            {min_h, start + offset * 10},
            {max_h, start + offset * 10},

            // 'N'
            {max_h, start + offset * 11},
            {min_h, start + offset * 11},
            {max_h, start + offset * 11},
            {min_h, start + offset * 12},
            {max_h, start + offset * 12},

            // 'E'
            {max_h, start + offset * 14},
            {max_h, start + offset * 13},
            {half_h, start + offset * 13},
            {half_h, start + offset * 14},
            {half_h, start + offset * 13},
            {min_h, start + offset * 13},
            {min_h, start + offset * 13}
        };

        void checkCount();
        
        public:
        // Creates a small offset in the destination of the boat. 
        void pgnAttack(struct can_frame* recvMsg);
        String displayName();
};

/**
 *  Helper functions used by multiple attacks.
 */

// Prints PGN to the Arduino's Serial output.
void pgnPrint(struct can_frame* recvMsg);

// Alters current_lat and current_long coordinates in the direction specified by dir (n,e,s,w).
void pgnPosAlter(struct can_frame* recvMsg, char dir);

// Returns True if PGN in the can_frame ID is equal to PGN_POSITION_RAPID.
bool isPositionPgn(struct can_frame* recvMsg);

// Given a frame's data, get the latitude and longitude as floating points.
void getLatLong(uint8_t data[], float* coord);

// Convert a coordinate structure into a N2K data array.
void getData(float* lat_long, uint8_t* data);

#endif
