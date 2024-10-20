# Temperature Sense board. e26

**target board:** arduino adafruit.

**purpose:** interface the LTC68031 with the Orian BMS, and to log cell data.<br>

**context:** this code allows for the Adafruit to act as a intermidetary between the Orian BMS and the LTC ic. this is needed to act as a custom voltage monitoring interface for the BMS. In our case the LTC is used to measure cell voltages (SPI), that data must be forwarded to the BMS (CAN).
there is also some extra stuff to handle features of BMS and data integerity. (light data analysis, error correction, etc).

---

### How it *supposed* to work
Adafruit periodically polls the LTC and forwards results to BMS, a copy of the data is logged internally to be recovered after each drive (rf solution?).

---

### Helpful resources

pic of a similar Adafruit(a arduino derivative) as the one being used. Choosen because it has integrated support for CAN.
<img src="https://cdn-shop.adafruit.com/970x728/5724-00.jpg" alt="epic image of the adafruit board (may not be the exact one but looks close enough)">
    <a href="https://www.adafruit.com/product/5724" alt="adafruit has falled"></a>
</img>

- [LTC68031 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/680313fa.pdf)
- idk where the Orian BMS datasheet that goes indepth about comms. theres a table goes over some command codes posted in the discord - named "thermistor_module_canbos(8).pdf" ( "*(8)*" yikes ).
- [Arduino Adafruit CAN setup & examples](https://learn.adafruit.com/adafruit-feather-m4-can-express/arduino-can-examples)

---

### clarifications on how the LTC6803 works

- the LTC is a non programmable IC. there is firmware(/hardware?) on it from the factory.
- control is done entirely over SPI.
- a mcu generated PEC is sent with every communication(see datasheet for details), if the LTC cannot confirm the PEC, the communicaiton is ignored. There is no direct indicator if it was rejected or not.
- if using interrupts, must know # of cells before hand (ltc.14 "unused input channels must be masked"). we dont care since were not using interrupts since were 
spam polling it anyways.

---

### things to consider

- DO pin requires a 5k pull up resisitor to Vdd(3v-5v) of the SPI controller (dev.10).
- CS/SS is NOT OPTIONAL, there is so much noise on the line.
- SCL max 1MHz (special consideration to hardware design is needed otherwise. see LTC datasheet). 
- careful with i/o assumptions! **most** pins operate by pulling low only, external pull ups with external voltage sources are needed for those pins.