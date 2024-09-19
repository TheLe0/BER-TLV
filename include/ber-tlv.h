#ifndef BER_TLV_H
#define BER_TLV_H

#include <stddef.h>
#include <stdint.h>

void interpret_tlv(uint8_t *tlvObject, size_t objLen);

#endif