/** Definitions for the analog converter **/

#ifndef __ANALOG_H__
#define __ANALOG_H__

#include <avr/io.h>
#include <stdio.h>

void ad_init(unsigned char channel);
unsigned int ad_sample(void);

#endif
