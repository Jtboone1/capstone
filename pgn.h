/*
 * Functions for dealing with the various PGNs.
*/
#include <SPI.h>
#include <mcp2515.h>

void print_pgn(struct can_frame recvMsg);
