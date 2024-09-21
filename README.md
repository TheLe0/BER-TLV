# BER-TLV

BER is Basic Encoding Rules. TLV stands for Tag + Length + Value. Tag is 1-4 byte long (integer) identifier. Length is 1-4 byte integer representing value (or entire branch) size. Value is binary data, which can also be a nested list of sub-items. BER-TLV is a binary data storage tree, which is widely used in electonic payment industry and in EMV in particular.

This project is a shared library that you can use in your projects, we receive a TLV object representation and interpret the value.

For example:

```c
size_t objLen = 13;
uint8_t tlvObject[objLen] = {
  0xE1, 0x0B, 0xC1, 0x03, 0x01, 0x02,
  0x03, 0xC2, 0x00, 0xC3, 0x02, 0xAA,
  0xBB
};

interpret_tlv(tlvObject, objLen);
```

is going to output this:

```bash
TAG – 0xE1 (private class, constructed)
LEN – 11 bytes

  TAG – 0xC1 (private class, primitive)
  LEN – 3 bytes
  VAL – 0x01 0x02 0x03

  TAG – 0xC2 (private class, primitive)
  LEN – 0 bytes

  TAG – 0xC3 (private class, primitive)
  LEN – 2 bytes
  VAL – 0xAA 0xBB
```

## Configuration

To run this on your machine you only need to have installed a C compiler (GNU, clang or msbuild).

### GNU

On the root directory:

```bash
mkdir -p ./build
cd ./build
cmake ..
make
./main
```
