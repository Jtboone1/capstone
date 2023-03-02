#include "pgn.h"

void pgn_processor(struct can_frame recvMsg)
{
    // TODO Extract PGN and deal with it here.
    
    // If we get the default PGN's we'll pass on the message.
    // if (PGN == PGN_IS_ACKNOWLEDGEMENT || PGN == PGN_ISO_ADDRESS_CLAIM || PGN == PGN_ISO_REQUEST)
    // {
    //     print_pgn(recvMsg);
    // }
}

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
