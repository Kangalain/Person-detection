#include "Adafruit_AMG88xx.h"
#include <msp430.h>

//#define I2C_DEBUG
// Work with MSP430FR5994

/**************************************************************************/
/*!
    @brief  Setups the I2C interface and hardware
    @param  addr Optional I2C address the sensor can be found on. Default is
   0x69
    @returns True if device is set up, false on any failure
*/
/**************************************************************************/
bool Adafruit_AMG88xx::begin(uint8_t addr) {
  _i2caddr = addr;

  _i2c_init();

  // enter normal mode
  _pctl.PCTL = AMG88xx_NORMAL_MODE;
  write8(AMG88xx_PCTL, _pctl.get());

  // software reset
  _rst.RST = AMG88xx_INITIAL_RESET;
  write8(AMG88xx_RST, _rst.get());

  // disable interrupts by default
  disableInterrupt();

  // set to 10 FPS
  _fpsc.FPS = AMG88xx_FPS_10;
  write8(AMG88xx_FPSC, _fpsc.get());

  __delay_cycles(100);

  return true;
}

/**************************************************************************/
/*!
    @brief  Set the moving average mode.
    @param  mode if True is passed, output will be twice the moving average
*/
/**************************************************************************/
void Adafruit_AMG88xx::setMovingAverageMode(bool mode) {
  _ave.MAMOD = mode;
  write8(AMG88xx_AVE, _ave.get());
}

/**************************************************************************/
/*!
    @brief  Set the interrupt levels. The hysteresis value defaults to .95 *
   high
    @param  high the value above which an interrupt will be triggered
    @param  low the value below which an interrupt will be triggered
*/
/**************************************************************************/
void Adafruit_AMG88xx::setInterruptLevels(float high, float low) {
  setInterruptLevels(high, low, high * .95);
}

/**************************************************************************/
/*!
    @brief  Set the interrupt levels
    @param  high the value above which an interrupt will be triggered
    @param  low the value below which an interrupt will be triggered
    @param hysteresis the hysteresis value for interrupt detection
*/
/**************************************************************************/
void Adafruit_AMG88xx::setInterruptLevels(float high, float low,
                                          float hysteresis) {
  /*
    int highConv = high / AMG88xx_PIXEL_TEMP_CONVERSION;
  highConv = constrain(highConv, -4095, 4095);
  _inthl.INT_LVL_H = highConv & 0xFF;
  _inthh.INT_LVL_H = (highConv & 0xF) >> 4;
  this->write8(AMG88xx_INTHL, _inthl.get());
  this->write8(AMG88xx_INTHH, _inthh.get());

  int lowConv = low / AMG88xx_PIXEL_TEMP_CONVERSION;
  lowConv = constrain(lowConv, -4095, 4095);
  _intll.INT_LVL_L = lowConv & 0xFF;
  _intlh.INT_LVL_L = (lowConv & 0xF) >> 4;
  this->write8(AMG88xx_INTLL, _intll.get());
  this->write8(AMG88xx_INTLH, _intlh.get());

  int hysConv = hysteresis / AMG88xx_PIXEL_TEMP_CONVERSION;
  hysConv = constrain(hysConv, -4095, 4095);
  _ihysl.INT_HYS = hysConv & 0xFF;
  _ihysh.INT_HYS = (hysConv & 0xF) >> 4;
  this->write8(AMG88xx_IHYSL, _ihysl.get());
  this->write8(AMG88xx_IHYSH, _ihysh.get());
  */
}

/**************************************************************************/
/*!
    @brief  enable the interrupt pin on the device.
*/
/**************************************************************************/
void Adafruit_AMG88xx::enableInterrupt() {
  _intc.INTEN = 1;
  this->write8(AMG88xx_INTC, _intc.get());
}

/**************************************************************************/
/*!
    @brief  disable the interrupt pin on the device
*/
/**************************************************************************/
void Adafruit_AMG88xx::disableInterrupt() {
  _intc.INTEN = 0;
  this->write8(AMG88xx_INTC, _intc.get());
}

/**************************************************************************/
/*!
    @brief  Set the interrupt to either absolute value or difference mode
    @param  mode passing AMG88xx_DIFFERENCE sets the device to difference mode,
   AMG88xx_ABSOLUTE_VALUE sets to absolute value mode.
*/
/**************************************************************************/
void Adafruit_AMG88xx::setInterruptMode(uint8_t mode) {
  _intc.INTMOD = mode;
  this->write8(AMG88xx_INTC, _intc.get());
}

/**************************************************************************/
/*!
    @brief  Read the state of the triggered interrupts on the device. The full
   interrupt register is 8 bytes in length.
    @param  buf the pointer to where the returned data will be stored
    @param  size Optional number of bytes to read. Default is 8 bytes.
    @returns up to 8 bytes of data in buf
*/
/**************************************************************************/
void Adafruit_AMG88xx::getInterrupt(uint8_t *buf, uint8_t size) {
    /*
    uint8_t bytesToRead = min(size, (uint8_t)8);

  this->read(AMG88xx_INT_OFFSET, buf, bytesToRead);
  */
}

/**************************************************************************/
/*!
    @brief  Clear any triggered interrupts
*/
/**************************************************************************/
void Adafruit_AMG88xx::clearInterrupt() {
  _rst.RST = AMG88xx_FLAG_RESET;
  write8(AMG88xx_RST, _rst.get());
}

/**************************************************************************/
/*!
    @brief  read the onboard thermistor
    @returns a the floating point temperature in degrees Celsius
*/
/**************************************************************************/
uint16_t Adafruit_AMG88xx::readThermistor() {
  uint8_t raw[2];
  this->read(AMG88xx_TTHL, raw, 2);
  uint16_t recast = ((uint16_t)raw[1] << 8) | ((uint16_t)raw[0]);

  return recast; //signedMag12ToFloat(recast) *  ;
}

/**************************************************************************/
/*!
    @brief  Read Infrared sensor values
    @param  buf the array to place the pixels in
    @param  size Optionsl number of bytes to read (up to 64). Default is 64
   bytes.
    @return up to 64 bytes of pixel data in buf
*/
/**************************************************************************/
void Adafruit_AMG88xx::readPixels(uint16_t *buf, uint8_t size) {

  uint16_t recast;

  uint8_t rawArray[AMG88xx_PIXEL_ARRAY_SIZE << 1];
  this->read(AMG88xx_PIXEL_OFFSET, rawArray, size << 1);

  for (int i = 0; i < size; i++) {
    uint8_t pos = i << 1;
    recast = ((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos]);

    buf[i] = recast;
  }

}

/**************************************************************************/
/*!
    @brief  write one byte of data to the specified register
    @param  reg the register to write to
    @param  value the value to write
*/
/**************************************************************************/
void Adafruit_AMG88xx::write8(uint8_t reg, uint8_t value) {
  this->write(reg, &value, 1);
}

/**************************************************************************/
/*!
    @brief  read one byte of data from the specified register
    @param  reg the register to read
    @returns one byte of register data
*/
/**************************************************************************/
uint8_t Adafruit_AMG88xx::read8(uint8_t reg) {
  uint8_t ret;
  this->read(reg, &ret, 1);

  return ret;
}

void Adafruit_AMG88xx::_i2c_init() { i2c_init(); } // i2c_init() is for MSP430 initialization

void Adafruit_AMG88xx::read(uint8_t reg, uint8_t *buf, uint8_t num) {
    i2c_read(reg, buf, num, AMG88xx_ADDRESS);
}

void Adafruit_AMG88xx::write(uint8_t reg, uint8_t *buf, uint8_t num) {
    i2c_write(reg, buf, num, AMG88xx_ADDRESS);
}

/**************************************************************************/
/*!
    @brief  convert a 12-bit signed magnitude value to a floating point number
    @param  val the 12-bit signed magnitude value to be converted
    @returns the converted floating point value
*/
/**************************************************************************/
float Adafruit_AMG88xx::signedMag12ToFloat(uint16_t val) {
  // take first 11 bits as absolute val
  uint16_t absVal = (val & 0x7FF);

  return (val & 0x800) ? 0 - (float)absVal : (float)absVal;
}

/**************************************************************************/
/*!
    @brief  convert a 12-bit integer two's complement value to a floating point
   number
    @param  val the 12-bit integer  two's complement value to be converted
    @returns the converted floating point value
*/
/**************************************************************************/
float Adafruit_AMG88xx::int12ToFloat(uint16_t val) {
  int16_t sVal =
      (val << 4); // shift to left so that sign bit of 12 bit integer number is
                  // placed on sign bit of 16 bit signed integer number
  return sVal >> 4; // shift back the signed number, return converts to float
}
