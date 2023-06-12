#include <pigpio.h>
#include <unistd.h>
#include <iostream>

#define LEDPIN 8

int main(int argc, char *argv[])
{
   if (gpioInitialise() >= 0)
   {
      /* Can't instantiate a Hasher before pigpio is initialised. */

      /* 
         This assumes the output pin of an IR receiver is
         connected to gpio 7.
      */

      gpioSetMode(LEDPIN, PI_OUTPUT);
      gpioWrite(LEDPIN, 0);
   }
}

