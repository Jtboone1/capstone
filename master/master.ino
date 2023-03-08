#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <mcp2515.h>

#include "pgn.h"

struct can_frame recvMsg;
struct can_frame sendMsg;

// LCD Pin connections
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;

// Button Setup
const int buttonPin = 3;
const int modeMax = 7;
int modeIdx = 0;
int buttonState = 0;
bool debounced = false;

MCP2515 mcp2515(10);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  
void setup()
{
    Serial.begin(115200);

    // Setup for the button.
    pinMode(buttonPin, INPUT);

    // Setup for the LCD screen.
    lcd.begin(16, 2);
    lcd.print("Mode: ");

    // Setup for the I2C communication between the master and slave.
    Wire.begin();
    
    // Setup for the CAN Module.
    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();
}

void loop()
{
    // This deals with switching attack modes on button press.
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH && debounced == false)
    {
        debounced = true;
        modeIdx++;

        if (modeIdx > modeMax)
        {
            modeIdx = 0;
        }
    }

    if (buttonState == LOW && debounced == true)
    {
        debounced = false;
    }
    
    lcd.setCursor(0, 1);

    // This receives the message from the master side of the N2K network.
    if (mcp2515.readMessage(&recvMsg) == MCP2515::ERROR_OK)
    {
        switch(modeIdx)
        {
            case 1:
                pgnGetPos(&recvMsg);
                lcd.print("Getting Position");
                break;

            case 2:
                pgnPosAlterNorth(&recvMsg);
                lcd.print("Moving North");
                break;

            case 3:
                pgnPosAlterSouth(&recvMsg);
                lcd.print("Moving South");
                break;

            case 4:
                pgnPosAlterEast(&recvMsg);
                lcd.print("Moving East");
                break;

            case 5:
                pgnPosAlterWest(&recvMsg);
                lcd.print("Moving West");
                break;

            case 6:
                pgnPosZigzag(&recvMsg);
                lcd.print("Zig Zagging");
                break;

            default:
                pgnPrint(&recvMsg);
                lcd.print("Printing Frames");
        }

        /**
         * After were done altering the CAN frame, we let it through to the network
         * through the slave.
         */
        sendToSlave(recvMsg);
    }
}

void sendToSlave(struct can_frame msg)
{
    char slave_frame_id[100];
    sprintf(slave_frame_id, "%lu ", msg.can_id);

    char slave_frame_data[100] = "";
    for (int i = 0; i < msg.can_dlc; i++)
    {
        char data[20];
        sprintf(data, "%u ", msg.data[i]);
        strcat(slave_frame_data, data);
    }

    // Send frame to slave arduino.
    Wire.beginTransmission(6);
    Wire.write(slave_frame_id);
    Wire.endTransmission();

    Wire.beginTransmission(6);
    Wire.write(slave_frame_data);
    Wire.endTransmission();

    Wire.beginTransmission(6);
    Wire.write("]");
    Wire.endTransmission();
}
  
