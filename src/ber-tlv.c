#include <stdio.h>
#include <stdint.h>
#include "ber-tlv.h"

void print_tlv(uint8_t *tlv, size_t len, int indent) {
    
    if (len == 0) return;

    // Read TAG and LEN
    uint8_t tag = tlv[0];
    size_t value_length = tlv[1];
    
    // Calculate depth for indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    // Print TAG and LEN headers
    printf("TAG – 0x%02X (%s class, %s)\n", tag,
        (tag & 0xC0) == 0xC0 ? "private" : "public",
        (tag & 0x20) ? "constructed" : "primitive");
    
    printf("LEN – %zu bytes\n", value_length);

    // If has value, output it
    if (value_length > 0) {

        for (int i = 0; i < indent + 1; i++) {
            printf("  ");
        }

        printf("VAL –");
        
        for (size_t i = 2; i < 2 + value_length; i++) {
            printf(" 0x%02X", tlv[i]);
        }
        
        printf("\n");
    }

    // Loop throught the next TLV
    size_t total_length = 2 + value_length;

    if (total_length < len) {
        print_tlv(tlv + total_length, len - total_length, indent);
    }
}

void interpret_tlv(uint8_t *tlvObject, size_t objLen) {
    print_tlv(tlvObject, objLen, 0);
}