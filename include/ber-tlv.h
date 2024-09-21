#ifndef BER_TLV_H
#define BER_TLV_H

#include <stddef.h>
#include <stdint.h>

// TLV schema
typedef struct
{
    uint32_t tag;       // TAG
    size_t length;      // LEN
    uint8_t *value;     // VAL
    int is_constructed; // constructed or primitive?
} TLV;

// Functions
void interpret_tlv(uint8_t *tlvObject, size_t objLen);
TLV parse_tlv(uint8_t *tlv, size_t len);
void print_tlv(TLV *tlv, int indent);

#endif
