#include "pgn.h"

void pgn_processor(int PGN, can_frame recvMsg)
{
    // If we get the default PGN's we'll pass on the message.
    if (PGN == PGN_IS_ACKNOWLEDGEMENT || PGN == PGN_ISO_ADDRESS_CLAIM || PGN == PGN_ISO_REQUEST)
    {
        print_pgn(recvMsg);
    }
}

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