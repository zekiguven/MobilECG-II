//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: ADuC70xx (Analog Devices)
//*
//*     TOOLKIT:   ARM GCC
//*
//*     PURPOSE:   Port Test File
//*
//*     Version: 4.00
//*
//*     $Revision: 529 $
//*     $Date:: 2012-04-04 #$
//*
//*     Copyright (c) 2003-2012, Harry E. Zhurov
//*
//*     Permission is hereby granted, free of charge, to any person
//*     obtaining  a copy of this software and associated documentation
//*     files (the "Software"), to deal in the Software without restriction,
//*     including without limitation the rights to use, copy, modify, merge,
//*     publish, distribute, sublicense, and/or sell copies of the Software,
//*     and to permit persons to whom the Software is furnished to do so,
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//*     =================================================================
//*     See http://scmrtos.sourceforge.net for documentation, latest
//*     information, license and contact details.
//*     =================================================================
//*
//******************************************************************************
//*     ARM port by Sergey A. Borshch, Copyright (c) 2006-2012
//*     ADuC70xx port maintenance: Anton B. Gusev aka AHTOXA, Copyright (c) 2011-2012

#include <scmRTOS.h>
#include "pin.h"
#include "display.hpp"
#include <cstring>
#include "logo.h"
//---------------------------------------------------------------------------
//
//      Process types
//
typedef OS::process<OS::pr0, 200> TProc1;
typedef OS::process<OS::pr1, 200> TProc2;
typedef OS::process<OS::pr2, 200> TProc3;
//---------------------------------------------------------------------------
//
//      Process objects
//
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
//---------------------------------------------------------------------------
//
//      IO Pins
//
Pin<1, 1> LED0;
Pin<0, 4> LED1;


OS::TEventFlag ef;
OS::TEventFlag TimerEvent;

volatile int pina;
int main()
{
    // configure IO pins
	//LED0.Alternate(OFF);
	LED0.direct(OUTPUT);
//	LED0.On();
//	LED1.Direct(OUTPUT);
//	LED1.Off();

	
    // run OS
    OS::run();
}


TDisplay<0,3,0,1,0,2,0,0> display;
char fb[128*64/8];

namespace OS 
{
    template<> 
    OS_PROCESS void TProc1::exec()
    {
        for(;;)
        {
            ef.wait();
            //LED0.Off();
        }
    }

    template<> 
    OS_PROCESS void TProc2::exec()
    {
		sleep(10);
		display.init();
		memset(fb,0,sizeof(fb));
		//memset(fb,0xFF,sizeof(fb)/2);
		#define SET_PIXEL(x, y, val) (fb[x + (y/8)*128] |= val << (y & 7));
		
		for (int r=0; r<64; r++){
			for (int c=0; c<128; c++){
				SET_PIXEL(c, r, logo[c+r*128]);
			}
		}
		/*
		for (int r=0; r<64; r++){
			SET_PIXEL(r,r,1);
		}*/
		
		
		display.sendFramebuffer(fb);
		
		bool inv=false;
		for(;;){
			sleep(500);
			inv = !inv;
			display.invert(inv);
		}
		
	}

    template<> 
    OS_PROCESS void TProc3::exec()
    {

        for(;;)
        {
			sleep(30);
			LED0.off();
			
            sleep(30);
            LED0.on();
   
            //ef.signal();
        }
    }
}   // namespace OS


void OS::system_timer_user_hook()
{
	static int timer_event_counter = 5;
	if (!--timer_event_counter)
	{
		timer_event_counter = 5;
       // TimerEvent.signal_isr();
	}
}

void OS::idle_process_user_hook() { }

extern "C" void IRQ_Switch()
{
    uint32_t irq = IRQSTA;
    if(irq & SYSTEM_TIMER_INT)
    {
		   OS::system_timer_isr();
    }
}
