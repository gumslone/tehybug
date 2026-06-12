#ifndef _MODBUS_H
    #define _MODBUS_H

    /* The function returns the CRC as a unsigned short type
        puchMsg  -> message to calculate CRC upon
        usDataLen -> quantity of bytes in message */
    uint16_t modbus_CRC16 (uint8_t *puchMsg, uint16_t usDataLen );
#endif
