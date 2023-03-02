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
const int modeMax = 5;
int buttonState = 0;
int modeIdx = 0;
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
    Wire.onReceive(send_to_network);
    
    // Setup for the CAN Module.
    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();
}

void loop()
{
    Wire.requestFrom(6, 100);
    char rc = ' ';
    while (Wire.available() > 0 && rc != ']')
    {
        char rc = Wire.read();
        
        if (rc == '<')
        {
            char can_id[50];
            int idx = 0;
            while (Wire.available() > 0 rc != '>')
            {
                rc = Wire.read();

                if (rc != '>')
                {
                    can_id[idx] = rc;
                }
                idx++;
            }

            can_id[idx] = '\0';
        }
    }
    
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

    if (mcp2515.readMessage(&recvMsg) == MCP2515::ERROR_OK)
    {
        // TODO: Change this to select function based off modeIdx.
        print_pgn(recvMsg);

        /**
         * After were done altering the CAN frame, we let it through to the network
         * through the slave.
         */
        send_to_slave(recvMsg);
    }

    lcd.setCursor(0, 1);
    lcd.print(modeIdx);
}

void send_to_slave(struct can_frame msg)
{
    char slave_frame[500];
    sprintf(slave_frame, "<%lu>|%u|", msg.can_id, msg.can_dlc);

    for (int i = 0; i < msg.can_dlc; i++)
    {
        char slave_data[10];
        sprintf(slave_data, ";%u;", msg.data[i]);
        strcat(slave_frame, slave_data);
    }

    // The ']' character will symbolize the end of the frame.
    strcat(slave_frame, ']');

    Serial.print("Slave Frame: ");
    Serial.print(slave_frame);
    Serial.print("\n");

    // Send frame to slave arduino.
    Wire.beginTransmission(6);
    Wire.write(slave_frame);
    Wire.endTransmission();
}

void send_to_network(int bytes)
{
    // TODO: Create Can frame from I2C string.
    
    mcp2515.send(&sendMsg);
}
    
  
