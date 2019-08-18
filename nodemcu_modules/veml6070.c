/*
 app/modules/veml6070.c
 
 Copyright (c) 2019 Oleg Jusaew
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "module.h"
#include "lauxlib.h"
#include "platform.h"
#include "c_math.h"

static const uint32_t veml6070_i2c_id = 0;
static const uint8_t veml6070_i2c_addr = 0x38;

static int veml6070_write(int data)
{
    platform_i2c_send_start(0);
    platform_i2c_send_address(0, veml6070_i2c_addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
    platform_i2c_send_byte(0, data);
    os_delay_us(30);
    platform_i2c_send_stop(0);
    return 0;
}

static int veml6070_setup(lua_State* L)
{
    // step 1: Wake sensor
    unsigned itime = luaL_checkinteger( L, 1 );
    veml6070_write(itime);
    return 0;
}

static int veml6070_read(lua_State* L)
{
    int delay = luaL_checkinteger( L, 1 );
    os_delay_us(delay);
    //read data, msb
    platform_i2c_send_start(0);
    platform_i2c_send_address(0, veml6070_i2c_addr + 1, PLATFORM_I2C_DIRECTION_RECEIVER);
    os_delay_us(30);
    uint16_t uvi = platform_i2c_recv_byte(0,1);
    platform_i2c_send_stop(0);
    
    uvi <<= 8;

    //read data, lsb
    platform_i2c_send_start(0);
    platform_i2c_send_address(0, veml6070_i2c_addr, PLATFORM_I2C_DIRECTION_RECEIVER);
    os_delay_us(30);
    uvi |= platform_i2c_recv_byte(0,1);
    platform_i2c_send_stop(0);
    
    lua_pushnumber(L, uvi);
    return 1;
}

static int veml6070_sleep(lua_State* L)
{
    // Put sensor into a sleep mode
    veml6070_write(0x03);
    return 0;
}

static const LUA_REG_TYPE veml6070_map[] = {
    { LSTRKEY( "read" ),  LFUNCVAL( veml6070_read )},
    { LSTRKEY( "setup" ), LFUNCVAL( veml6070_setup )},
    { LSTRKEY( "sleep" ), LFUNCVAL( veml6070_sleep )},
    { LNILKEY, LNILVAL}
};

NODEMCU_MODULE(VEML6070, "veml6070", veml6070_map, NULL);
