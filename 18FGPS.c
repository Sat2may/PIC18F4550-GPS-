#include <pic18f4550.h>
#include "config.h"
#include "LCD_4.h"
#include "USART.h"
#include <xc.h>
#include <stdbool.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


/*
$GPRMC,141848.00,A,2237.63306,N,08820.86316,E,0.553,,100418,,,A*73
$GPVTG,,T,,M,0.553,N,1.024,K,A*27
$GPGGA,141848.00,2237.63306,N,08820.86316,E,1,03,2.56,1.9,M,-54.2,M,,*74
$GPGSA,A,2,06,02,05,,,,,,,,,,2.75,2.56,1.00*02
$GPGSV,1,1,04,02,59,316,30,05,43,188,25,06,44,022,23,25,03,324,*76
$GPGLL,2237.63306,N,08820.86316,E,141848.00,A,A*65
*/


char location[73];
char out;
unsigned char received_char;
char ggastr[3]= "GGA";
unsigned char buffer[3] = "";
int ggacount, count;
bool flag = false, exit12 = false, printcor = false;
char lat[10], longt[11];
float _lat, _longt;
unsigned char N_S, E_W;   




float convert_to_degrees(float NMEA_lat_long){
	
	float minutes, dec_deg, decimal;
	int degrees;
	float position;

	degrees = (int)(NMEA_lat_long/100.00);
	minutes = NMEA_lat_long - degrees*100.00;
	dec_deg = minutes / 60.00;
	decimal = degrees + dec_deg;
	if (N_S == 'S' || E_W == 'W') { // return negative
		decimal *= -1;
    }	
	/* convert raw latitude/longitude into degree format */
	return decimal;
}






void interrupt Serial_ISR()   
{
	 
	if(RCIF){
		GIE  = 0;							/* Disable global interrupt */
		 received_char = RCREG;
        if(RCSTAbits.OERR){                 /* check if any overrun occur due to continuous reception */           
            CREN = 0;
            NOP();
            CREN=1;
        }
         
         
         if(received_char == ggastr[ggacount] && flag == false){
             
             buffer[ggacount] = received_char;
             ggacount++;
             if(ggacount == 3){
                 ggacount =0;
             }
         }
         
         if( buffer[0] = 'G' && buffer[1] == 'G'  &&  buffer [2] == 'A' && exit12 == false){
             flag = true;
             
             location[count] = received_char;
             
             count++;
             
             if(count == 73){
                  count = 0;
                    flag = false;
                 //exit12 = true;
                    buffer[3] = "";
                    printcor = true;
                 INTCONbits.GIE=0;	/* Enable Global Interrupt */
                    INTCONbits.PEIE=0;	/* Enable Peripheral Interrupt */
                   PIE1bits.RCIE=0;	/* Enable Receive Interrupt */    
                    
             }
             
         }
         GIE  = 1;
         RCIF = 0;
    }
}

// **********START of Main Function**************//
void main(void)
{
    
    char gpsbuf[10];
    char gpsbuf2[11];
    int i,x, y=0;
    Initialize_UART();    //Initialize UART module                    
    LCD_Init();
    //UART_send_string("UART Module Initialized and active");    // Introductory Text
    
    Initialize_UART();    //Initialize UART module    
    
    while(1) //Infinite loop
    {
          if(printcor == true){                      
        //if(received_char == 'G')                      
        for (i = 0; i <= 73; i++){
            
            if(location[i] == 'N' || location[i] == 'S'){
           
              
              for(x = i -11; x<= i-1; x++){
                  lat[y] = location[x];
                  y++;
              }
              y =0;
              //UART_send_string(lat); 
              _lat = atof(lat);
              
                }
            
            if(location[i] == 'E' || location[i] == 'W' ){
                
                for(x = i-12; x<= i-1; x++){
                    
                    longt[y] = location[x];
                  y++;
                }
                 y =0;
                _longt = atof(longt);
                
            }
            
            }
            LCD_String_xy(1,0,"Lt: ");
            sprintf(gpsbuf,"%.05f",_lat);		/* convert float value to string */
            UART_send_string(gpsbuf); 
            LCD_String(gpsbuf);            			/* display latitude in degree */
            memset(gpsbuf,0,10);
           //gpsbuf[10] = "";
               UART_send_char(10);
            
               
            LCD_String_xy(2,0,"lg: ");
            sprintf(gpsbuf2,"%.05f",_longt);		/* convert float value to string */
            UART_send_string(gpsbuf2); 
            LCD_String(gpsbuf2);            			/* display latitude in degree */
            memset(gpsbuf2,0,11);
             // gpsbuf[10] = "";
               UART_send_char(10);
           
              __delay_ms(500);
             LCD_Clear();   
             
             printcor = false;
              INTCONbits.GIE=1;	/* Enable Global Interrupt */
            INTCONbits.PEIE=1;	/* Enable Peripheral Interrupt */
            PIE1bits.RCIE=1;	/* Enable Receive Interrupt */
            
             memset(location,0,73);
             
          }
            
}
  
}
// **********END of Main Function**************//