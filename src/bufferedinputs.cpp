// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#include "Arduino.h"
#include "bufferedinputs.h"
#include <stdlib.h> // for malloc() and free()

namespace anyrtttl
{

/****************************************************************************
 * String buffered input functions
 ****************************************************************************/

typedef struct string_buffer_helper_t {
  const char * str;
  size_t pos;
} string_buffer_helper_t;

char stringBufferPeekByte(buffered_input_t * self)
{
  string_buffer_helper_t * helper = (string_buffer_helper_t *)self->opaque;
  char c = helper->str[helper->pos];
  return c;
}

char stringBufferReadByte(buffered_input_t * self)
{
  string_buffer_helper_t * helper = (string_buffer_helper_t *)self->opaque;
  char c = helper->str[helper->pos];
  helper->pos++;
  return c;
}

void stringBufferSeek(buffered_input_t * self, int count)
{
  string_buffer_helper_t * helper = (string_buffer_helper_t *)self->opaque;
  helper->pos += count;
}

void stringBufferFree(buffered_input_t * self)
{
  string_buffer_helper_t * helper = (string_buffer_helper_t *)self->opaque;
  free(helper);
  self->opaque = NULL;
}

void initStringBufferedInput(buffered_input_t * input, const char * value)
{
  memset(input, 0, sizeof(buffered_input_t));
  input->self = input;
  input->opaque = (string_buffer_helper_t*)malloc(sizeof(string_buffer_helper_t));

  string_buffer_helper_t * helper = (string_buffer_helper_t *)input->opaque;
  memset(helper, 0, sizeof(string_buffer_helper_t));
  helper->str = value;
  helper->pos = 0;

  input->peekByte = &stringBufferPeekByte;
  input->readByte = &stringBufferReadByte;
  input->seek = &stringBufferSeek;
  input->free = &stringBufferFree;
}

/****************************************************************************
 * Program Memory buffered input functions
 ****************************************************************************/

typedef struct progmem_buffer_helper_t {
  const char * addr;
  size_t pos;
} progmem_buffer_helper_t;

char progmemBufferPeekByte(buffered_input_t * self)
{
  progmem_buffer_helper_t * helper = (progmem_buffer_helper_t *)self->opaque;
  char c = helper->addr[helper->pos];
  return c;
}

char progmemBufferReadByte(buffered_input_t * self)
{
  progmem_buffer_helper_t * helper = (progmem_buffer_helper_t *)self->opaque;
  char c = pgm_read_byte_near(&helper->addr[helper->pos]);
  helper->pos++;
  return c;
}

void progmemBufferSeek(buffered_input_t * self, int count)
{
  progmem_buffer_helper_t * helper = (progmem_buffer_helper_t *)self->opaque;
  helper->pos += count;
}

void progmemBufferFree(buffered_input_t * self)
{
  progmem_buffer_helper_t * helper = (progmem_buffer_helper_t *)self->opaque;
  free(helper);
  self->opaque = NULL;
}

void initProgmemBufferedInput(buffered_input_t * input, const char * addr)
{
  memset(input, 0, sizeof(buffered_input_t));
  input->self = input;
  input->opaque = (progmem_buffer_helper_t*)malloc(sizeof(progmem_buffer_helper_t));

  progmem_buffer_helper_t * helper = (progmem_buffer_helper_t *)input->opaque;
  memset(helper, 0, sizeof(progmem_buffer_helper_t));
  helper->addr = addr;
  helper->pos = 0;

  input->peekByte = &progmemBufferPeekByte;
  input->readByte = &progmemBufferReadByte;
  input->seek = &progmemBufferSeek;
  input->free = &progmemBufferFree;
}

/****************************************************************************
 * Binary buffered input functions
 ****************************************************************************/

typedef struct binary_buffer_helper_t {
  const unsigned char * buffer;
  size_t pos;
  size_t size;
} binary_buffer_helper_t;

char binaryBufferPeekByte(buffered_input_t * self)
{
  binary_buffer_helper_t * helper = (binary_buffer_helper_t *)self->opaque;
  if (helper->pos >= helper->size) return '\0';
  char c = helper->buffer[helper->pos];
  return c;
}

char binaryBufferReadByte(buffered_input_t * self)
{
  binary_buffer_helper_t * helper = (binary_buffer_helper_t *)self->opaque;
  if (helper->pos >= helper->size) return '\0';
  char c = helper->buffer[helper->pos];
  helper->pos++;
  return c;
}

void binaryBufferSeek(buffered_input_t * self, int count)
{
  binary_buffer_helper_t * helper = (binary_buffer_helper_t *)self->opaque;
  helper->pos += count;
}

void binaryBufferFree(buffered_input_t * self)
{
  binary_buffer_helper_t * helper = (binary_buffer_helper_t *)self->opaque;
  free(helper);
  self->opaque = NULL;
}

void initBinaryBufferedInput(buffered_input_t * input, const unsigned char * buffer, size_t size)
{
  memset(input, 0, sizeof(buffered_input_t));
  input->self = input;
  input->opaque = (binary_buffer_helper_t*)malloc(sizeof(binary_buffer_helper_t));

  binary_buffer_helper_t * helper = (binary_buffer_helper_t *)input->opaque;
  memset(helper, 0, sizeof(binary_buffer_helper_t));
  helper->buffer = buffer;
  helper->pos = 0;
  helper->size = size;

  input->peekByte = &binaryBufferPeekByte;
  input->readByte = &binaryBufferReadByte;
  input->seek = &binaryBufferSeek;
  input->free = &binaryBufferFree;
}

}; //anyrtttl namespace
