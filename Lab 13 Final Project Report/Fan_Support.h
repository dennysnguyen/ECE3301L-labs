int get_duty_cycle(int , int );
void Monitor_Fan(void);
int get_RPM();
void Toggle_Fan();
void Turn_Off_Fan();
void Turn_On_Fan();
void Increase_Duty_Cycle();
void Decrease_Duty_Cycle();
void do_update_pwm(char) ;

void Do_Change_Fan_Timer_Mode_Plus();
void Do_Change_Fan_Timer_Mode_Minus();

#define FAN_EN          PORTDbits.RD1           // <-- Need to change the assignments here
#define FANON_LED       PORTDbits.RD2           // <-- Need to change the assignments here

#define FAN_TIMER_MODE0 0
#define FAN_TIMER_MODE1 1
#define FAN_TIMER_MODE2 2
#define FAN_TIMER_MODE3 3

#define FAN_TIMER_TIME_MODE0    0
#define FAN_TIMER_TIME_MODE1    10
#define FAN_TIMER_TIME_MODE2    15
#define FAN_TIMER_TIME_MODE3    20

#define Timer0          0
#define Timer1          5
#define Timer2          10
#define Timer3          20
