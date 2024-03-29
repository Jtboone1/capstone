#include "pgn.h"

float current_lat = 0.000000;
float current_long = 0.000000;

void PGN_Print_Atk::pgnAttack(struct can_frame* recvMsg)
{
    pgnPrint(recvMsg);
}

String PGN_Print_Atk::displayName()
{
    return "Printing PGN    ";
}

void PGN_Aquire_Pos_Atk::pgnAttack(struct can_frame* recvMsg)
{
    if (isPositionPgn(recvMsg))
    {   
        /*
         * We can alter the latitude and longitude relative to the REAL
         * latitude and longitude by getting their values from the can  
         * frame data into two floating points.
         */
        float coords[2];
        getLatLong(recvMsg->data, &(coords[0]));
        current_lat = coords[0];
        current_long = coords[1];
        pgnPrint(recvMsg);
    }
}

String PGN_Aquire_Pos_Atk::displayName()
{
    return "Getting Coords  ";
}

void PGN_North_Atk::pgnAttack(struct can_frame* recvMsg)
{
    pgnPosAlter(recvMsg, 'n');
}

String PGN_North_Atk::displayName()
{
    return "Moving North    ";
}

void PGN_East_Atk::pgnAttack(struct can_frame* recvMsg)
{
    pgnPosAlter(recvMsg, 'e');
}

String PGN_East_Atk::displayName()
{
    return "Moving East     ";
}

void PGN_South_Atk::pgnAttack(struct can_frame* recvMsg)
{
    pgnPosAlter(recvMsg, 's');
}

String PGN_South_Atk::displayName()
{
    return "Moving South    ";
}

void PGN_West_Atk::pgnAttack(struct can_frame* recvMsg)
{
    pgnPosAlter(recvMsg, 'w');
}

String PGN_West_Atk::displayName()
{
    return "Moving West     ";
}

void PGN_Zig_Zag_Atk::pgnAttack(struct can_frame* recvMsg)
{
    if (isPositionPgn(recvMsg))
    {   
        current_lat += 0.100000;
        current_long += 0.100000;

        if (current_lat >= 89)
        {
            current_lat = -89;
        }

        if (current_long >= 179)
        {
            current_lat = -179;
        }
        
        float coords[2] = {current_lat, current_long};
         
        getData(coords, recvMsg->data);
        pgnPrint(recvMsg);
    }
}

String PGN_Zig_Zag_Atk::displayName()
{
    return "Moving Zig Zag  ";
}

void PGN_Small_Offset_Atk::checkCount()
{
    if (this->current_count < this->max_count)
    {
        this->current_count++;
        return;
    }

    this->current_count = 0;

    if (this->point == 0 || this->point == 2)
    {
        this->point = 1;
    }
    else
    {
        if (this->forward)
        {
            this->forward = false;
            this->point = 2;
        }
        else
        {
            this->forward = true;
            this->point = 0;
        }
    }
}

void PGN_Small_Offset_Atk::pgnAttack(struct can_frame* recvMsg)
{
    this->checkCount();
    if (this->point == 0)
    {    
        float coords[2] = {this->lat1, this->long1};
        getData(coords, recvMsg->data);
    }
    else if (this->point == 1)
    {   
        float coords[2] = {current_lat, current_long};
        getData(coords, recvMsg->data);
    }
    else
    {        
        float coords[2] = {this->lat2, this->long2};
        getData(coords, recvMsg->data);
    }
    pgnPrint(recvMsg);
}

String PGN_Small_Offset_Atk::displayName()
{
    return "Small Offset Fr ";
}

void PGN_Capstone_Atk::checkCount()
{
    if (this->current_count < this->max_count)
    {
        this->current_count++;
        return;
    }

    this->current_count = 0;

    if (this->point < 42)
    {
        this->point++;
    }
}

void PGN_Capstone_Atk::pgnAttack(struct can_frame* recvMsg)
{
    this->checkCount();
    float coords[2] = {this->points[this->point][0], this->points[this->point][1]};
    getData(coords, recvMsg->data);   
}

String PGN_Capstone_Atk::displayName()
{
    return "Capstone        ";
}

void pgnPrint(struct can_frame* recvMsg)
{
    unsigned long PGN = recvMsg->can_id;

    PGN &= ~(0xFC000000);
    PGN /= 256;

    Serial.print("ID: ");
    Serial.print(recvMsg->can_id, HEX);
    Serial.print("\n");

    Serial.print("PGN: ");
    Serial.print(PGN);
    Serial.print(" | 0x");
    Serial.print(PGN, HEX);
    Serial.print("\n");

    Serial.print("DATA: ");
    for (int i = 0; i < recvMsg->can_dlc; i++)
    {
        Serial.print(recvMsg->data[i], HEX);
        Serial.print(" ");
    }
    Serial.println("\n");
}

void pgnPosAlter(struct can_frame* recvMsg, char dir)
{
    if (isPositionPgn(recvMsg))
    {           
        /* 
         *  We now have the latitude and longitude coordinates
         *  array. We can then alter it however we see fit, and then
         *  get the data array back using getData.
         *  
         *  For this function, we will either move N,E,S,W depending on 
         *  the argument 'dir'.
         */

        switch (dir)
        {
            case 'n':
                current_lat += .100000;
                break;
            case 's':
                current_lat -= .100000;
                break;
            case 'e':
                current_long += .100000;
                break;
            default:
                current_long -= .100000;
        }   
        
        float coords[] = {current_lat, current_long};
        getData(coords, recvMsg->data);
        pgnPrint(recvMsg);
    }
}

bool isPositionPgn(struct can_frame* recvMsg)
{
    unsigned long PGN = recvMsg->can_id;

    PGN &= ~(0xFC000000);
    PGN /= 256;

    return PGN == PGN_POSITION_RAPID;
}

void getData(float* lat_long, uint8_t* data)
{
    int32_t latitude = *(lat_long) * 10000000;
    int32_t longitude = *(lat_long + 1) * 10000000;

    char hex_lat[16];
    char hex_long[16];

    sprintf(hex_lat, "%08lX", latitude);
    sprintf(hex_long, "%08lX", longitude);
    
    // Padding with zeros makes indexing the data array easier later on.
    while (strlen(hex_lat) < 8)
    {
        char tmp[20];
        sprintf(tmp, "0%s", hex_lat);
        strcpy(hex_lat, tmp);
    }

    while (strlen(hex_long) < 8)
    {
        char tmp[20];
        sprintf(tmp, "0%s", hex_long);
        strcpy(hex_long, tmp);
    }

    int offset = 3;
    for (int i = 0; i < 4; i++)
    {
        char res[4];

        char first_lat = hex_lat[offset * 2];
        char second_lat = hex_lat[offset * 2 + 1];
        
        char first_long = hex_long[offset * 2];
        char second_long = hex_long[offset * 2 + 1];

        sprintf(res, "%c%c", first_lat, second_lat);
        data[i] = strtoul(res, nullptr, 16);

        sprintf(res, "%c%c", first_long, second_long);
        data[i + 4] = strtoul(res, nullptr, 16);

        offset--;
    }
}

void getLatLong(uint8_t data[], float* coord)
{
    char latitude[100];
    char longitude[100];

    // Convert the strings to hex representation.
    sprintf(latitude, "%X%X%X%X", data[3], data[2], data[1], data[0]);
    sprintf(longitude, "%X%X%X%X", data[7], data[6], data[5], data[4]);

    uint32_t latitude_n = strtoul(latitude, nullptr, 16);
    uint32_t longitude_n = strtoul(longitude, nullptr, 16);

    // Take 2's compliment.
    int32_t signed_lat = static_cast<int32_t>(latitude_n);
    int32_t signed_long = static_cast<int32_t>(longitude_n);
    
    float final_lat = signed_lat / 10000000.000000;
    float final_long = signed_long / 10000000.000000;

    *(coord) = final_lat;
    *(coord + 1) = final_long;
}
