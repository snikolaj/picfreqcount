# picfreqcount
A frequency counter using a PIC12F1501, an SN74HC595 shift register and a 16x2 LCD. Works using the 16 bit counter on the PIC12F1501. Uses up all the pins of the tiny microcontroller. Has an inbuilt Schmitt trigger. Since it uses the internal oscillator, the accuracy is off by 2% in preliminary testing. XC8 leaves only 22 bytes left for extras. Can be improved.
