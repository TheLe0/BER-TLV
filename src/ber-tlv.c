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

size_t print_tlv(uint8_t *tlv, size_t len, int indent, int is_first)
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

    // If LEN is header or with 0 length, jump line
    if (is_first || length == 0)
    {
        printf("\n");
    }

    size_t total_length = tag_len + len_len + length;

    // Process constructed objects (skip value print)
    if (tag & 0x20)
    {
        size_t processed_len = tag_len + len_len;
        while (processed_len < total_length)
        {
            processed_len += print_tlv(tlv + processed_len, len - processed_len, indent + 1, 0);
        }
    }
    else if (length > 0)
    {
        // Print value for primitive tags

        print_indent(indent);
        printf("VAL –");

        for (size_t i = 0; i < length; i++)
        {
            printf(" 0x%02X", tlv[tag_len + len_len + i]);
        }
        printf("\n");

        // End of section
        printf("\n");
    }

    return total_length;
}

void interpret_tlv(uint8_t *tlvObject, size_t objLen)
{
    // Pass 1 to indicate that the first TLV is being processed
    print_tlv(tlvObject, objLen, 0, 1);
}
