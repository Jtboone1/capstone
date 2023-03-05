#include "pgn.h"

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
    Serial.print(" ");
    Serial.print(PGN, HEX);
    Serial.print("\n");

    Serial.print("DATA COUNT: ");
    Serial.print(recvMsg->can_dlc);
    Serial.print("\n");

    Serial.print("DATA: ");
    for (int i = 0; i < recvMsg->can_dlc; i++)
    {
        Serial.print(recvMsg->data[i], HEX);
        Serial.print(" ");
    }
    Serial.println("\n");
    
}

void pgnPosAlter(struct can_frame* recvMsg)
{
    unsigned long PGN = recvMsg->can_id;

    PGN &= ~(0xFC000000);
    PGN /= 256;

    if (PGN == PGN_POSITION_RAPID || PGN == PGN_GNSS_POS)
    {   
        float coords[2];
        getLatLong(recvMsg->data, &(coords[0]));

        /* 
         *  TODO: We now have the latitude and longitude coordinates
         *  array. We can then alter it however we see fit, and then
         *  get the data array back using getData.
         */
         
        getData(coords, recvMsg->data);
        
        pgnPrint(recvMsg);
    }
}

void getData(float* lat_long, uint8_t* data)
{
    int32_t latitude = *(lat_long) * 1000000;
    int32_t longitude = *(lat_long + 1) * 1000000;

    // Inverse 2's compliment.
    uint32_t unsigned_lat = static_cast<int32_t>(latitude);
    uint32_t unsigned_long = static_cast<int32_t>(longitude);

    char hex_lat[16];
    char hex_long[16];

    sprintf(hex_lat, "%X", unsigned_lat);
    sprintf(hex_long, "%X", unsigned_long);

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

    uint32_t latitude_n = strtol(latitude, nullptr, 16);
    uint32_t longitude_n = strtol(longitude, nullptr, 16);

    // Take 2's compliment.
    int32_t signed_lat = static_cast<int32_t>(latitude_n);
    int32_t signed_long = static_cast<int32_t>(longitude_n);

    /*
     * The standard for latitude and longitude PGN's is that they take 6 points
     * past the '.', So say we read 49123456, we need the float to become
     * 49.123456. So we divide by 10^6 and add 6 trailing zeros
     * so we don't lose data.
     */
    float final_lat = signed_lat / 1000000.000000;
    float final_long = signed_long / 1000000.000000;

    *(coord) = final_lat;
    *(coord + 1) = final_long;
}
