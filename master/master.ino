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
const int modeMax = 7;
int modeIdx = 0;
int modeSelected = 0;
int buttonEnterState = 0;
int buttonLeftState = 0;
int buttonRightState = 0;
const int buttonEnterPin = 3;
const int buttonLeftPin = A0;
const int buttonRightPin = A1;
const int selectedLed = A2;
const int nonSelectedLed = A3;
bool debouncedEnter = false;
bool debouncedLeft = false;
bool debouncedRight = false;

// Instantiates attack classes.
PGN_Print_Atk       print_a;
PGN_Aquire_Pos_Atk  aquire_a;
PGN_North_Atk       north_a;
PGN_East_Atk        east_a;
PGN_South_Atk       south_a;
PGN_West_Atk        west_a;
PGN_Zig_Zag_Atk     zig_zag_a;

// Stores them in an array.
PGN_Attack* attacks[7] = {&print_a, &aquire_a, &north_a, &east_a, &south_a, &west_a, &zig_zag_a};

MCP2515 mcp2515(10);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  
void setup()
{
    Serial.begin(115200);

    // Setup for the buttons.
    pinMode(buttonEnterPin, INPUT);
    pinMode(buttonLeftPin, INPUT);
    pinMode(buttonRightPin, INPUT);
    pinMode(selectedLed, OUTPUT);
    pinMode(nonSelectedLed, OUTPUT);

    // Setup for the LCD screen.
    lcd.begin(16, 2);

    // Setup for the I2C communication between the master and slave.
    Wire.begin();
    
    // Setup for the CAN Module.
    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();
}

void loop()
{
    buttonCheck(buttonEnterPin, &debouncedEnter, &buttonEnterState);
    buttonCheck(buttonLeftPin, &debouncedLeft, &buttonLeftState);
    buttonCheck(buttonRightPin, &debouncedRight, &buttonRightState);

    if (modeIdx == modeSelected)
    {
        digitalWrite(selectedLed, HIGH);
        digitalWrite(nonSelectedLed, LOW);
    }
    else
    {
        digitalWrite(selectedLed, LOW);
        digitalWrite(nonSelectedLed, HIGH);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.setCursor(0, 1);
    lcd.print(attacks[modeSelected]->displayName());

    // This receives the message from the master side of the N2K network.
    if (mcp2515.readMessage(&recvMsg) == MCP2515::ERROR_OK)
    {
        attacks[modeSelected]->pgnAttack(&recvMsg);
        /**
         * After were done altering the CAN frame, we let it through to the network
         * through the slave.
         */
        sendToSlave(recvMsg);
    }
}

void buttonCheck(int buttonPin, bool* debounced, int* buttonState)
{
    // This deals with switching attack modes on button press.
    *buttonState = digitalRead(buttonPin);

    if (buttonPin == buttonEnterPin && *buttonState == HIGH)
    {
        modeSelected = modeIdx;
        return;
    }
    
    if (*buttonState == HIGH && *debounced == false)
    {
        *debounced = true;

        if (buttonPin == buttonRightPin)
        {
            modeIdx++;
            if (modeIdx >= modeMax)
            {
               modeIdx = 0;
            }
        }
        else
        {
            modeIdx--;
            if (modeIdx < 0)
            {
               modeIdx = modeMax - 1;
            }
        }
    }

    if (*buttonState == LOW && *debounced == true)
    {
        *debounced = false;
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
