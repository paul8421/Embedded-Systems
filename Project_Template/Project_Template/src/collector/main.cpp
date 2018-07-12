#include <Arduino.h>
#include <Zumo32U4.h>
#include <nrf24.h>
#include <nRF24L01.h>
#include "../../libs/arduino/HardwareSerial.h"
#include "math.h"

/******************************RF***********************************/
int flag = 1;
char buff[2] = {'O', 'K'};
char buff1[3] = {'N', 'O', 'K'};
char buff3[1] = {10};
char reg_data[8] = {'0','0','0','0','0','0','0','0'};

uint8_t tx_address[5] = {0xe1,0xf0,0xf0,0xf0,0xf0};
uint8_t rx_address[5] = {0x12,0x12,0x66,0x4b,0x6f};
//0x6f4b661212
uint8_t data_array[20];
uint8_t temp;
uint8_t q = 0;
/******************************RF***********************************/
#define MODE 2
#define  INIT_PING 2
int m = 0;
void dance();
uint16_t _2hex2dec(uint8_t reg1, uint8_t reg2);
float drive_homing(float x1, float y1, float theta, float x2, float y2);
Zumo32U4Motors motors;
typedef struct coordinates{
    int x;
    int y;

}coordiantes;
void drive();
float distances [] = {0,0,0};
float find_angle(float x1, float y1, float x2, float y2);
float find_dist(float x1, float y1, float x2, float y2);
void turn_around(float angle);
float drive(float x1, float y1, float x2, float y2);
void insertionSort(float distances[], int n);
#define  mot_spd 210

void  send_serial_RF(uint8_t* data_array);

void setup() {
    int m =0;
    Serial1.begin(9600);
    nrf24_init();
    nrf24_config(111, 10); // on channel 111 with payload length 4
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    Serial1.println("COLLECTOR");
    DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input

    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled



    EICRA |= (1 << ISC10);    // set INT0 to trigger on falling edge
    EIMSK |= (1 << INT0);     // Turns on INT0

    sei();                    // turn on interrupts
}



void loop() {
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    uint8_t send_reg[32];
    send_reg[2] = 0xde;
    //send_reg[3] = 0x02;
    send_reg[1] = 0xa0;
    send_reg[0] = 0x50;

    float angle3;
    uint8_t status = nrf24_getStatus();
    delay(20);
    if (nrf24_dataReady()) {
        Serial1.println("Get data");
        nrf24_getData(data_array);
        delay(200);
        switch (data_array[0]) {
            case 0x51:
                Serial1.println("ACK for PING\n");
                Serial1.print(data_array[0], HEX);
                Serial1.print(data_array[1], HEX);
                Serial1.println(data_array[2], HEX);
                flag = 1;
                break;
            case 0x50:
                Serial1.print(data_array[0], HEX);
                Serial1.print(data_array[1], HEX);
                Serial1.println(data_array[2], HEX);

                if (data_array[2] == 0xff)
                    data_array[1]++;
                data_array[0]++;
                data_array[2]++;
                data_array[0]= 0x15;
                data_array[1]= 0x15;
                data_array[2]= 0x15;
                send_serial_RF(data_array);
                Serial1.print(data_array[0], HEX);
                Serial1.print(data_array[1], HEX);
                Serial1.println(data_array[2], HEX);
                flag = 1;
                break;

            case 0x60:
                if(flag)
                {Serial1.println("In 0x60");
                    motors.setSpeeds(45, 45);
                    delay(500);
                    flag = 0;
                    nrf24_csn_digitalWrite(LOW);
                    spi_transfer(FLUSH_RX);
                    nrf24_csn_digitalWrite(HIGH);}
                else{}
                break;
            case 0x61:

                Serial1.println("OOB");
                motors.setSpeeds(0,0);
                for(int i = 0; i<15; i++)
                {
                    delay(2000);
                }

                uint16_t angle_decode = _2hex2dec(data_array[1], data_array[2]);

                float temppp = (float) angle_decode;

                temppp = temppp*360;
                temppp = temppp/65535;

                delay(500);
                int temp11 = _2hex2dec(data_array[3], data_array[4]);

                int temp2 = _2hex2dec(data_array[5], data_array[6]);//rcv_reg[5]*256 + rcv_reg[6];
                angle3 = drive_homing((float) temp11/100, (float) temp2/100, temppp, 0, 0);
                dance();


                while(1);
                break;              ;
        }
    }
}


void  send_serial_RF(uint8_t * data_array)
{
    nrf24_send(data_array);

    while (nrf24_isSending());
    temp = nrf24_lastMessageStatus();

    if (temp == NRF24_TRANSMISSON_OK)
    {
        Serial1.println("TX OK");

    }
    else if (temp == NRF24_MESSAGE_LOST) {
        {
            Serial1.println("TX NNNNOK");
        }
    }

    /* Retranmission count indicates the tranmission quality */
    temp = nrf24_retransmissionCount();
    /* Optionally, go back to RX mode ... */
    nrf24_powerUpRx();
    delay(200);
}

float find_angle(float x1, float y1, float x2, float y2)
{
    float theta,x_diff,y_diff, tan_theta;
    x_diff = x2 - x1;
    y_diff = y2 - y1;
    tan_theta = (y2-y1)/(x2-x1);
    theta = atan(tan_theta);
    if (x_diff >= 0 && y_diff < 0 )  // 4th quad
    {
        theta = 2*(3.14285714) + theta;
    }
    else if ((x_diff < 0 && y_diff >=0) || (x_diff < 0 && y_diff <0)  ) // 2nd quad and 3rd quad
    {
        theta = theta + (3.14285714);
    }

    theta = theta*(180/3.14285714);
    return theta;
}

float find_dist(float x1, float y1, float x2, float y2)
{
    float s_dist_x, s_dist_y, s_sdist, dist;
    s_dist_x = (x2-x1)*(x2-x1);
    s_dist_y = (y2-y1)*(y2-y1);
    s_sdist = s_dist_x+s_dist_y;
    dist = sqrt(s_sdist);
    return dist;
}

void turn_around(float angle)
{
    float angle1 = 360-angle;
    delay(1000);
    motors.setSpeeds(-mot_spd,mot_spd);
    delay(angle1*4.1);
    motors.setSpeeds(0,0);
}

float drive(float x1, float y1, float x2, float y2) {
    float angle = find_angle(x1, y1, x2, y2);
    float dist = find_dist(x1, y1, x2, y2);
    int dist1 = dist * 400;
    int time_motors = angle * 4.1;

    delay(1000);
    motors.setSpeeds(-mot_spd, mot_spd);
    delay(time_motors);
    motors.setSpeeds(0, 0);


    motors.setSpeeds(mot_spd, mot_spd);
    delay(dist1);
    motors.setSpeeds(0, 0);

    return angle;

}


float drive_homing(float x1, float y1, float theta, float x2, float y2) {
    Serial1.println(("In drive\n"));
    Serial1.println(x1);
    Serial1.println(y1  );
    Serial1.println(theta  );



    if(theta >=0 && theta <= 90)
    {
        theta = 90 - theta;
    }
    else
    {
        theta = abs(450- theta);
    }
    float angle = find_angle(x1, y1, x2, y2) - theta;
    Serial1.print("find angle yields  ");
    Serial1.println(angle+theta);
    if(angle < 0)
    {
        angle = angle +360;
    }

    Serial1.print("Final angle is  :");
    Serial1.println(angle);
    float dist = find_dist(x1, y1, x2, y2);
    int dist1 = dist * 400;
    int time_motors = angle * 4.1;

    delay(1000);
    motors.setSpeeds(-mot_spd, mot_spd);
    delay(time_motors);
    motors.setSpeeds(0, 0);


    motors.setSpeeds(mot_spd, mot_spd);
    delay(dist1);
    motors.setSpeeds(0, 0);

    return angle;

}


void dance()
{
    int i = 0;
    while(i<5)
    {
        motors.setSpeeds(mot_spd, -mot_spd);
        delay(100);
        motors.setSpeeds(-mot_spd, mot_spd);
        delay(100);
        i++;
    }
    motors.setSpeeds(0, 0);
}

uint16_t _2hex2dec(uint8_t reg1, uint8_t reg2)
{
    char buff[50];
    uint16_t tempalisa =  (reg1 & 0xf0);

    tempalisa = (tempalisa >> 4);
    tempalisa = tempalisa * 4096;


    uint16_t tempalisa1 = (reg1 & 0x0f);
    tempalisa1 = tempalisa1*256;


    uint16_t tempalisa2 = (reg2 & 0xf0);
    tempalisa2 = tempalisa2>>4;
    tempalisa2 = tempalisa2 * 16;

    uint16_t tempalisa3 = (reg2 & 0x0f);

    uint16_t final = tempalisa+tempalisa1+tempalisa2+tempalisa3;



    return final;
}

void drive()
{
    motors.setSpeeds(45, 45);
}
ISR (INT0_vect)
{
    play_frequency(6000, 150, 15);

}