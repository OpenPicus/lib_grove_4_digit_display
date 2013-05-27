/** \file 4digitdisplay.c
 *  \brief Grove devices support library 
 */

/**
\addtogroup Grove devices
@{
 **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        4digitdisplay.c
 *  Dependencies:    OpenPicus libraries
 *  Module:          FlyPort WI-FI - FlyPort ETH
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Davide Vicca	     1.0     05/10/2013		   First release  
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/

#include "taskFlyport.h"
#include "grovelib.h"
#include "4digitdisplay.h"


struct Interface *attachSensorToDigioBus(void *,int,int);


/*display array*/
const BYTE TubeTab[] = {0x3f,0x06,0x5b,0x4f,
                           0x66,0x6d,0x7d,0x07,
                           0x7f,0x6f,0x77,0x7c,
                           0x39,0x5e,0x79,0x71};//0~9,A,b,C,d,E,F                        

/*_4DigitDisplay grove class*/
struct _4Digit
{
	const void *class;
	struct Interface *inter;
	BYTE firstDigit;
	BYTE secondDigit;
	BYTE thirdDigit;
	BYTE fourthDigit;
	BYTE brightness;
	BYTE point;
};

/*send the STOP Command*/
static void stop(struct _4Digit *self)
{
	IOPut(self->inter->port->Pin1,OFF);
	Delay10us(1);
	IOPut(self->inter->port->Pin2,OFF);
	Delay10us(1);
	IOPut(self->inter->port->Pin1,ON);
	Delay10us(1);
	IOPut(self->inter->port->Pin2,ON);
}

/*send the START Command*/
static void start(struct _4Digit *self)
{
	IOPut(self->inter->port->Pin1,ON);
	IOPut(self->inter->port->Pin2,ON);
	Delay10us(1);
	IOPut(self->inter->port->Pin2,OFF);
}

/*send the ACK Command*/
static void ack(struct _4Digit *self) 
{
	IOPut(self->inter->port->Pin1,OFF);
	Delay10us(1);	
	IOInit(self->inter->port->Pin2,IN);
	while(IOGet(self->inter->port->Pin2));
	IOInit(self->inter->port->Pin2,OUT);
	IOPut(self->inter->port->Pin1,ON);
	Delay10us(1);	
	IOPut(self->inter->port->Pin1,OFF);
}

/*send the data BYTE to be written*/
static void writeByte(struct _4Digit *self,BYTE data)
{
	int i;
	for(i=0;i<8;i++)
	{
		IOPut(self->inter->port->Pin1,OFF);
		if(data & 0x01)
			IOPut(self->inter->port->Pin2,ON);
		else
			IOPut(self->inter->port->Pin2,OFF);
		data = data>>1;
		IOPut(self->inter->port->Pin1,ON);
		Delay10us(1);	
	}	
}

/*program the 4 didit display*/
static void program(struct _4Digit *self) // Write display register
{
	start(self);
	writeByte(self,0x40); 
	ack(self);
	stop(self);
	start(self);
	writeByte(self,0xC0); 
	ack(self);
	writeByte(self,self->firstDigit); 
	ack(self);
	writeByte(self,self->secondDigit); 
	ack(self);
	writeByte(self,self->thirdDigit); 
	ack(self);
	writeByte(self,self->fourthDigit);
	ack(self);
	stop(self);
	start(self);
	writeByte(self,self->brightness); 
	ack(self);
	stop(self);
}

/*coding the data BYTE to be displayed*/
static BYTE coding(BYTE point,BYTE DispData)
{
	BYTE PointData;
  	if(point)
		PointData = 0x80;
  	else 
		PointData = 0; 
  	if(DispData == 0x7f) 
		DispData = 0x00 + PointData;
  	else 
		DispData = TubeTab[DispData] + PointData;
  	return DispData;
}

/*clear all 4 digits*/
static void clearDisplay(struct _4Digit *self)
{
	start(self);
	writeByte(self,0x40); 
	ack(self);
	stop(self);
	start(self);
	writeByte(self,0xC0); 
	ack(self);
	writeByte(self,0x00); 
	ack(self);
	writeByte(self,0x00); 
	ack(self);
	writeByte(self,0x00); 
	ack(self);
	writeByte(self,0x00);
	ack(self);
	stop(self);
	start(self);
	writeByte(self,self->brightness); 
	ack(self);
	stop(self);
}

/**
 * static void *_4Digit_ctor (void * _self, va_list *app)- 4 Digit Display grove device Constructor  
 * \param *_self - pointer to the 4 Digit Display grove device class.
 * \param *app 
 * \		1- display brightness
 * \		2- Points 
* \return - Pointer to the _4Digit_ctor devices instantiated
*/
static void *_4Digit_ctor (void * _self, va_list *app)
{
	struct _4Digit *self = _self;
	self->brightness = va_arg(*app, const BYTE) + 0x87;
	/*Reset all display's digits*/
	self->point = va_arg(*app, const BYTE);
	self->firstDigit = 0x00;
	self->secondDigit = 0x00;
	self->thirdDigit = 0x00;
	self->fourthDigit = 0x00;
	self->inter = NULL;
	return self;
}	


/**
 * static void _4Digit_ctor (void * _sensor)- 4 Digit display grove device Destructor  
 * \param *_sensor - pointer to the 4 Digit display grove device class.
 * \return - None
*/
static void _4Digit_dtor (void * _sensor)
{
	struct _4Digit *sensor = _sensor;
	if(sensor->inter)
	{
		free(sensor->inter);
	}
}	


/**
 * static void* _4Digit_dtor (void * _board,void *_sensor,int n) - attach a 4 Digit display grove device to the GroveNest DIGX port  
 * \param *_board - pointer to the GroveNest 
 * \param *_sensor - pointer to the 4 Digit display grove device class.
 * \param n -  which DIG port the device is connected to
 * \return 
 <UL>
	<LI><Breturn = Pointer to the DIG interface created:</B> the operation was successful.</LI> 
	<LI><B>return = NULL:</B> the operation was unsuccessful.</LI> 
 </UL>
 */
static void *_4Digit_attach (void * _board,void *_sensor,int n)
{
	struct _4Digit *sensor = _sensor;
	sensor->inter = attachSensorToDigioBus(_board,n,5);//set 2 pins as output	
	if(!sensor->inter)
		return NULL;
	program(sensor);	
	return sensor->inter;
}	

/**
 * static int _4Digit_config(void *_self,va_list *app) - configure the 4 Digit display grove device to the GroveNest
 * \param *_self - pointer to the 4 Digit display grove device class.
 * \param *app 
 * \		1- param: digit number
 * \		2- data : data to be written on digit number 
 * \return 
 <UL>
	<LI><B>return = 0:</B> The operation is always successful.</LI> 
 </UL>
 */
static int _4Digit_config(void *_self,va_list *app)
{ 
	struct _4Digit *self = _self;
	BYTE param = va_arg(*app, const BYTE);
	BYTE data = va_arg(*app, const BYTE);
	data = coding(self->point,data);
	switch(param)
	{
		case 1:
			self->firstDigit = data;
		break;
		case 2:
			self->secondDigit = data;
		break;
		case 3:
			self->thirdDigit = data;
		break;
		case 4:
			self->fourthDigit = data;
	}
	return 0;
}

/**
 *  static int _4Digit_set(void *_self,va_list *app) - Set a new 4 digit display operation.
 * \param *_self - pointer to the device 
 * \param *app 
 * \		1- param: ON/OFF
 * \return 
 * \return:
	<LI><B>return = 0:</B> The operation is always successful.</LI> 
 </UL>
 */
static int _4Digit_set(void *_self,va_list *app)
{ 
	struct _4Digit *self = _self;
	BYTE param = va_arg(*app, const BYTE);	
	if(param)
		program(self);
	else
		clearDisplay(self);
	return 0;
}


static const struct SensorClass __4Digit =
{	
	sizeof(struct _4Digit),
	_4Digit_ctor,
	_4Digit_dtor,
	_4Digit_attach,
	_4Digit_config,
	_4Digit_set,
	0,
};

const void *_4Digit = &__4Digit;



