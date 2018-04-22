/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ilbe0418
 *
 * Created on 21 апреля 2018 г., 14:36
 */

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GPIOClass.h"
#include <fnode/service.h>
#include <memory>
#include <pthread.h>


using namespace std;

#define MODE_RELY "RELY"
#define MODE_COCL "COCL"
#define MODE_TADC "TADC"


#define LEDS_CNT 2
#define BTNS_CNT 2
#define BTN_RESET_STATE     "1"
#define BTN_SET_STATE       "0"
#define LED_RESET_STATE     "0"
#define LED_SET_STATE       "1"

GPIOClass ledPool[LEDS_CNT] = {GPIOClass("26"),GPIOClass("12")};//,GPIOClass("16"),GPIOClass("20")};
const string ledPins[LEDS_CNT] = {"26", "12"};//, "16", "20"};
GPIOClass btnPool[BTNS_CNT] = {GPIOClass("5"),GPIOClass("6")};//,GPIOClass("13"),GPIOClass("19")};
const string btnPins[BTNS_CNT] = {"5", "6"};//, "13", "19"};
string LedState;
string BtnState;

int getCoreTemp()
{
	float temp = 0.;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    
    stream = popen("/opt/vc/bin/vcgencmd measure_temp 2>&1", "r");	
    if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) 
			{					
				sscanf(buffer, "temp=%f'C", &temp);				
			}
		pclose(stream);
    }
    return ((int)temp - 30);
}
      
void __getPinsState(GPIOClass* pinsPool, const uint32_t size, string& out_state)
{
    string inputstate;
    out_state.clear();
    for(uint32_t i = 0;i < size; i++)
    {
        pinsPool[i].getval_gpio(inputstate);
        out_state.append(inputstate); 
    }
}        
const string& getLedsState()
{
    static string sState;	
    __getPinsState(ledPool, LEDS_CNT, sState);	
    return sState;
}

const string& getBtnsState()
{
    static string sState;
    __getPinsState(btnPool, BTNS_CNT, sState);
	for(int i = 0;i < sState.length(); i++)
	{
		sState[i] = (sState[i] == '0') ? '1' : '0';
	}
    return sState;
}

void setLedState(GPIOClass* pledPin, const string new_state)
{
    pledPin->setval_gpio(new_state);
}

void sig_handler(int sig);
bool ctrl_c_pressed = false;
void request_handler_clbk(uint32_t cmd, char const * data, uint32_t size) 
{    
	if(size > 2)
    {
        int led_i = int(data[1]-'0') % LEDS_CNT;
        setLedState(&ledPool[led_i], (data[2] != '0') ? LED_SET_STATE : LED_RESET_STATE);
    }
}

void sendBtnsState(fnode_service_t * service, const string& btnsState)
{
    static string packet;
    packet.clear();
    packet.append("0");
    packet.append(btnsState);
    while(packet.length() < 9)
        packet.append("0");
    if(service)
        fnode_service_notify_state(service, packet.c_str(), packet.length());
}

void* SensorChangeHandler(void* service)
{
	for(;;)
	{
		while(BtnState.compare(getBtnsState()) == 0)
		{
			usleep(1);
		}
		
		usleep(20000);
		const string& unconfirmedState = getBtnsState();
		if(BtnState.compare(unconfirmedState) != 0)
		{
			BtnState.replace(0, unconfirmedState.length(), unconfirmedState);			
			sendBtnsState((fnode_service_t *)service, BtnState);
		}  
	}    
}

void* TempChangeHandler(void* service)
{
	static string msg="";
	static char temp_str[32];
	for(;;)
	{		
		sprintf(temp_str, "%d", getCoreTemp());
		msg.clear();
		msg.append("0");
		msg.append(temp_str);
		//cout << msg << endl;
		fnode_service_notify_state((fnode_service_t *)service, msg.c_str(), msg.length());
		usleep(1000000L);				
	}    
}
/*
 * 
 */
int main(int argc, char** argv) {
	
	if (argc < 3)
	{
		fprintf(stderr, "Usage: getd SERIAL-NUMBER [COCL|RELY]\n");
		return -1;
	}

	const string MODE(argv[2]);
	
    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sig action" << endl;
        exit(1);
    }
    
    for(int i = 0;i < LEDS_CNT; i++)
    {
        //ledPool[i] = shared_ptr<GPIOClass>(new GPIOClass(ledPins[i])); 
        ledPool[i].export_gpio();
        ledPool[i].setdir_gpio("out");
        setLedState(&ledPool[i], LED_RESET_STATE);
    }
    
    for(int i = 0;i < BTNS_CNT; i++)
    {
        //btnPool[i] = new shared_ptr<GPIOClass>(new GPIOClass(btnPins[i])); 
        btnPool[i].export_gpio(); 
        btnPool[i].setdir_gpio("in");
    }
    
    fnode_service_t * service = fnode_service_create(argv[1], argv[2]);
    if(!service)
    {
        fprintf(stderr, "Failed to create SENS service\n");
        exit(1);
    }
        
    //оптавляем текущие значения серверу
    const string& state = getBtnsState();
	BtnState.erase();
    BtnState.replace(0, state.length(), state);
    sendBtnsState(service, BtnState);
    
	if(MODE.compare(MODE_RELY) == 0)
	{
		fnode_service_reg_handler(service, request_handler_clbk);
	} 
	else if(MODE.compare(MODE_COCL) == 0)
	{
		cout << "SENS Thread is started" << endl;
		pthread_t periph_control_thread;
		pthread_create(&periph_control_thread, NULL, SensorChangeHandler, (void*)service);	
		cout << "SENS Thread is run" << endl;
	}
	else if(MODE.compare(MODE_TADC) == 0)
	{
		cout << "TEMP Thread is started" << endl;
		pthread_t periph_control_thread;
		pthread_create(&periph_control_thread, NULL, TempChangeHandler, (void*)service);	
		cout << "TEMP Thread is run" << endl;
	}
	
    while(1)
    {      
		//SensorChangeHandler(service);
        fnode_service_update(service);
        if(ctrl_c_pressed)
                    {
                        cout << "Ctrl^C Pressed" << endl;   
										
                        //btnPin->unexport_gpio();
                        //ledPin->unexport_gpio();                       
                        //delete btnPin;                        
                        //delete ledPin;
                        //btnPin = 0;
                        //ledPin =0;
                        break;

                    }
        //usleep(20000);  // wait for 0.05 seconds

    }	
    fnode_service_release(service);
    cout << "Exiting....." << endl;
    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}

