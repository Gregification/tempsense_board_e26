# Temp sense code
> this code is untested!

**target board:** arduino adafruit.

**purpose:** interface the LTC68031 with the Orian BMS, and to log cell data.<br>

**context:** this code allows for the Adafruit to act as a intermidetary between the Orian and the LTC ic. this is needed to act as a custom voltage monitoring interface to the Orian. In our case the LTC is used to measure cell voltages, that must be forwarded to the BMS; problem is that the 2 interfaces - physically and digitally - are incompatiable. the arduino just bridges that.

---
### How it _supposed_ to work
Adafruit periodically polls the LTC and forwards results to BMS, a copy of the data is logged internally to be recovered after each drive. (rf solution?)

---
### Resources
<img src="https://cdn-shop.adafruit.com/970x728/5724-00.jpg" alt="epic image of the adafruit board (may not be the exact one but looks close enough)">
    <a href="https://www.adafruit.com/product/5724" alt="adafruit has falled"></a>
</img>

- [LTC68031 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/680313fa.pdf)
- i have no idea where the Orian BMS CAN datasheet is online. theres a copy in discord named "thermistor_module_canbos(8).pdf" ( "(8)" yikes ).
