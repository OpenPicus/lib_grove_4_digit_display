lib_grove_4_digit_display
=========================

OpenPicus Flyport library for 4 digit display. This kind of display can show hex value, and can be usefule to display the time or any other hex value. You can find more infos on OpePicus wiki: wiki.openpicus.com.<br>
<br>
To use the library inside your Flyport project, just follow these steps:<br>
1) Create a new project from a Grove template.<br>
2) import files inside Flyport IDE using the external libs button.<br>
3) add following code example in FlyportTask.c:

<pre>
#include "taskFlyport.h"
#include "grovelib.h"
#include "4digitdisplay.h"
 
 
unsigned char mex[20];
 
void FlyportTask()
{  
	vTaskDelay(100);
	UARTWrite(1,"Welcome to GROVE NEST example!\r\n");
 
	// GROVE board
	void *board = new(GroveNest);
 
	// 4 digit GROVE display 	
	void *disp = new(_4Digit,6,POINT);
	attachToBoard(board,disp,DIG1);
 
	UARTWrite(1,"Flyport connected... hello world!\r\n");
	vTaskDelay(200);
	while(1)
	{
		/*Configure digit number and number to be displayed*/
		configure(disp,1,1);
		configure(disp,2,2);
		configure(disp,3,3);
		configure(disp,4,4);
 
		/*Turn the display on*/
		set(disp,ON);
		vTaskDelay(200);
		/*Turn the display off*/
		set(disp,OFF);
 
		configure(disp,1,5);
		configure(disp,2,6);
		configure(disp,3,7);
		configure(disp,4,8);
 
		/*Blinking*/
		set(disp,ON);
		vTaskDelay(100);
		set(disp,OFF);
		vTaskDelay(100);
		set(disp,ON);
		vTaskDelay(100);
		set(disp,OFF);
		vTaskDelay(100);
		set(disp,ON);
		vTaskDelay(100);
		set(disp,OFF);
 
		configure(disp,1,9);
		configure(disp,2,10);
		configure(disp,3,11);
		configure(disp,4,12);
		vTaskDelay(100);
		set(disp,ON);
		vTaskDelay(100);
		set(disp,OFF);
		vTaskDelay(100);
		configure(disp,1,0x0D);
		configure(disp,2,0x0E);
		configure(disp,3,0x0F);
		configure(disp,4,4);
		set(disp,ON);
		vTaskDelay(200);
		set(disp,OFF);
 
	}		
 
}

</pre>
