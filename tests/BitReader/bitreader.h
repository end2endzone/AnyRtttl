//
//  BitReader Library - v1.1 - 03/24/2016
//  Copyright (C) 2016 Antoine Beauchamp
//  The code & updates for the library can be found on http://end2endzone.com
//
// AUTHOR/LICENSE:
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 3.0 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License (LGPL-3.0) for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// DISCLAIMER:
//  This software is furnished "as is", without technical support, and with no 
//  warranty, express or implied, as to its usefulness for any purpose.
//
// PURPOSE:
//  This BitReader library allows one to read data which is not aligned on 
//  8, 16 or 32 bits structures. It is usefull for storing or decoding data in
//  binary format using the minimum amount of bits.
//  For example, storing words composed of only lower case letters (and spaces)
//  requires only 5 bits per characters instead of 8 bits (which saves 3 bits
//  per character). The library is particularly useful for dealing with bit
//  field structures.
//
// USAGE:
//  Create an instance of BitReader, BitWriter or BitAddress (depending on the 
//  optimization switches). Then assign a reading or writing buffer using the
//  setBuffer() method. Then call the write() method for writing bits and the
//  read() method for reading bits. Each method allows one to specify the amount
//  of bits to read or write.
//
//  See BitReader and BitWriter example for details
//
// HISTORY:
// 03/18/2016 v1.0 - Initial release.
// 03/24/2016 v1.1 - Optimized bit operations.
//

#ifndef BITREADER_H
#define BITREADER_H

#include "arduino.h"

#ifndef BITREADER_API
#define BITREADER_API
#endif

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif

//Bit optimization switches
#define USE_BITADDRESS_SETTER_GETTER
//#define USE_SINGLEBIT_MACROS
#define USE_BITADDRESS_READ_WRITE

struct BITREADER_API BitAddress
{
  unsigned int byte; //byte index
  unsigned char bit; //bit index

  BitAddress();
  BitAddress(const BitAddress & iBitAddress);
  void reset();
#ifdef USE_BITADDRESS_SETTER_GETTER
  void set(void * iBuffer, bool iBitValue);
  bool get(const void * iBuffer);
#endif
  void add(unsigned int iBits);
  void next();
  unsigned int toAbsBitOffset();

#ifdef WIN32
  std::string toString();
#endif

#ifdef USE_BITADDRESS_READ_WRITE
  void setBuffer(const void * iBuffer);
  void read(unsigned int iNumBits, void * iTargetBits);
  void write(const void * iSourceBits, unsigned int iNumBits);
  const unsigned char * mBuffer;
#endif

};

#ifndef USE_BITADDRESS_READ_WRITE

class BITREADER_API BitReader
{
public:
  BitReader();
  void setBuffer(const void * iBuffer);
  void read(unsigned int iNumBits, void * iTargetBits);
  unsigned char read8(unsigned int iNumBits);
  unsigned short read16(unsigned int iNumBits);
  const BitAddress & getAddress();
private:
  const unsigned char * mBuffer;
  BitAddress mAddr;
};

class BITREADER_API BitWriter
{
public:
  BitWriter();
  void setBuffer(void * iBuffer);
  void write(const void * iSourceBits, unsigned int iNumBits);
  void write8(unsigned char iSourceBits, unsigned int iNumBits);
  void write16(unsigned short iSourceBits, unsigned int iNumBits);
  void skip(unsigned int iNumBits);
  const BitAddress & getAddress();
private:
  unsigned char * mBuffer;
  BitAddress mAddr;
};

#endif //USE_BITADDRESS_READ_WRITE
#endif //BITREADER_H
