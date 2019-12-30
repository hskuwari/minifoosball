#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "Serial.h"
#include "timer.h"

#define PF0 (*((volatile uint32_t *)0x40025004))
#define PF4 (*((volatile uint32_t *)0x40025040))

#define PLAYER 0

int Done0A = 0;
int Period0A = 0;
int First0A = 0;
int Done1A = 1;
int Period1A = 0;
int First1A = 0;
int Done2A = 0;
bool pulse = 0;
int counter4dist = 0;
int delay4dist = 0;
int delay4servo = 0;

// button debounce vars
int pf0pressedC = 0; // counter for PE2 if pressed
int pf0releasedC = 0; // counter for PE2 if released
bool pf0pressed = false;
int pf4pressedC = 0; // counter for PE2 if pressed
int pf4releasedC = 0; // counter for PE2 if released
bool pf4pressed = false;
int pb2pressedC = 0; // counter for PE2 if pressed
int pb2releasedC = 0; // counter for PE2 if released
bool pb2pressed = false;

int delay4motor = 0;

// PE1 is attacker (button A?)
// PE2 is defender (button B?)
int whichMotor = 2;
int time = 150;
int setPE = 0;
int timeC = 0;

// period is (1*10^-6)/(12.5*10^-9) = 80
void Timer2A_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x05;
    TIMER2_CTL_R = 0;
    TIMER2_CFG_R = 0;
    TIMER2_TAMR_R = 2;
    TIMER2_TAILR_R = 799; // period - 1
    TIMER2_TAPR_R = 0;
    TIMER2_ICR_R = 1;
    TIMER2_IMR_R = 1;
    NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x80000000;
    NVIC_EN0_R |= 1 << 23;
    TIMER2_CTL_R = 1;
}

void buttonCase(int msg)
{
    int player = (msg >> 15) & (0x1);
    int nplayer = (msg >> 14) & (0x1);
    int button = (msg >> 12) & (0x3);
    int nbutton = (msg >> 10) & (0x3);
    int joystick = (msg >> 5) & (0x1F);
    int njoystick = msg & (0x1F);
    bool checkPlayer = ((~player) & 0x1) == nplayer;
    bool checkButton = ((~button) & 0x3) == nbutton;
    bool checkJoystick = ((~joystick) & 0x1F) == njoystick;
    if (checkPlayer && checkButton && checkJoystick)
    {
        if (player == PLAYER)
        {
            if (button == 0)
            {
                time = joystick*10;
                if (time <= 150) time = 70;
                else time = 210;
                setPE = 0;
                timeC = 0;
                SerialWrite("time: ");
                SerialWriteInt(time);
            }
            else if (button < 3)
            {
                whichMotor = button;
            }
        }
    }
}

void Timer2A_Handler(void)
{
    TIMER2_ICR_R = 1;
    counter4dist++;
    delay4dist++;
    delay4servo++;
    timeC++;
    delay4motor++;
    Done2A = 1;
}

void Timer1A_Init(void)
{
//    SYSCTL_RCGCTIMER_R |= 0x02;
//    TIMER1_CTL_R &= ~0x00000001; // Disable timer 0 for configuration � more on this in a bit
//    TIMER1_CFG_R = 0x00000004; // Configure for 16-bit capture mode
//    TIMER1_TAMR_R = 0x00000007;
//    TIMER1_CTL_R |= 0xC; // configure for rising edge  more on this in a bit
//    TIMER1_TAILR_R = 0x0000FFFF; // Start value for count down
//    TIMER1_TAPR_R = 0xFF; // Activate pre-scale
//    TIMER1_IMR_R |= 0x00000004; // Enable Input capture interrupt
//    TIMER1_ICR_R = 0x00000004; // Clear Timer0A capture match flag
//    TIMER1_CTL_R |= 0x00000001; // Timer 0A 24-bit, rising edge
//    NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x00004000;
//    NVIC_EN0_R |= 1 << 21;
//    SYSCTL_RCGCGPIO_R |= 0x02;
//    GPIO_PORTB_LOCK_R = 0x4C4F434B;
//    GPIO_PORTB_CR_R = 0xFF;
//    GPIO_PORTB_AMSEL_R &= ~0x10;
//    //GPIO_PORTB_PUR_R &= ~0x10;
//    GPIO_PORTB_DIR_R |= 0x10; // Make PB6 an input
//    GPIO_PORTB_AFSEL_R &= ~0x10; // Alternate Functionality Select for PB6
//    GPIO_PORTB_DEN_R |= 0x10; // Enable Digital Functionality
//    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) + 0x00070000;
}
void Timer1A_Handler(void)
{
//    TIMER1_ICR_R = 4; // acknowledge the interrupt
//    // calculate the period
//    Period1A = (First1A - TIMER1_TAR_R) & 0x00FFFFFF;
//
//    // remember the timer for next time
//    First1A = TIMER1_TAR_R;
//    // set flag to say that period has a new value
//    if (pulse)
//    {
//        GPIO_PORTB_DIR_R |= 0x10;
//        GPIO_PORTB_AFSEL_R &= ~0x10;
//        Done1A = 1;
//    }
////    SerialWriteInt(Period1A);
//    pulse = !pulse;
}

void Timer0A_Init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01;

    TIMER0_CTL_R &= ~0x00000001; // Disable timer 0 for configuration � more on this in a bit
    TIMER0_CFG_R = 0x00000004; // Configure for 16-bit capture mode
    TIMER0_TAMR_R = 0x00000007;
    TIMER0_CTL_R &= ~0xC; // configure for rising edge � more on this in a bit
    TIMER0_CTL_R += 0xC;
    TIMER0_TAILR_R = 0x0000FFFF; // Start value for count down
    TIMER0_TAPR_R = 0xFF; // Activate pre-scale
    TIMER0_IMR_R |= 0x00000004; // Enable Input capture interrupt
    TIMER0_ICR_R = 0x00000004; // Clear Timer0A capture match flag
    TIMER0_CTL_R |= 0x00000001; // Timer 0A 24-bit, rising edge
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x40000000;
    NVIC_EN0_R = 1 << 19;
    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_DIR_R &= ~0x40; // Make PB6 an input
    GPIO_PORTB_AFSEL_R |= 0x40; // Alternate Functionality Select for PB6
    GPIO_PORTB_DEN_R |= 0x40; // Enable Digital Functionality
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xF0FFFFFF) + 0x07000000;
}

void Timer0A_Handler(void)
{
    TIMER0_ICR_R = 0x00000004; // acknowledge the interrupt
    // calculate the period
    Period0A = (First0A - TIMER0_TAR_R) & 0x00FFFFFF;
    // remember the timer for next time
    First0A = TIMER0_TAR_R;
    // set flag to say that period has a new value
    Done0A = 1;
}

void PortE_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x10;
    GPIO_PORTE_LOCK_R = 0x4C4F434B;
    GPIO_PORTE_CR_R = 0xFF; // unlock the pin for bit 1 and 2
    GPIO_PORTE_AMSEL_R = 0x00; // disable analog functionality
    GPIO_PORTE_AFSEL_R = 0x00; // disable alternate functionality
    GPIO_PORTE_PCTL_R = 0x00000000; // select GPIO mode in PCTL
    GPIO_PORTE_PUR_R = 0; // enable/disable pullup registers
    GPIO_PORTE_DEN_R = 0xFF; // enable digital ports
    GPIO_PORTE_DIR_R = 0xFF; // Port E1 is input and E2 is output
}

void PortF_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x11; // unlock the pin for bit 0 and 4
    GPIO_PORTF_AMSEL_R = 0x00; // disable analog functionality
    GPIO_PORTF_AFSEL_R = 0x00; // disable alternate functionality
    GPIO_PORTF_PCTL_R = 0x00000000; // select GPIO mode in PCTL
    GPIO_PORTF_PUR_R = 0x11; // enable/disable pullup registers
    GPIO_PORTF_DEN_R = 0xFF; // enable digital ports
    GPIO_PORTF_DIR_R = 0xEE; // Port F0 and F4 are inputs
}

void PortB_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x2;
    GPIO_PORTB_LOCK_R = 0x4C4F434B;
    GPIO_PORTB_CR_R |= 0x4; // unlock the pin for bit 1 and 2
    GPIO_PORTB_AMSEL_R &= ~0x4; // disable analog functionality
    GPIO_PORTB_AFSEL_R &= ~0x4; // disable alternate functionality
    GPIO_PORTB_PCTL_R &= ~0x4; // select GPIO mode in PCTL
//    GPIO_PORTB_PUR_R |= 0x4; // enable/disable pullup registers
    GPIO_PORTB_DEN_R |= 0x4; // enable digital ports
    GPIO_PORTB_DIR_R &= ~0x4; // Port E1 is input and E2 is output
}

void PLLInit()
{
    SYSCTL_RCC2_R |= 0x80000000;
    SYSCTL_RCC2_R |= 0x00000800;
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) + 0x00000540;
    SYSCTL_RCC2_R &= ~0x00000070;
    SYSCTL_RCC2_R &= ~0x00002000;
    SYSCTL_RCC2_R |= 0x40000000;
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (4 << 22);
    while ((SYSCTL_RIS_R & 0x00000040) == 0)
    {
    };
    SYSCTL_RCC2_R &= ~0x00000800;
}

/**
 * main.c
 */
int main(void)
{
    PLLInit();
    SetupSerial();
    PortF_Init();
    PortE_Init();
    PortB_Init();
    Timer2A_Init();
    Timer0A_Init();
    Timer1A_Init();
    Done1A = 1;
    int set2one = 0;
    int distance;
    bool done = Done0A;
    bool setDistance = 0;
    int prevDistance = 0;
    int prevDistanceC = 0;
    int score = 2;
    bool initialize = 0;
    int i = 0;
    int shift = 0;
    int temp;
    int bit;
    bool scoreChange = 1;
    // if 0 then header was not sent
    // if 1 then 9000microsec LOW was sent
    // if 2 then 4500microsec HI was sent
    // if 3 then done with msg
    // if 4 then error then reset
    int header = 0;
    int bitIR = 15;
    bool bitlow = 0;
    int msg = 0;
    int x = 0;
    while (1)
    {
//        if (Done2A) {
        temp = score;
        while (i < 2)
        {
            if (whichMotor == 1 && i == 0)
            {
                bit = 1;
            }
            else if (whichMotor == 2 && i == 1)
            {
                bit = 1;
            }
            else
            {
                bit = 0;
            }
            if (shift == 0)
            {
                if (bit)
                {
                    GPIO_PORTE_DATA_R |= 0x8;
                }
                else
                {
                    GPIO_PORTE_DATA_R &= ~0x8;
                }
                GPIO_PORTE_DATA_R |= 0x20;
                Done2A = 0;
                shift = 1;
            }
            else if (shift == 1 && Done2A)
            {
                GPIO_PORTE_DATA_R &= ~0x20;
                shift = 2;
                Done2A = 0;
            }
            else if (shift == 2 && Done2A)
            {
                shift = 0;
                Done2A = 0;
                i++;
                temp--;
            }
        }
        while (i < 12)
        {
            if (temp > 0)
            {
                bit = 1;
            }
            else
            {
                bit = 0;
            }
            if (shift == 0)
            {
                if (bit)
                {
                    GPIO_PORTE_DATA_R |= 0x8;
                }
                else
                {
                    GPIO_PORTE_DATA_R &= ~0x8;
                }
                GPIO_PORTE_DATA_R |= 0x20;
                Done2A = 0;
                shift = 1;
            }
            else if (shift == 1 && Done2A)
            {
                GPIO_PORTE_DATA_R &= ~0x20;
                shift = 2;
                Done2A = 0;
            }
            else if (shift == 2 && Done2A)
            {
                shift = 0;
                Done2A = 0;
                i++;
                temp--;
            }
        }
        if (shift == 0 && i == 12)
        {
            GPIO_PORTE_DATA_R |= 0x10;
            Done2A = 0;
            shift = 1;
        }
        else if (shift == 1 && i == 12 && Done2A)
        {
            GPIO_PORTE_DATA_R &= ~0x10;
            shift = 0;
            i = 0;
            scoreChange = 0;
        }

        distance = Period1A / 340 / 20;
        done = Done1A;
        if (delay4dist >= 20000)
        {
//            if (done)
//            {
//                if (set2one == 0)
//                {
//                    GPIO_PORTB_DATA_R |= 0x10;
//                    set2one = 1;
//                    counter4dist = 0;
//                }
//                else if (set2one == 1 && counter4dist >= 11)
//                {
//                    GPIO_PORTB_DATA_R &= ~0x10;
//                    set2one = 2;
//                    counter4dist = 0;
//                }
//                else if (set2one == 2 && counter4dist >= 11)
//                {
//                    GPIO_PORTB_DIR_R &= ~0x10;
//                    GPIO_PORTB_AFSEL_R |= 0x10;
//                    set2one = 0;
//                    Done1A = 0;
//                    delay4dist = 0;
//                }
//
//            }
//            SerialWriteInt(distance);
//            setDistance = 1;
        }

        if (setDistance)
        {
//            setDistance = 0;
//            if (distance <= 3)
//            {
//                prevDistance = distance;
//                prevDistanceC++;
//            }
//            else if (prevDistanceC >= 2 && distance > 3)
//            {
//                score++;
//                scoreChange = 1;
//                prevDistanceC = 0;
//                distance = 400;
////                SerialWriteLine("score!");
//            }

        }

        if (delay4motor >= 10)
        {
            if (PF4 == 0)
            { // PF4
                pf4pressedC++;
                if (PF4 == 0 && pf4pressedC >= 20)
                {
                    pf4pressed = true;
                    pf4pressedC = 0;
                }
            }
            else
            { // !PF4
                if (pf4pressed)
                {
                    pf4releasedC++;
                    if (pf4releasedC >= 20)
                    {
                        SerialWriteLine("pf4");
                        pf4pressed = false;
                        pf4releasedC = 0;
                        setPE = 0;
                        if (time < 24)
                        {
                            time++;
                        }
                    }
                }
            }

            if (PF0 == 0)
            { // PF0
                pf0pressedC++;
                if (PF0 == 0 && pf0pressedC >= 20)
                {
                    pf0pressed = true;
                    pf0pressedC = 0;
                }
            }
            else
            { // !PF0
                if (pf0pressed)
                {
                    pf0releasedC++;
                    if (pf0releasedC >= 200)
                    {
                        SerialWriteLine("pf0");
                        pf0pressed = false;
                        pf0releasedC = 0;
                        setPE = 0;
                        if (time > 6)
                        {
                            time--;
                        }
                    }
                }
            }

            if ((GPIO_PORTB_DATA_R >> 2) & 1 == 1)
            { // PF0
                pb2pressedC++;
                if ((GPIO_PORTB_DATA_R >> 2) & 1 == 1 && pb2pressedC >= 20)
                {
                    pb2pressed = true;
                    pb2pressedC = 0;
                }
            }
            else
            { // !PF0
                if (pb2pressed)
                {
                    pb2releasedC++;
                    if (pb2releasedC >= 20)
                    {
                        SerialWriteLine("pe2");
                        pb2pressed = false;
                        pb2releasedC = 0;
                        score++;
                        if (score > 12) {
                            score = 2;
                        }
                    }
                }
            }
            delay4motor = 0;
        }

        if (delay4servo >= 2000)
        {
            GPIO_PORTE_DATA_R |= 1 << whichMotor;
            timeC = 0;
            setPE = 1;
            delay4servo = 0;
        }
        else if (setPE == 1 && timeC >= time)
        {
            setPE = 2;
            GPIO_PORTE_DATA_R &= ~(1 << whichMotor);
            timeC = 0;
        }

        if (Done0A)
        {
            Done0A = 0;
            if (header == 0)
            {
                if (Period0A >= 576000 && Period0A <= 864000)
                {
                    header++;
                }
                else
                {
                    header = 4;
                }
            }

            else if (header == 1)
            {

                if (Period0A >= 288000 && Period0A <= 432000)
                {
                    header++;
                }
                else
                {
                    header = 4;
                }
            }

            else if (header == 2)
            {

                if (Period0A >= 35840 && Period0A <= 53760 && !bitlow)
                {
                    // recieved first 560microsec lows
                    bitlow = 1;
                }

                else if (Period0A >= 108160 && Period0A <= 162240 && bitlow)
                {
                    // bit one
                    bitlow = 0;
                    msg |= 1 << bitIR;
//                    SerialWrite("bit: ");
//                    SerialWriteInt(bitIR);
                    bitIR--;
                    if (bitIR < 0)
                    {
                        header = 3;
                    }
                }
                else if (Period0A >= 35840 && Period0A <= 53760 && bitlow)
                {
                    // bit zero
                    bitlow = 0;
//                    SerialWrite("bit: ");
//                    SerialWriteInt(bitIR);
                    bitIR--;
                    if (bitIR < 0)
                    {
                        header = 3;
                    }
                }
                else
                {
                    header = 4;
                }
            }
            else if (header == 3)
            {
                buttonCase(msg);
                header = 0;
                bitlow = 0;
                msg = 0;
                bitIR = 15;
            }

            if (header == 4)
            {
                header = 0;
                bitlow = 0;
                msg = 0;
                bitIR = 15;
            }
        }
//        Done2A = 0;
//        }

    }

}
