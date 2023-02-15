#include "pgn.h"

void print_pgn(struct can_frame recvMsg)
{
    Serial.println("ID  DLC  DATA");
    Serial.print(recvMsg.can_id, HEX); // print ID
    Serial.print(" ");
    Serial.print(recvMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");

    for (int i = 0; i < recvMsg.can_dlc; i++)
    {
        Serial.print(recvMsg.data[i], HEX);
        Serial.print(" ");
    }

    Serial.println("\n");
}