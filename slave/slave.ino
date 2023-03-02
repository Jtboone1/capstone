#include <SPI.h>
#include <Wire.h>
#include <mcp2515.h>

struct can_frame recvMsg;
struct can_frame sendMsg;

MCP2515 mcp2515(10);

int data_idx = 0;
String frame_str = "";

void setup()
{
    Serial.begin(115200);
    
    Wire.begin(6);
    Wire.onReceive(receive_event);

    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();
}

void loop()
{
}

void create_frame()
{
    int i = 0;
    char master_id[100] = "";
    char data_str[50] = "";

    // This part creates the frame ID.
    while (frame_str[i] != ' ')
    {
        char id_char[1];
        sprintf(id_char, "%c", frame_str[i]);
        strcat(master_id, id_char); 
        i++;
    }
    i++;
        
    sendMsg.can_id = string_to_ul(master_id);
    Serial.print("ID: ");
    Serial.print(sendMsg.can_id);
    Serial.print("\n");
    Serial.print("DATA: ");

    // This part gets the frame data.
    while (data_idx != 8)
    {

        while (frame_str[i] != ' ')
        {
            char data_char[1];
            sprintf(data_char, "%c", frame_str[i]);
            strcat(data_str, data_char); 
            i++;
        }
        i++;

        sendMsg.data[data_idx] = atoi(data_str);

        Serial.print(sendMsg.data[data_idx]);
        Serial.print(" ");
        
        data_str[0] = '\0';        
        data_idx++;
    }
    Serial.print('\n');

    // The CAN data count is always 8.
    sendMsg.can_dlc = 8;
    data_idx = 0;

    // Send the CAN frame back to the network.
    mcp2515.send(%sendMsg);
}

void receive_event(int bytes)
{
    char rc;

    char master_id[100] = "";
    char master_data[100] = "";
    
    while (Wire.available() > 0)
    {
        rc = Wire.read();
        frame_str += rc;

        if (rc == ']')
        {
            Serial.print("FRAME: ");
            Serial.print(frame_str);
            Serial.print("\n");
            create_frame();
            frame_str = "";
        }
    }
}

/**
 * Helper function to convert our ID strings to unsigned longs. 
 * strtoul was maxing out on our IDs so this is basically our own strtoul() function.
 */
unsigned long string_to_ul(char str[])
{
    unsigned long x = 0;
    unsigned long place = 1;
    for (int i = strlen(str) - 1; i >= 0; i--)
    {
        unsigned long y = (str[i] - '0') * place;             
        x += y;  
        place *= 10;
    }

    return x;
}
