
#include <Arduino.h>
#include <Zumo32U4.h>
#include <nrf24.h>
#include <nRF24L01.h>
#include "../../libs/arduino/HardwareSerial.h"






#define  HAL 1
const unsigned char byte2ascii_tab[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/******************************RF***********************************/
int flag = 1;
char buff[2] = {'O', 'K'};
char buff1[3] = {'N', 'O', 'K'};
char buff3[1] = {10};
char reg_data[8] = {'0','0','0','0','0','0','0','0'};

uint8_t rx_address[5] = {0xb1,0x7e,0xf6,0xd1,0x9c};
uint8_t tx_address[5] = {0x45,0x23,0x54,0x0f,0x15};
uint8_t data_array[6];
uint8_t data_array_temp[8];
uint8_t temp;
uint8_t q = 0;
/******************************RF***********************************/
#define MODE 2
#define  INIT_PING 1
int m = 0;
void dance();
float drive_homing(float x1, float y1, float theta, float x2, float y2);
Zumo32U4Motors motors;
typedef struct coordinates{
    int x;
    int y;

}coordiantes;
float distances [] = {0,0,0};
float find_angle(float x1, float y1, float x2, float y2);
float find_dist(float x1, float y1, float x2, float y2);
void turn_around(float angle);
float drive(float x1, float y1, float x2, float y2);
void insertionSort(float distances[], int n);
#define  mot_spd  210

void  send_serial_RF(uint8_t* data_array);
void set_mtr_speed(int , int );
void send_message(uint8_t* data_array);

bool get_message(uint8_t * data_array);


void setup() {
    int m =0;
    Serial1.begin(9600);
    nrf24_init();
    nrf24_config(111, 4); // on channel 111 with payload length 4
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
   // Serial1.println("COLLECTOR");
}



void loop() {
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    uint8_t send_reg[32];
    send_reg[2] = 0xde;
    send_reg[3] = 0x02;
    send_reg[1] = 0xa0;
    send_reg[0] = 0x60;



    float angle3;
    uint8_t status = nrf24_getStatus();
    //delay(20);
    //get the serial data from the dev cable
    //you know its ping msg from ref
    //put that thing to data array and return
    get_message(data_array);

    //delay(200);
    switch (data_array[0]) {
        case 0x50:
           // Serial1.print(data_array[0], HEX);
            //Serial1.print(data_array[1], HEX);
            //Serial1.println(data_array[2], HEX);

            if (data_array[2] == 0xff)
                data_array[1]++;
            data_array[0]++;
            data_array[2]++;
            send_message(data_array);

            flag = 1;
            break;

        case 0x51:
            Serial1.println("ACK for PING\n");
            Serial1.print(data_array[0], HEX);
            Serial1.print(data_array[1], HEX);
            Serial1.println(data_array[2], HEX);
            flag = 1;
            break;

        case 0x60:
            if (!flag)
                break;
            Serial1.print("Theta :");
            Serial1.println(data_array[1], HEX);
            Serial1.print("X     :");
            Serial1.println(data_array[2], HEX);
            Serial1.print("Y     :");
            Serial1.println(data_array[3], HEX);
            angle3 = drive_homing((float) data_array[2], (float) data_array[3], (float) data_array[1], 0, 0);
            angle3 = angle3 + 1;
            angle3 = angle3 - 1;
            dance();
            Serial1.println("------------------------------------");
            //send_message(data_array);
            /* angle3 = drive((float)data_array[2], (float)data_array[2], 0, 0);
             angle3 = angle3+1;
             angle3 = angle3-1;*/
            flag = 0;
            break;

        case 0x61:
            if (!flag)
                break;
        for(int i = 0; i < 30; ++i) {

            set_mtr_speed(0, 0);
            delay(1000);
        }
            set_mtr_speed(50, 50);
            break;
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
        theta = 2*(22/7) + theta;
    }
    else if ((x_diff < 0 && y_diff >=0) || (x_diff < 0 && y_diff <0)  ) // 2nd quad and 3rd quad
    {
        theta = theta + (22/7);
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
    //delay(2000);
    float angle = find_angle(x1, y1, x2, y2) - theta;
    float dist = find_dist(x1, y1, x2, y2);
    int dist1 = dist * 400;
    int time_motors = angle * 4.1;

    delay(1000);
    set_mtr_speed(-mot_spd, mot_spd);

    delay(time_motors);
    set_mtr_speed(0, 0);


    set_mtr_speed(mot_spd, mot_spd);
    delay(dist1);
    set_mtr_speed(0, 0);

    return angle;

}


void dance()
{
    int i = 0;
    while(i<5)
    {
        set_mtr_speed(mot_spd, -mot_spd);
        delay(100);
        set_mtr_speed(-mot_spd, mot_spd);
        delay(100);
        i++;
    }
    set_mtr_speed(0, 0);
}



/******************************HAL***********************************/

bool get_message(uint8_t* data_array)
{
    String incomingString;
    int temp_;
   // Serial1.println("in get msg");

#ifdef HAL
    while(Serial1.available() == 0)
        ;
    while(Serial1.available() > 0) {


        incomingString = Serial1.readString();
      //  Serial1.println(incomingString);
        // assuming the data is in test_msg_devPort buffer
        for(int i = 0 ; i <incomingString.length(); i++){
            //Serial1.println(incomingString);
            //Serial1.println(incomingString[i]);
            if(incomingString[i] == 'a')
                data_array_temp[i] = 0xa;
            else if(incomingString[i] == 'b')
                data_array_temp[i] = 0xb;
            else if(incomingString [i] == 'c')
                data_array_temp[i] = 0xc;
            else if(incomingString[i] == 'd')
                data_array_temp[i] = 0xd;
            else if(incomingString[i] == 'e')
                data_array_temp[i] = 0xe;
            else if(incomingString[i] == 'f')
                data_array_temp[i] = 0xf;
            else if(incomingString[i] == '0')
                data_array_temp[i] = 0x0;
            else if(incomingString[i] == '1')
                data_array_temp[i] = 0x1;
            else if(incomingString[i] == '2')
                data_array_temp[i] = 0x2;
            else if(incomingString[i] == '3')
                data_array_temp[i] = 0x3;
            else if(incomingString[i] == '4')
                data_array_temp[i] = 0x4;
            else if(incomingString[i] == '5' )
                data_array_temp[i] = 0x5;
            else if(incomingString[i] == '6')
                data_array_temp[i] = 0x6;
            else if(incomingString[i] == '7')
                data_array_temp[i] = 0x7;
            else if(incomingString[i] == '8')
                data_array_temp[i] = 0x8;
            else if(incomingString[i] == '9')
                data_array_temp[i] = 0x9;
        }

        int n = 0;
        for(int k=0;k<8; k=k+2)
        {
            data_array[n] = 16*data_array_temp[k] + data_array_temp[k+1];
            n++;
        }

        /*
        char temp[10];

        int k ;
        for( k = 0; k < 6 ; k++)
        {
            itoa(data_array[k], temp+k, 16);

        }
        temp[k] = '\0';
        Serial1.print(temp);
        Serial.flush();
        */
        //  while()
        //memcpy(temp_, data_array, sizeof temp_);
        return true;
    }

#else

    if (nrf24_dataReady())
        nrf24_getData(data_array);
    return false;

#endif
}

void send_message(uint8_t* data_array)
{

#ifdef HAL

    Serial1.print(data_array[0], HEX);
    Serial1.print(data_array[1], HEX);
    Serial1.println(data_array[2], HEX);

#else
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
#endif
}
void set_mtr_speed(int mtr_speed, int speed) {
#ifdef HAL
    Serial1.print("left motor speed ");
    Serial1.println(-mtr_speed);
    Serial1.print("right motor speed ");
    Serial1.println(mtr_speed);
#else
    motors.setSpeeds(-mot_spd, mot_spd);
#endif
}

/******************************HAL***********************************/
