/*
 * Functions for dealing with the various PGNs.
*/
#include <SPI.h>
#include <mcp2515.h>

#define PGN_IS_ACKNOWLEDGEMENT 059392
#define PGN_ISO_REQUEST        055904
#define PGN_ISO_ADDRESS_CLAIM  060928
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

void print_pgn(struct can_frame recvMsg);
