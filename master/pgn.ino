#include "pgn.h"

void print_pgn(struct can_frame recvMsg)
{
    unsigned long PGN = recvMsg.can_id;

    PGN &= ~(0xFC000000);
    PGN /= 256;

    Serial.print("ID: ");
    Serial.print(recvMsg.can_id, HEX);
    Serial.print("\n");

    Serial.print("PGN: ");
    Serial.print(PGN);
    Serial.print("\n");

    Serial.print("DATA COUNT: ");
    Serial.print(recvMsg.can_dlc);
    Serial.print("\n");

    Serial.print("DATA: ");
    for (int i = 0; i < recvMsg.can_dlc; i++)
    {
        Serial.print(recvMsg.data[i], HEX);
        Serial.print(" ");
    }
    Serial.println("\n");
    
}
