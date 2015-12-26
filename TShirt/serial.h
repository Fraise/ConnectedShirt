/** Definitions for serial port **/

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define PRESCALER 1024
#define TIME_SLOT
#define NB_TICK 200 //20ms -> 312.5 : ~50ms

void init_serial(int speed);
void send_serial(char c);
char get_serial(void);
void init_printf(void);
void init_timer(void);

#endif
