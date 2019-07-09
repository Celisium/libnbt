/* 
  libnbt
  Written in 2019 by IDidMakeThat

  To the extent possible under law, the author(s) have dedicated all copyright
  and related and neighboring rights to this software to the public domain
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along
  with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef NBT_H
#define NBT_H

#define LIBNBT_VERSION 10
#define LIBNBT_NBT_VERSION 19133

#include <stddef.h>

#ifndef NBT_NO_STDLIB
#include <stdlib.h>
#include <string.h>
#define NBT_MALLOC malloc
#define NBT_REALLOC realloc
#define NBT_FREE free
#define NBT_MEMCPY memcpy
#endif

#ifndef NBT_NO_STDINT
#include <stdint.h>
#endif

#ifdef NBT_OWN_ZLIB
#include NBT_OWN_ZLIB
#else
#include "miniz.h"
#endif

#ifndef NBT_BUFFER_SIZE
#define NBT_BUFFER_SIZE 32768
#endif

typedef enum {
  NBT_TYPE_END,
  NBT_TYPE_BYTE,
  NBT_TYPE_SHORT,
  NBT_TYPE_INT,
  NBT_TYPE_LONG,
  NBT_TYPE_FLOAT,
  NBT_TYPE_DOUBLE,
  NBT_TYPE_BYTE_ARRAY,
  NBT_TYPE_STRING,
  NBT_TYPE_LIST,
  NBT_TYPE_COMPOUND,
  NBT_TYPE_INT_ARRAY,
  NBT_TYPE_LONG_ARRAY,
  NBT_NO_OVERRIDE // Only used internally.
} nbt_tag_type_t;

typedef struct nbt_tag_t nbt_tag_t;

struct nbt_tag_t {

  nbt_tag_type_t type;

  char* name;
  size_t name_size;

  union {
    struct {
      int8_t value;
    } tag_byte;
    struct {
      int16_t value;
    } tag_short;
    struct {
      int32_t value;
    } tag_int;
    struct {
      int64_t value;
    } tag_long;
    struct {
      float value;
    } tag_float;
    struct {
      double value;
    } tag_double;
    struct {
      int8_t* value;
      size_t size;
    } tag_byte_array;
    struct {
      char* value;
      size_t size;
    } tag_string;
    struct {
      nbt_tag_t** value;
      nbt_tag_type_t type;
      size_t size;
    } tag_list;
    struct {
      nbt_tag_t** value;
      size_t size;
    } tag_compound;
    struct {
      int32_t* value;
      size_t size;
    } tag_int_array;
    struct {
      int64_t* value;
      size_t size;
    } tag_long_array;
  };

};

typedef struct {
  size_t (*read)(void* userdata, uint8_t* data, size_t size);
  void* userdata;
} nbt_reader_t;

typedef enum {
  NBT_PARSE_FLAG_FORCE_GZIP = 1,
  NBT_PARSE_FLAG_FORCE_ZLIB = 2,
  NBT_PARSE_FLAG_FORCE_RAW = 3,
} nbt_parse_flags_t;

nbt_tag_t* nbt_parse(nbt_reader_t reader, int parse_flags);

void nbt_free_tag(nbt_tag_t* tag);

#endif

#ifdef NBT_IMPLEMENTATION

typedef struct {
  uint8_t* buffer;
  size_t buffer_offset;
} nbt__stream_t;

static uint8_t nbt__get_byte(nbt__stream_t* stream) {

  return stream->buffer[stream->buffer_offset++];

}

static int16_t nbt__get_int16(nbt__stream_t* stream) {
  uint8_t bytes[2];
  for (int i = 1; i >= 0; i--) {
    bytes[i] = nbt__get_byte(stream);
  }
  return *(int16_t*)(bytes);
}

static int32_t nbt__get_int32(nbt__stream_t* stream) {
  uint8_t bytes[4];
  for (int i = 3; i >= 0; i--) {
    bytes[i] = nbt__get_byte(stream);
  }
  return *(int32_t*)(bytes);
}

static int64_t nbt__get_int64(nbt__stream_t* stream) {
  uint8_t bytes[8];
  for (int i = 7; i >= 0; i--) {
    bytes[i] = nbt__get_byte(stream);
  }
  return *(int64_t*)(bytes);
}

static float nbt__get_float(nbt__stream_t* stream) {
  uint8_t bytes[4];
  for (int i = 3; i >= 0; i--) {
    bytes[i] = nbt__get_byte(stream);
  }
  return *(float*)(bytes);
}

static double nbt__get_double(nbt__stream_t* stream) {
  uint8_t bytes[8];
  for (int i = 7; i >= 0; i--) {
    bytes[i] = nbt__get_byte(stream);
  }
  return *(double*)(bytes);
}

static nbt_tag_t* nbt__parse(nbt__stream_t* stream, int parse_name, nbt_tag_type_t override_type) {

  nbt_tag_t* tag = (nbt_tag_t*)NBT_MALLOC(sizeof(nbt_tag_t));

  if (override_type == NBT_NO_OVERRIDE) {
    tag->type = nbt__get_byte(stream);
  } else {
    tag->type = override_type;
  }

  if (parse_name && tag->type != NBT_TYPE_END) {
    tag->name_size = nbt__get_int16(stream);
    tag->name = (char*)NBT_MALLOC(tag->name_size + 1);
    for (size_t i = 0; i < tag->name_size; i++) {
      tag->name[i] = nbt__get_byte(stream);
    }
    tag->name[tag->name_size] = '\0';
  } else {
    tag->name = NULL;
    tag->name_size = 0;
  }

  switch (tag->type) {
    case NBT_TYPE_END: {
      // Don't do anything.
      break;
    }
    case NBT_TYPE_BYTE: {
      tag->tag_byte.value = nbt__get_byte(stream);
      break;
    }
    case NBT_TYPE_SHORT: {
      tag->tag_short.value = nbt__get_int16(stream);
      break;
    }
    case NBT_TYPE_INT: {
      tag->tag_int.value = nbt__get_int32(stream);
      break;
    }
    case NBT_TYPE_LONG: {
      tag->tag_long.value = nbt__get_int64(stream);
      break;
    }
    case NBT_TYPE_FLOAT: {
      tag->tag_float.value = nbt__get_float(stream);
      break;
    }
    case NBT_TYPE_DOUBLE: {
      tag->tag_double.value = nbt__get_double(stream);
      break;
    }
    case NBT_TYPE_BYTE_ARRAY: {
      tag->tag_byte_array.size = nbt__get_int32(stream);
      tag->tag_byte_array.value = (int8_t*)NBT_MALLOC(tag->tag_byte_array.size);
      for (size_t i = 0; i < tag->tag_byte_array.size; i++) {
        tag->tag_byte_array.value[i] = nbt__get_byte(stream);
      }
      break;
    }
    case NBT_TYPE_STRING: {
      tag->tag_string.size = nbt__get_int16(stream);
      tag->tag_string.value = (char*)NBT_MALLOC(tag->tag_string.size + 1);
      for (size_t i = 0; i < tag->tag_string.size; i++) {
        tag->tag_string.value[i] = nbt__get_byte(stream);
      }
      tag->tag_string.value[tag->tag_string.size] = '\0';
      break;
    }
    case NBT_TYPE_LIST: {
      tag->tag_list.type = nbt__get_byte(stream);
      tag->tag_list.size = nbt__get_int32(stream);
      tag->tag_list.value = (nbt_tag_t**)NBT_MALLOC(tag->tag_list.size * sizeof(nbt_tag_t*));
      for (size_t i = 0; i < tag->tag_list.size; i++) {
        tag->tag_list.value[i] = nbt__parse(stream, 0, tag->tag_list.type);
      }
      break;
    }
    case NBT_TYPE_COMPOUND: {
      tag->tag_compound.size = 0;
      tag->tag_compound.value = NULL;
      for (;;) {
        nbt_tag_t* inner_tag = nbt__parse(stream, 1, NBT_NO_OVERRIDE);

        if (inner_tag->type == NBT_TYPE_END) {
          nbt_free_tag(inner_tag);
          break;
        } else {
          tag->tag_compound.value = (nbt_tag_t**)NBT_REALLOC(tag->tag_compound.value, (tag->tag_compound.size + 1) * sizeof(nbt_tag_t*));
          tag->tag_compound.value[tag->tag_compound.size] = inner_tag;
          tag->tag_compound.size++;
        }
      }
      break;
    }
    case NBT_TYPE_INT_ARRAY: {
      tag->tag_int_array.size = nbt__get_int32(stream);
      tag->tag_int_array.value = (int32_t*)NBT_MALLOC(tag->tag_int_array.size * sizeof(int32_t));
      for (size_t i = 0; i < tag->tag_int_array.size; i++) {
        tag->tag_int_array.value[i] = nbt__get_int32(stream);
      }
      break;
    }
    case NBT_TYPE_LONG_ARRAY: {
      tag->tag_long_array.size = nbt__get_int32(stream);
      tag->tag_long_array.value = (int64_t*)NBT_MALLOC(tag->tag_long_array.size * sizeof(int64_t));
      for (size_t i = 0; i < tag->tag_long_array.size; i++) {
        tag->tag_long_array.value[i] = nbt__get_int64(stream);
      }
      break;
    }
    default: {
      NBT_FREE(tag);
      return NULL;
    }

  }

  return tag;

}

nbt_tag_t* nbt_parse(nbt_reader_t reader, int parse_flags) {

  int compressed;
  int gzip_format;
  switch (parse_flags & 3) {
    case 0: { // Automatic detection (not yet implemented).
      compressed = 1;
      gzip_format = 1;
      break;
    }
    case 1: { // gzip
      compressed = 1;
      gzip_format = 1;
      break;
    }
    case 2: { // zlib
      compressed = 1;
      gzip_format = 0;
      break;
    }
    case 3: { // raw
      compressed = 0;
      gzip_format = 0;
      break;
    }
  }

  uint8_t* buffer = NULL;
  size_t buffer_size = 0;

  nbt__stream_t stream;

  if (compressed) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    if (gzip_format) {
      uint8_t header[10];
      reader.read(reader.userdata, header, 10);
      int fhcrc = header[3] & 2;
      int fextra = header[3] & 4;
      int fname = header[3] & 8;
      int fcomment = header[3] & 16;

      (void)fextra; // I don't think many files use this.

      if (fname) {
        uint8_t byte = 0;
        do {
          reader.read(reader.userdata, &byte, 1);
        } while (byte != 0);
      }

      if (fcomment) {
        uint8_t byte = 0;
        do {
          reader.read(reader.userdata, &byte, 1);
        } while (byte != 0);
      }

      uint16_t crc;
      if (fhcrc) {
        reader.read(reader.userdata, (uint8_t*)&crc, 2);
      }

      (void)crc;
    }

    int ret = inflateInit2(&stream, gzip_format ? -Z_DEFAULT_WINDOW_BITS : Z_DEFAULT_WINDOW_BITS);
    if (ret != Z_OK) {
      NBT_FREE(buffer);
      return NULL;
    }

    uint8_t in_buffer[NBT_BUFFER_SIZE];
    uint8_t out_buffer[NBT_BUFFER_SIZE];
    do {
      stream.avail_in = reader.read(reader.userdata, in_buffer, NBT_BUFFER_SIZE);
      stream.next_in =  in_buffer;

      do {
        stream.avail_out = NBT_BUFFER_SIZE;
        stream.next_out = out_buffer;

        ret = inflate(&stream, Z_NO_FLUSH);

        size_t have = NBT_BUFFER_SIZE - stream.avail_out;
        buffer = (uint8_t*)NBT_REALLOC(buffer, buffer_size + have);
        NBT_MEMCPY(buffer + buffer_size, out_buffer, have);
        buffer_size += have;
        
      } while (stream.avail_out == 0);

      
    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);
    
  } else {

    uint8_t in_buffer[NBT_BUFFER_SIZE];
    size_t bytes_read;
    do {
      bytes_read = reader.read(reader.userdata, in_buffer, NBT_BUFFER_SIZE);
      buffer = (uint8_t*)NBT_REALLOC(buffer, buffer_size + bytes_read);
      NBT_MEMCPY(buffer + buffer_size, in_buffer, bytes_read);
      buffer_size += bytes_read;
    } while (bytes_read == NBT_BUFFER_SIZE);

  }

  stream.buffer = buffer;
  stream.buffer_offset = 0;

  nbt_tag_t* tag = nbt__parse(&stream, 1, NBT_NO_OVERRIDE);

  NBT_FREE(buffer);

  return tag;

}

void nbt_free_tag(nbt_tag_t* tag) {
  
}

#endif
