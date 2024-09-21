#include <stdio.h>
#include <stdint.h>
#include "ber-tlv.h"

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }
}

size_t parse_tag(uint8_t *tlv, size_t len, uint32_t *tag)
{
    *tag = tlv[0];
    size_t tag_len = 1;

    // Multi-byte tag
    if ((*tag & 0x1F) == 0x1F)
    {
        *tag = 0;

        // Keep processing while MSB is set
        while (tlv[tag_len] & 0x80)
        {
            *tag = (*tag << 7) | (tlv[tag_len] & 0x7F);
            tag_len++;
        }
        *tag = (*tag << 7) | (tlv[tag_len] & 0x7F);
        tag_len++;
    }
    return tag_len;
}

size_t parse_length(uint8_t *tlv, size_t len, size_t *length)
{
    if (tlv[0] < 0x80)
    {
        *length = tlv[0];
        return 1;
    }
    else
    {
        int num_bytes = tlv[0] & 0x7F;
        *length = 0;
        for (int i = 1; i <= num_bytes; i++)
        {
            *length = (*length << 8) | tlv[i];
        }
        return 1 + num_bytes;
    }
}

size_t print_tlv(uint8_t *tlv, size_t len, int indent)
{
    if (len == 0)
        return 0;

    uint32_t tag;
    size_t tag_len = parse_tag(tlv, len, &tag);

    size_t length;
    size_t len_len = parse_length(tlv + tag_len, len - tag_len, &length);

    print_indent(indent);
    printf("TAG – 0x%02X (%s class, %s)\n", tag,
           (tag & 0xC0) == 0xC0 ? "private" : "public",
           (tag & 0x20) ? "constructed" : "primitive");

    print_indent(indent);
    printf("LEN – %zu bytes\n", length);

    if (length > 0)
    {
        print_indent(indent + 1);
        printf("VAL –");

        for (size_t i = 0; i < length; i++)
        {
            printf(" 0x%02X", tlv[tag_len + len_len + i]);
        }

        printf("\n");
    }

    size_t total_length = tag_len + len_len + length;

    // Process constructed objects recursively
    if (tag & 0x20)
    {
        // Constructed tag
        size_t processed_len = tag_len + len_len;
        while (processed_len < total_length)
        {
            processed_len += print_tlv(tlv + processed_len, len - processed_len, indent + 1);
        }
    }

    return total_length;
}

void interpret_tlv(uint8_t *tlvObject, size_t objLen)
{
    print_tlv(tlvObject, objLen, 0);
}
