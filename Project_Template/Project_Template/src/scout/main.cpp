#include <avr/io.h>
#include <avr/interrupt.h>
#include <3pi.h>
#include <stdlib.h>
#include <nrf24.h>
#include <nRF24L01.h>
#include <math.h>
//#include "../../../../../../../Arduino/hardware/tools/avr/avr/include/time.h"
#include <stdio.h>
#include <string.h>
/******************************RF***********************************/
char buff5[2] = {'2', '3'};
char no_ack[3] = {'A', 'C', 'K'};
char buff3[1] = {10};
char rcv_data[4];
char *v;
char reg_data[8] = {'0','0','0','0','0','0','0','0'};
int flag = 1;
float find_angle(float x1, float y1, float x2, float y2);
float find_dist(float x1, float y1, float x2, float y2);
void turn_around(float angle);
float drive(float x1, float y1, float x2, float y2);
void dance();
float drive_homing(float x1, float y1, float theta, float x2, float y2);
#define  mot_spd 73
uint16_t _2hex2dec(uint8_t reg1, uint8_t reg2);

uint8_t rx_address[5] = {0xe1,0xf0,0xf0,0xf0,0xf0};
uint8_t tx_address[5] = {0x12,0x12,0x66,0x4b,0x6f};
uint8_t data_array[32];

uint8_t send_reg[32];
uint8_t rcv_reg[32];
static char serial_buffer [80];
static char serial_buffer1 [80];

char * data_rcvd;
uint8_t temp;
uint8_t q = 0;
char to_serial[32];
/******************************RF***********************************/
void transfer_rcv(char *reg_data);
void setBaudRate(unsigned long baud)

#define  MODE 2
/*  1 - RELAY
 *  2 - OTHERS
 */
#define INIT_PING 1
{
#if _SERIAL_PORTS > 1  // Orangutan X2 and SVP users
    serial_set_baud_rate(UART0, baud);
#else
    serial_set_baud_rate(baud);
#endif
}

// this code sets up timer1 for a 1s  @ 16Mhz Clock (mode 4)
void send_serial_RF(uint8_t *);
char* rcv_serial_RF();
int main(void) {
    nrf24_init();
    nrf24_config(111, 15); // on channel 11 with payload length 4
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    send_reg[6] = 0xF4;
    send_reg[5] = 0x01;
    send_reg[4] = 0xc8;
    send_reg[3] = 0x00;
    send_reg[2] = 0x99;
    send_reg[1] = 0xD9;
    send_reg[0] = 0x60;
    int i = 0;
    int bytes;
    setBaudRate(9600);
    int hi, lo;
    float angle3 = 0;
    serial_send_blocking("hi\n", sizeof("hi\n"));
    sei();
    //set_motors(60,60);
    //delay(5000);
    //set_motors(0,0);
    if (MODE == 1) {
        serial_send_blocking("expecting serial input\n", sizeof("expecting serial input\n"));
        while (1) {
            bytes = 0;
            while (bytes == 0) {

                while (serial_receive_blocking(buff5, sizeof(buff5), 6000000));
                bytes = serial_get_received_bytes();

            }
            *data_array = (buff5[0] - '0') * 10 + (buff5[1] - '0');
            //serial_send_blocking("Received the foll on serial :", sizeof("Received the foll on serial :"));
            serial_send_blocking(buff5, sizeof(buff5));
            serial_send_blocking("\n", sizeof("\n"));

            send_serial_RF(data_array);
            if(nrf24_dataReady())
            {
                serial_send_blocking("received ping back from collector\n", sizeof("received ping back from collector\n"));
            }

        }
        //serial_send_blocking(buff5, sizeof(buff5));
        //break;

    }

    else {
        //float angle3;
        // enable interrupts
        while (1) {
            bytes = 0;
            if(INIT_PING == 1) {
                for(int i = 0; i<1; i++) {
                    send_serial_RF(send_reg);
                    serial_send_blocking("Sent pos\n", sizeof("Sent pos\n"));
                }
                delay(10000);
                send_reg[0] = 0x61;
                serial_send_blocking("sending oob\n", sizeof("sending oob\n"));
                for(int i = 0; i<1;i++)
                {
                    send_serial_RF(send_reg);
                    play_frequency(6000, 150, 13);
                }

            }
            //delay(50);

        }
    }
}


void  send_serial_RF(uint8_t* data_array)
{
    /* Automatically goes to TX mode */
    nrf24_send(data_array);

    /* Wait for transmission to end */
    while (nrf24_isSending());
    /* Make analysis on last tranmission attempt */
    temp = nrf24_lastMessageStatus();

    if (temp == NRF24_TRANSMISSON_OK)
    {
        serial_send_blocking("trans_ok\n", sizeof("trans_ok\n"));
    }
    else if (temp == NRF24_MESSAGE_LOST) {
        {
            serial_send_blocking("msg_lost\n", sizeof("msg_lost\n"));
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
    else if ((x_diff < 0 && y_diff >=0) || ((x_diff < 0 && y_diff <0) ))// 2nd quad and 3rd quad
    {
        theta = theta + (3.14285714);
    }



    theta = theta * (180.0/3.14285714) ;

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
    delay(3000);
    set_motors(-80,80);
    delay(angle1*2.08);
    set_motors(0,0);
}

float drive(float x1, float y1, float x2, float y2)
{
    float angle = find_angle(x1, y1, x2, y2);
    float dist = find_dist(x1, y1, x2, y2);
    int dist1 = dist*500;
    int time_motors = angle * 2.22;

    delay(3000);
    set_motors(-82,82);
    delay(time_motors);
    set_motors(0,0);

    delay(50);
    set_motors(mot_spd-2,mot_spd);
    delay((dist1)*0.5);
    set_motors((0.6*mot_spd-2), 0.6*mot_spd);
    delay(dist1*0.2);
    set_motors((0.3*mot_spd-2), 0.3*mot_spd);
    delay(dist1*0.3);
    set_motors(0,0);
    return angle;

}

float drive_homing(float x1, float y1, float theta, float x2, float y2) {

    if(theta >=0 && theta <= 90)
    {
        theta = 90 - theta;
    }
    else
    {
        theta = abs(450- theta);
    }
    float angle = find_angle(x1, y1, x2, y2) - theta;

    if(angle < 0)
    {
        angle = angle +360;
    }

    float dist = find_dist(x1, y1, x2, y2);

    int dist1 = dist * 500;
    int time_motors = angle * 2.1;

    delay(3000);
    set_motors(-82,82);
    delay(time_motors);
    set_motors(0,0);

    delay(50);
    set_motors(mot_spd,mot_spd);
    delay((dist1)*0.5);
    set_motors((0.6*mot_spd), 0.6*mot_spd);
    delay(dist1*0.2);
    set_motors((0.3*mot_spd), 0.3*mot_spd);
    delay(dist1*0.3);
    set_motors(0,0);

    return angle;

}

void dance()
{
    int i = 0;
    while(i<5)
    {
        set_motors(mot_spd, -mot_spd);
        delay(100);
        set_motors(-mot_spd, mot_spd);
        delay(100);
        i++;
    }
    set_motors(0, 0);
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