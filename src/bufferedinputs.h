// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef BUFFERED_INPUTS_H
#define BUFFERED_INPUTS_H

#include "Arduino.h"

namespace anyrtttl
{

/****************************************************************************
 * Custom typedefs
 ****************************************************************************/

typedef struct buffered_input_t {
  void * self;    // the address of this instance
  void * opaque;  // an opaque pointer for storing custom data. Usually for implementing peekByte() and readByte() functions.
  char (*peekByte)(buffered_input_t * self);
  char (*readByte)(buffered_input_t * self);
  void (*seek)(buffered_input_t * self, int count);
  void (*free)(buffered_input_t * self);
} input_buffer_t;

/****************************************************************************
 * Custom functions
 ****************************************************************************/

/****************************************************************************
 * Description:
 *   Setup a buffered input type that get input from a c-string.
 * Parameters:
 *   input:     The buffered_input_t to setup.
 *   value:     The value of the c-string.
 ****************************************************************************/
void initStringBufferedInput(buffered_input_t * input, const char * value);

/****************************************************************************
 * Description:
 *   Setup a buffered input type that get input from a buffer in Program Memory address space.
 * Parameters:
 *   input:     The buffered_input_t to setup.
 *   addr:      The address of the c-string.
 ****************************************************************************/
void initProgmemBufferedInput(buffered_input_t * input, const char * addr);

/****************************************************************************
 * Description:
 *   Setup a buffered input type that get input from binary buffer.
 * Parameters:
 *   input:     The buffered_input_t to setup.
 *   buffer:    The address of the input buffer.
 *   size:      The size in bytes of the input buffer.
 ****************************************************************************/
void initBinaryBufferedInput(buffered_input_t * input, const unsigned char * buffer, size_t size);

}; //anyrtttl namespace

#endif //BUFFERED_INPUTS_H
