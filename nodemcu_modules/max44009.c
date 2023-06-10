/*
 app/modules/max44009.c

 Copyright (c) 2019 Oleg Jusaew
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include "c_math.h"
#include "lauxlib.h"
#include "module.h"
#include "platform.h"

static const uint32_t max44009_i2c_id = 0;
static const uint8_t max44009_i2c_addr = 0x4A;
static const uint8_t max44009_i2c_cnf_reg = 0x02;

static int max44009_setup(lua_State *L) {
  // step 1: Wake sensor
  unsigned mode = luaL_checkinteger(L, 1);
  platform_i2c_send_start(0);
  platform_i2c_send_address(0, max44009_i2c_addr,
                            PLATFORM_I2C_DIRECTION_TRANSMITTER);
  platform_i2c_send_byte(0, max44009_i2c_cnf_reg);
  platform_i2c_send_byte(0, mode);
  os_delay_us(30);
  platform_i2c_send_stop(0);
  return 0;
}

static int max44009_read(lua_State *L) {
  int i;
  unsigned int lux_high[2];
  unsigned int lux_low[2];
  int delay = luaL_checkinteger(L, 1);
  os_delay_us(delay);
  // send the read command
  platform_i2c_send_start(0);
  platform_i2c_send_address(0, max44009_i2c_addr,
                            PLATFORM_I2C_DIRECTION_TRANSMITTER);
  platform_i2c_send_byte(0, 0x03);
  os_delay_us(30);
  platform_i2c_send_stop(0);
  // read data
  platform_i2c_send_start(0);
  platform_i2c_send_address(0, max44009_i2c_addr,
                            PLATFORM_I2C_DIRECTION_RECEIVER);
  os_delay_us(30);
  for (i = 0; i < 2; i++) {
    lux_high[i] = platform_i2c_recv_byte(0, 1);
  }
  platform_i2c_send_stop(0);

  // send the read command
  platform_i2c_send_start(0);
  platform_i2c_send_address(0, max44009_i2c_addr,
                            PLATFORM_I2C_DIRECTION_TRANSMITTER);
  platform_i2c_send_byte(0, 0x04);
  os_delay_us(30);
  platform_i2c_send_stop(0);
  // read data
  platform_i2c_send_start(0);
  platform_i2c_send_address(0, max44009_i2c_addr,
                            PLATFORM_I2C_DIRECTION_RECEIVER);
  os_delay_us(30);
  for (i = 0; i < 2; i++) {
    lux_low[i] = platform_i2c_recv_byte(0, 1);
  }
  platform_i2c_send_stop(0);

  // Convert the data to lux
  int exponent = (lux_high[0] & 0xF0) >> 4;
  int mantissa = ((lux_high[0] & 0x0F) << 4) | (lux_high[1] & 0x0F);
  float luminance = pow(2, exponent) * mantissa * 0.045;
  lua_pushnumber(L, luminance);

  // Convert the data to lux
  exponent = (lux_high[0] & 0xF0) >> 4;
  mantissa = ((lux_high[0] & 0x0F) << 4) | (lux_low[1] & 0x0F);
  luminance = pow(2, exponent) * mantissa * 0.045;
  lua_pushnumber(L, luminance);
  return 2;
}
static const LUA_REG_TYPE max44009_map[] = {
    {LSTRKEY("read"), LFUNCVAL(max44009_read)},
    {LSTRKEY("setup"), LFUNCVAL(max44009_setup)},
    {LNILKEY, LNILVAL}};

NODEMCU_MODULE(MAX44009, "max44009", max44009_map, NULL);
