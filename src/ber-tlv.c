#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ber-tlv.h"

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }
}

// TLV parsing
TLV parse_tlv(uint8_t *tlv, size_t len)
{
    TLV tlv_obj = {0};

    // TAG
    tlv_obj.tag = tlv[0];
    size_t tag_len = 1;

    // Multi-byte TAG
    if ((tlv_obj.tag & 0x1F) == 0x1F)
    {
        tlv_obj.tag = 0;
        while (tlv[tag_len] & 0x80)
        {
            tlv_obj.tag = (tlv_obj.tag << 7) | (tlv[tag_len] & 0x7F);
            tag_len++;
        }
        tlv_obj.tag = (tlv_obj.tag << 7) | (tlv[tag_len] & 0x7F);
        tag_len++;
    }

    // LEN
    size_t length_len = 1;
    if (tlv[tag_len] < 0x80)
    {
        tlv_obj.length = tlv[tag_len];
    }
    else
    {
        int num_bytes = tlv[tag_len] & 0x7F;
        tlv_obj.length = 0;
        for (int i = 1; i <= num_bytes; i++)
        {
            tlv_obj.length = (tlv_obj.length << 8) | tlv[tag_len + i];
        }
        length_len = 1 + num_bytes;
    }

    // constructed or primitive?
    tlv_obj.is_constructed = (tlv[0] & 0x20) ? 1 : 0;

    // Values
    tlv_obj.value = (tlv + tag_len + length_len);

    return tlv_obj;
}

// Output formatted TLV
void print_tlv(TLV *tlv, int indent)
{
    print_indent(indent);
    printf("TAG – 0x%02X (%s class, %s)\n", tlv->tag,
           (tlv->tag & 0xC0) == 0xC0 ? "private" : "public",
           tlv->is_constructed ? "constructed" : "primitive");

    print_indent(indent);
    printf("LEN – %zu bytes\n", tlv->length);

    // If LEN = 0 bytes or constructed, jump line
    if (tlv->is_constructed || tlv->length == 0)
    {
        printf("\n");
    }

    // process child objects if constructed
    if (tlv->is_constructed)
    {
        size_t offset = 0;
        while (offset < tlv->length)
        {
            TLV child_tlv = parse_tlv(tlv->value + offset, tlv->length - offset);
            print_tlv(&child_tlv, indent + 1);
            offset += 2 + child_tlv.length; // tag + length
        }
    }
    else if (tlv->length > 0)
    {
        // Print primitive TLV
        print_indent(indent);
        printf("VAL –");
        for (size_t i = 0; i < tlv->length; i++)
        {
            printf(" 0x%02X", tlv->value[i]);
        }
        printf("\n\n");
    }
}

// Entry point
void interpret_tlv(uint8_t *tlvObject, size_t objLen)
{
    TLV root_tlv = parse_tlv(tlvObject, objLen);
    print_tlv(&root_tlv, 0);
}
