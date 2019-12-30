#include "hall.h"



__IO uint16_t uSaHall;
__IO uint16_t vSaHall;
__IO uint16_t wSaHall; //电机 W相，采样电压值

__IO uint8_t step;


volatile uint8_t SENSE_H;



void NO_HALL_GPIO_Init(void)
{
      gpio_pin_config_t key_config = {
	  kGPIO_DigitalInput ,
	  0,
    };

   /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);
    
    CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_PortC);

	//PORT_SetMultiplePinsConfig();

    /* PORTD0 (pin 93) is configured as PTD0, */
    PORT_SetPinMux(PORTB, 18U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, 8U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, 16U, kPORT_MuxAsGpio);
		
    GPIO_PinInit(NO_HALL_A_GPIO, NO_HALL_A_GPIO_PIN, &key_config);
    GPIO_PinInit(NO_HALL_B_GPIO, NO_HALL_B_GPIO_PIN, &key_config);
	GPIO_PinInit(NO_HALL_C_GPIO, NO_HALL_C_GPIO_PIN, &key_config);
	
    
    PORT_PinPullConfig(HW_GPIOB, 18,  kPORT_PullUp);
    PORT_PinPullConfig(HW_GPIOC, 8,   kPORT_PullUp);
    PORT_PinPullConfig(HW_GPIOE, 16,  kPORT_PullUp);
        
}
/***********************************************
    *
    *
    *
    *
**********************************************/

uint8_t NO_HallSensor_Hex(void)
{

  static uint8_t noHallValue=0x4;

 // noHallValue = NO_HallSensor_GetPinState();
 // PRINTF("d = %d \r\n",noHallValue);
    DelayMs(4);
     
  
  if(noHallValue == 0x4) //0x4 --hall  0x5 0x1
   {

          
         if(uwStep == 5)
           {
              uwStep = 0x4;
              step=4;
              return uwStep ;
           }
         if(uwStep == 4)
           {
              uwStep = 0x6;
              step=6;
              return uwStep ;
           }
         if(uwStep == 6)
           {
              uwStep = 0x2;
              step=2;
              return uwStep ;
           }
         if(uwStep == 2)
           {
              uwStep = 0x3;
              step=3;
              return uwStep ;
           }
         if(uwStep == 3)
           {
              uwStep = 0x1;
              step=1;
              return uwStep ;
           }
         if(uwStep == 1)
           {
              uwStep = 0x5;
              step=5;
              return uwStep ;
           }
          else  
          {

              uwStep = 0x4;
              step=4;
           
              return uwStep ;
          }
        
      
    }
   if((noHallValue == 0x0)||(noHallValue == 0x7)) //0x0 --hall 0x6 0x04
   {
          
            if(uwStep == 5)
           {
              uwStep = 0x4;
              step=4;
              return uwStep ;
           }
         if(uwStep == 4)
           {
              uwStep = 0x6;
              step=6;
              return uwStep ;
           }
         if(uwStep == 6)
           {
              uwStep = 0x2;
              step=2;
              return uwStep ;
           }
         if(uwStep == 2)
           {
              uwStep = 0x3;
              step=3;
              return uwStep ;
           }
         if(uwStep == 3)
           {
              uwStep = 0x1;
              step=1;
              return uwStep ;
           }
         if(uwStep == 1)
           {
              uwStep = 0x5;
              step=5;
              return uwStep ;
           }
         else
         {
              uwStep = 0x5;
              step=5;
           
              return uwStep ;

         }
          
    }
   if(noHallValue == 0x3) //0x3 --hall 0x2,0x03
   {

         
         if(uwStep == 5)
           {
              uwStep = 0x4;
              step=4;
              return uwStep ;
           }
         if(uwStep == 4)
           {
              uwStep = 0x6;
              step=6;
              return uwStep ;
           }
         if(uwStep == 6)
           {
              uwStep = 0x2;
              step=2;
              return uwStep ;
           }
         if(uwStep == 2)
           {
              uwStep = 0x3;
              step=3;
              return uwStep ;
           }
         if(uwStep == 3)
           {
              uwStep = 0x1;
              step=1;
              return uwStep ;
           }
         if(uwStep == 1)
           {
              uwStep = 0x5;
              step=5;
              return uwStep ;
           }
        else
          {
           uwStep = 0x3;
           step=3;
       
           return uwStep ;
          }
     
   }
   if(noHallValue == 0x1) 
   {
         
        if(step == 5)
           {
              uwStep = 0x4;
              step=4;
              return uwStep ;
           }
         if(step == 4)
           {
              uwStep = 0x6;
              step=6;
              return uwStep ;
           }
         if(step == 6)
           {
              uwStep = 0x2;
              step=2;
              return uwStep ;
           }
         if(step == 2)
           {
              uwStep = 0x3;
              step=3;
              return uwStep ;
           }
         if(step == 3)
           {
              uwStep = 0x1;
              step=1;
              return uwStep ;
           }
         if(step == 1)
           {
              uwStep = 0x5;
              step=5;
              return uwStep ;
           }
          else
          {
           uwStep = 0x1;
           step=1;
         
           return uwStep ;
          }
   }
   if(noHallValue == 0x5)
   {
      if(uwStep == 5)
        {
           uwStep = 0x4;
           step=4;
           return uwStep ;
        }
      if(uwStep == 4)
        {
           uwStep = 0x6;
           step=6;
           return uwStep ;
        }
      if(uwStep == 6)
        {
           uwStep = 0x2;
           step=2;
           return uwStep ;
        }
      if(uwStep == 2)
        {
           uwStep = 0x3;
           step=3;
           return uwStep ;
        }
      if(uwStep == 3)
        {
           uwStep = 0x1;
           step=1;
           return uwStep ;
        }
      if(uwStep == 1)
        {
           uwStep = 0x5;
           step=5;
           return uwStep ;
        }
      else
      {

         uwStep = 0x5;
         step=5;
          return uwStep ;


      }
   }
      if(noHallValue == 0x2)
         {
            if(uwStep == 5)
              {
                 uwStep = 0x4;
                 step=4;
                 return uwStep ;
              }
            if(uwStep == 4)
              {
                 uwStep = 0x6;
                 step=6;
                 return uwStep ;
              }
            if(uwStep == 6)
              {
                 uwStep = 0x2;
                 step=2;
                 return uwStep ;
              }
            if(uwStep == 2)
              {
                 uwStep = 0x3;
                 step=3;
                 return uwStep ;
              }
            if(uwStep == 3)
              {
                 uwStep = 0x1;
                 step=1;
                 return uwStep ;
              }
            if(uwStep == 1)
              {
                 uwStep = 0x5;
                 step=5;
                 return uwStep ;
              }
            else
            {
      
               uwStep = 0x2;
               step=2;
                return uwStep ;
      
      
            }
      }
      
      if(noHallValue == 0x6)
         {
            if(uwStep == 5)
              {
                 uwStep = 0x4;
                 step=4;
                 return uwStep ;
              }
            if(uwStep == 4)
              {
                 uwStep = 0x6;
                 step=6;
                 return uwStep ;
              }
            if(uwStep == 6)
              {
                 uwStep = 0x2;
                 step=2;
                 return uwStep ;
              }
            if(uwStep == 2)
              {
                 uwStep = 0x3;
                 step=3;
                 return uwStep ;
              }
            if(uwStep == 3)
              {
                 uwStep = 0x1;
                 step=1;
                 return uwStep ;
              }
            if(uwStep == 1)
              {
                 uwStep = 0x5;
                 step=5;
                 return uwStep ;
              }
            else
            {
      
               uwStep = 0x6;
               step=6;
                return uwStep ;
      
      
            }
      }
  
   return uwStep;
}

/*********************************************************
 *
 *Funtion Name: NO_Sensorless_GetPinState
 *
 *
 *
*********************************************************/

#if 0
//############################################################################
// + Interruptroutine
// + Wird durch den Analogkomperator ausgelöst
// + Dadurch wird das Kommutieren erzeugt
////{AB,AC,BC,BA,CA,CB} Phase =0 =AB通电
//############################################################################
uint8_t Dector_Phase(uint8_t duty)
{
      uint8_t sense = 0,hex;
      uint8_t sense_u[20], sense_v[20],sense_w[20],i=0; 
      sense_u[0]= 1;
      sense_v[0]= 0;
      sense_w[0] = 1;
   
    ADC_UVW_Sample_HALL_Value();

       #ifdef DEBUG_PRINT  
            PRINTF("u= %d\r\n", uSaHall);
            PRINTF("v= %d\r\n", vSaHall);
            PRINTF("w= %d\r\n",wSaHall);
         #endif 
     
      
   
      

     // do  //消磁检测事件
      {
         if(SENSE_H) 
            sense = 1; 
         else 
            sense = 0;
         
         switch(Phase)  //{AB,AC,BC,BA,CA,CB} Phase =0 =AB通电
         {
            case 0:  //A+ B- = "2" 换相到 A+ C- = '6'
                     
                        if(wSaHall == 0) //A+ B- 到A+ C- 换向
                        {
                           
                                 /*  A+ C- = "6" */ 
                               //  PWMA_Close_ABC_Channel(1); //close B channel 
                                 /*  Channel configuration A+ */
                               //  PWMA_Select_AB_Channel(1,duty);
                               hex= 0x06;
                              
                              PRINTF("Hex= %d \r\n",hex);
                              Phase++;
                              i++;
                              PRINTF("Phase= %d \r\n",Phase);
                                 
                           
                         
                       //  return hex;     
                        } 
                
                  else
                  {
                        /*A+ B- = '2'*/
                      //  PWMA_Close_ABC_Channel(2); //close C channel 
                      //  PWMA_Select_AB_Channel(0,duty);
                      hex= 0x2;
                    
                     PRINTF("Hex= %d \r\n",hex);
                    //  return hex;
                  }
                        
                  
            break;
            case 1:   /*  A+ C- = "6"  换成 B+ C-= '4'*/   
                    
                        if(vSaHall  > 4000 ) //A+ C- 换成 B+ C-
                        { 
                           
                              /*  B+ C- = "4" */ 
                             // PWMA_Close_ABC_Channel(0); //close A channel 
                              /*  Channe3 configuration B+  */
                             // PWMA_Select_BC_Channel(1,duty);
                             hex = 0x4;
                       
                            PRINTF("Hex= %d \r\n",hex);
                             Phase++;
                           i++;
                           PRINTF("Phase= %d \r\n",Phase);
                            //    return hex;
                        
                       
                        }
                  
                  else
                  {
                        /*  A+ C- = "6" */ 
                    //    PWMA_Close_ABC_Channel(1); //close B channel 
                     /*  Channel configuration A+ */
                       // PWMA_Select_AB_Channel(1,duty);
                        hex = 0x6;
                         PRINTF("Hex= %d \r\n",hex);
                      //  return hex;   
                  }

                  break;
            case 2:  // B+ C- ='4'换成 B + A- '5'
                       
                         if(uSaHall==0 ) // vSaHall = "A"
                         {
                           
                              //从B+ C-='4' 换成 B+ A- = '5'
                              /*  B+ A-  = '5' */ 
                            //  PWMA_Close_ABC_Channel(2);  //close C channel 
                              /*  PWM_A1 and PWM_B1 output */
                            //  PWMA_Select_BC_Channel(0,duty);
                             hex = 0x5;
                              PRINTF("Hex= %d \r\n",hex);
                         //    return hex;
                              Phase++;
                              i++;
                              PRINTF("Phase= %d \r\n",Phase);
                           //   return hex;
                           
                         
                        
                        }
                    
                     else
                     {
                           /*B+ C- = '4'*/
                          // PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe3 configuration B+  */
                          // PWMA_Select_BC_Channel(1,duty);
                           hex = 0x4;
                            PRINTF("Hex= %d \r\n",hex);
                        
                          // return hex;
                     }
                     
                  break;
            case 3:  //B+ A- ='5' 换成 C+ A- = '1'
                  
              
                        if(wSaHall > 4000)
                        {
                           /*  C+ A- = '1' */       
                           //PWMA_Close_ABC_Channel(1); //close B channel 
                          // PWMA_Select_CA_Channel(0,duty);
                            hex = 0x1;
                            PRINTF("Hex= %d \r\n",hex);
                          //   return hex;
                           Phase++;
                        i++;
                        PRINTF("Phase= %d \r\n",Phase);
                       //   return hex;
                        }
                     else
                     {
                          /*  B+ A-  = '5' */ 
                         //  PWMA_Close_ABC_Channel(2);  //close C channel 
                           /*  PWM_A1 and PWM_B1 output */
                         //  PWMA_Select_BC_Channel(0,duty);
                             hex = 0x5;
                              PRINTF("Hex= %d \r\n",hex);
                          //  return hex;
                     }
                     

                  break;
            case 4:  //C+ A- ='1' 换成 C+ B- = '3'
                       
                        if(vSaHall==0)
                        {
                           /*  C+ B- = '3' */ 
                          // PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe2 configuration */
                          // PWMA_Select_CA_Channel(1,duty);
                           hex = 0x3;
                            PRINTF("Hex= %d \r\n",hex);
                             Phase++;
                          PRINTF("Phase= %d \r\n",Phase);
                        }
                        
                     
                
                    if(uSaHall ==0 && wSaHall > 3000 )
                     {
                         /*  C+ A- = '1' */       
                          // PWMA_Close_ABC_Channel(1); //close B channel 
                          // PWMA_Select_CA_Channel(0,duty);
                           hex = 0x1;
                           PRINTF("Hex= %d \r\n",hex);
                         //  return hex;
                     }
                     
                  break;
            case 5:  //C+ B- ='3' 换成 A+ B-  ='2'
                     
          
                     
                        if(uSaHall > 4000)
                        {
                          
                             /* A+ B- ='2' */ 
                            // PWMA_Close_ABC_Channel(2); //close C channel 
                            // PWMA_Select_AB_Channel(0,duty);
                             hex = 0x2;
                              PRINTF("Hex= %d \r\n",hex);
                          //   return hex;
                          Phase = 0;
                          i=0;
                          PRINTF("Phase= %d \r\n",Phase);
                          
                      //   CADC_ClearStatusFlags(DEMO_CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);
                         // return hex;
                        
                        }
               
                     if(vSaHall ==0 && wSaHall > 3000)
                     {
                         /*C+ B- = '3'*/
                        // PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe2 configuration */
                         //PWMA_Select_CA_Channel(1,duty);
                          hex = 0x3;
                           PRINTF("Hex= %d \r\n",hex);
                      //  return hex;
                         
                     }
                     
                  break;
         }
    //   printf("Phase = %d\r \n", Phase);
      }
    //  while(SENSE_H && !sense);  //消磁事件检测 
      return hex;
     
}

#endif 






