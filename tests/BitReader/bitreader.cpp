// ---------------------------------------------------------------------------
//  BitReader Library - v1.1 - 03/24/2016
//  Copyright (C) 2016 Antoine Beauchamp
//  The code & updates for the library can be found on http://end2endzone.com
//
// See "BitReader.h" for license, purpose, syntax, version history, links, and more.
// ---------------------------------------------------------------------------

#include "bitreader.h"

#define MIN_BETWEEN2(a,b) (a < b ? a : b)
#define MIN_BETWEEN3(a,b,c) MIN_BETWEEN2(MIN_BETWEEN2(a,b),c)
#define MAX_BETWEEN2(a,b) (a > b ? a : b)
#define MAX_BETWEEN3(a,b,c) MAX_BETWEEN2(MAX_BETWEEN2(a,b),c)

#define BIT_SET(value, bitIdx) ((value) |= (1<<(bitIdx)))
#define BIT_CLEAR(value, bitIdx) ((value) &= ~(1<<(bitIdx)))
#define BIT_FLIP(value, bitIdx) ((value) ^= (1<<(bitIdx)))
#define BIT_CHECK1(value, bitIdx) ((value) & (1<<(bitIdx)))
#define BIT_CHECK2(value, bitIdx) ((value >> bitIdx) & (1))
#define BIT_CHECK3(value, bitIdx) ((byte & (1<<bitIdx)) >> bitIdx)
#define BIT_UPDATE1(value, bitIdx, bitBoolValue) ( (bitBoolValue) ? BIT_SET((value), (bitIdx)) : BIT_CLEAR((value), (bitIdx)) )
#define BIT_UPDATE2(value, bitIdx, bitBoolValue) BIT_CLEAR((value), (bitIdx)); (value) |= ((bitBoolValue)<<bitIdx);
#define BIT_UPDATE3(value, bitIdx, bitBoolValue) ( (value) = ((value) & ~(1<<bitIdx)) | ((bitBoolValue)<<bitIdx) )

#define BITMASK_SET(value, mask) ((value) |= (mask))
#define BITMASK_CLEAR(value, mask) ((value) &= (~(mask)))
#define BITMASK_FLIP(value, mask) ((value) ^= (mask))
#define BITMASK_CHECK(value, mask) ((value) & (mask))

#define setSingleBit(byte, bitIdx, bitValue) BIT_UPDATE2((byte), (bitIdx), (bitValue))
#define getSingleBit(byte, bitIdx) BIT_CHECK2((byte), (bitIdx))

#define TO_UINT8_PTR(expr) ((unsigned char *)expr)

BitAddress::BitAddress()
{
  reset();
}

BitAddress::BitAddress(const BitAddress & iBitAddress)
{
  byte = iBitAddress.byte;
  bit = iBitAddress.bit;
}

void BitAddress::reset()
{
  byte = 0;
  bit = 0;
};

#ifdef USE_BITADDRESS_SETTER_GETTER

void BitAddress::set(void * iBuffer, bool iBitValue)
{
  setSingleBit( TO_UINT8_PTR(iBuffer)[byte], bit, iBitValue );
}

bool BitAddress::get(const void * iBuffer)
{
  return getSingleBit( TO_UINT8_PTR(iBuffer)[byte], bit ) != 0;
}

#endif //USE_BITADDRESS_SETTER_GETTER

void BitAddress::add(unsigned int iBits)
{
  byte += (iBits / BITS_PER_BYTE);
  bit += (iBits % BITS_PER_BYTE);
  if (bit >= BITS_PER_BYTE)
  {
    byte++;
    bit -= BITS_PER_BYTE;
  }
}

void BitAddress::next()
{
  bit++;
  if (bit == BITS_PER_BYTE)
  {
    byte++;
    bit = 0;
  }
}

unsigned int BitAddress::toAbsBitOffset()
{
  return byte*BITS_PER_BYTE + bit;
}

#ifdef WIN32
std::string BitAddress::toString()
{
  static const int BUFFER_SIZE = 24;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "[%d,%d](bit %d)", byte, bit, toAbsBitOffset());
  return buffer;
}
#endif //WIN32

#ifdef USE_BITADDRESS_READ_WRITE
void BitAddress::setBuffer(const void * iBuffer)
{
  mBuffer = (const unsigned char *)iBuffer;
}

void BitAddress::read(unsigned int iNumBits, void * iTargetBits)
{
  BitAddress output;
  for(unsigned int i=0; i<iNumBits; i++)
  {
#ifdef USE_BITADDRESS_SETTER_GETTER
    output.set(iTargetBits, get(mBuffer) );
#endif
#ifdef USE_SINGLEBIT_MACROS
    setSingleBit(TO_UINT8_PTR(iTargetBits)[output.byte], output.bit, getSingleBit(mBuffer[byte], bit) );
#endif
    next();
    output.next();
  }
}

void BitAddress::write(const void * iSourceBits, unsigned int iNumBits)
{
  BitAddress input;
  for(unsigned int i=0; i<iNumBits; i++)
  {
#ifdef USE_BITADDRESS_SETTER_GETTER
    set(TO_UINT8_PTR(mBuffer), input.get(iSourceBits) );
#endif
#ifdef USE_SINGLEBIT_MACROS
    setSingleBit(TO_UINT8_PTR(mBuffer)[byte], bit, getSingleBit( TO_UINT8_PTR(iSourceBits)[input.byte], input.bit) );
#endif
    next();
    input.next();
  }
}

#endif //USE_BITADDRESS_READ_WRITE


// ===============================================================================================
#ifndef USE_BITADDRESS_READ_WRITE

BitReader::BitReader()
{
  mBuffer = NULL;
}

void BitReader::setBuffer(const void * iBuffer)
{
  mBuffer = (const unsigned char *)iBuffer;
  mAddr.reset();
}

void BitReader::read(unsigned int iNumBits, void * iTargetBits)
{
  BitAddress output;
  for(unsigned int i=0; i<iNumBits; i++)
  {
#ifdef USE_BITADDRESS_SETTER_GETTER
    output.set(iTargetBits, mAddr.get(mBuffer) );
#endif
#ifdef USE_SINGLEBIT_MACROS
    setSingleBit(TO_UINT8_PTR(iTargetBits)[output.byte], output.bit, getSingleBit(mBuffer[mAddr.byte], mAddr.bit) );
#endif
    mAddr.next();
    output.next();
  }
}

unsigned char BitReader::read8(unsigned int iNumBits)
{
  unsigned char bits = 0;
  read( MIN_BETWEEN2(1*BITS_PER_BYTE, iNumBits), (unsigned char *)&bits); 
  return bits;
}

unsigned short BitReader::read16(unsigned int iNumBits)
{
  unsigned short bits = 0;
  read( MIN_BETWEEN2(2*BITS_PER_BYTE, iNumBits), (unsigned char *)&bits); 
  return bits;
}

const BitAddress & BitReader::getAddress()
{
  return mAddr;
}

// ===============================================================================================

BitWriter::BitWriter()
{
  mBuffer = NULL;
}

void BitWriter::setBuffer(void * iBuffer)
{
  mBuffer = (unsigned char *)iBuffer;
  mAddr.reset();
}

void BitWriter::write(const void * iSourceBits, unsigned int iNumBits)
{
  BitAddress input;
  for(unsigned int i=0; i<iNumBits; i++)
  {
#ifdef USE_BITADDRESS_SETTER_GETTER
    mAddr.set(mBuffer, input.get(iSourceBits) );
#endif
#ifdef USE_SINGLEBIT_MACROS
    setSingleBit(mBuffer[mAddr.byte], mAddr.bit, getSingleBit( TO_UINT8_PTR(iSourceBits)[input.byte], input.bit) );
#endif
    mAddr.next();
    input.next();
  }
}

void BitWriter::write8(unsigned char iSourceBits, unsigned int iNumBits)
{
  write(&iSourceBits, MIN_BETWEEN2(1*BITS_PER_BYTE, iNumBits) );
}

void BitWriter::write16(unsigned short iSourceBits, unsigned int iNumBits)
{
  write((unsigned char *)&iSourceBits, MIN_BETWEEN2(2*BITS_PER_BYTE, iNumBits) );
}

void BitWriter::skip(unsigned int iNumBits)
{
  mAddr.add(iNumBits);
}

const BitAddress & BitWriter::getAddress()
{
  return mAddr;
}

#endif //USE_BITADDRESS_READ_WRITE
