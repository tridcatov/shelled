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


using namespace std;

#define LEDS_CNT 4
#define BTNS_CNT 4
#define BTN_RESET_STATE     "1"
#define BTN_SET_STATE       "0"
#define LED_RESET_STATE     "0"
#define LED_SET_STATE       "1"

GPIOClass ledPool[LEDS_CNT] = {GPIOClass("26"),GPIOClass("12"),GPIOClass("16"),GPIOClass("20")};
const string ledPins[LEDS_CNT] = {"26", "12", "16", "20"};
GPIOClass btnPool[BTNS_CNT] = {GPIOClass("5"),GPIOClass("6"),GPIOClass("13"),GPIOClass("16")};
const string btnPins[BTNS_CNT] = {"5", "6", "13", "19"};
string LedState;
string BtnState;
      
void __getPinsState(GPIOClass* pinsPool, const uint32_t size, string& out_state)
{
    string inputstate;
    out_state.erase();
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
    packet.erase();
    packet.append("0");
    packet.append(btnsState);
    while(packet.length() < 9)
        packet.append("0");
    if(service)
        fnode_service_notify_state(service, packet.c_str(), packet.length());
}

void SensorChangeHandler(fnode_service_t * service)
{
    if(BtnState.compare(getBtnsState()) != 0)
    {
        usleep(20000);
        const string& unconfirmedState = getBtnsState();
        if(BtnState.compare(unconfirmedState) != 0)
        {
            BtnState.replace(0, unconfirmedState.length(), unconfirmedState);
            sendBtnsState(service, BtnState);
        }    
    }
    /*
    btnPin->getval_gpio(inputstate); //read state of GPIO17 input pin
        if(inputstate == "0") // if input pin is at state "0" i.e. button pressed
        {
                usleep(20000);
                    btnPin->getval_gpio(inputstate); // checking again to ensure that state "0" is due to button press and not noise
            if(inputstate == "0")
            {
                ledPin->setval_gpio("1"); // turn LED ON
                
                while (inputstate == "0"){
                btnPin->getval_gpio(inputstate);
                };            
    
            }
        }
        ledPin->setval_gpio("0");
    */
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
        
    
    const string& state = getBtnsState();
    BtnState.replace(0, state.length(), state);
    sendBtnsState(service, BtnState);
    
    fnode_service_reg_handler(service, request_handler_clbk);
    while(1)
    {        
        SensorChangeHandler(service);
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
        usleep(50000);  // wait for 0.5 seconds

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

