# Test of basic comms
> code tested and works

**target board:** arduino adafruit.

**purpose:** test read/write of control registers<br>

**conclusions:** Data lines seem to have a lot of noise, typtically takes 5-20 read attempts per sucessful read (tested in the archive), unknown attempts on write as we havent found a way to test all registers without just reading them. suspect noise is caused by bad connections (seried jumper wires with pin&slot connectors).

### note
- can potentially damage connected things! a itterating counter is being written to the control registers, this WILL cause it to do things like dicharge the cells.
- tested using the suggested circuit in the devboards datasheet. 4 resistors in series with single a psu. tested over 10-20v, .5Amax.
- control registers are both for read and write. 'registers' can read differently than what they are set to. (e.g: consider the watch dog timer register, writing turns it on/off but reading returns if it has timed out, not indicative of it being on/off).
