#include <SPI.h>
#include <mcp2515.h>

#include "pgn.h"

struct can_frame recvMsg;
struct can_frame sendMsg;
MCP2515 mcp2515(10);

void setup()
{
    Serial.begin(115200);

    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();

    Serial.println("------ CAN Read ------");

}

void loop()
{
    if (mcp2515.readMessage(&recvMsg) == MCP2515::ERROR_OK)
    {
        // PGN handling code goes here.
        print_pgn(recvMsg);
    }

    delay(100);
}
