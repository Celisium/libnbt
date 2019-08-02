# libnbt documentation

## Types

### `nbt_tag_t`

#### Definition
```c
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
```

#### Description
`nbt_tag_t` is a struct which represents a single NBT tag. It contains the type of the tag as well as the name and the value of the tag.

#### Members
* `type`: The type of the tag (see the `nbt_tag_type_t` enum).
* `name`: The name of the tag. If the tag does not have a name (e.g. members of a list), this will be a null pointer. This string is guaranteed to be null terminated for convenience, but embedded nulls may also be present.
* `name_size`: The number of bytes used to store the name, excluding the null terminator. If non-ASCII characters are used, this may not be equal to the number of characters in the name. If the tag does not have a name, this will be 0.
* `tag_xxx` (where `xxx` is an NBT tag type, in lower case): The value of the NBT tag. Only the one corresponding to the tag's type should be accessed, with the values of the other members being undefined.

### `nbt_reader_t`

#### Definition
```c
typedef struct {
  size_t (*read)(void* userdata, uint8_t* data, size_t size);
  void* userdata;
} nbt_reader_t;
```

#### Description
`nbt_reader_t` is a struct which can be used to read NBT data from an arbitrary source.  
It contains a pointer to a function used to read data, and a user-provided pointer which is passed to this function.

#### Members
* `read`: A pointer to the function which is used to read data. The function is expected to copy up to `size` bytes into the buffer pointed to by `data`, with the return value being the number of bytes actually read.
* `userdata`: An arbitrary, user-provided pointer which is passed as the `userdata` parameter to the aforementioned `read` function.

### `nbt_writer_t`

#### Definition
```c
typedef struct {
  size_t (*write)(void* userdata, uint8_t* data, size_t size);
  void* userdata;
} nbt_writer_t;
```

#### Description
`nbt_writer_t` is a struct which can be used to write NBT data to an arbitrary destination.
It contains a pointer to a function used to write data, and a user-provided pointer which is passed to this function.

#### Members
* `write`: A pointer to the function which is used to write data. The function is expected to write up to `size` bytes from the buffer pointed to by `data`, with the return value being the number of bytes actually written.
* `userdata`: An arbitrary, user-provided pointer which is passed as the `userdata` parameter to the aforementioned `write` function.

## Enums

### `nbt_tag_type_t`

#### Definition
```c
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
```

#### Description
Represents an NBT tag type. Each type (including `TAG_End`) can be represented and has the same integer value as in the official specification.
`NBT_NO_OVERRIDE` is only used internally and will not appear in any output provided by the library, and must not be used in any input provided to it.

### `nbt_parse_flags_t`

#### Definition
```c
typedef enum {
  NBT_PARSE_FLAG_USE_GZIP = 1,
  NBT_PARSE_FLAG_USE_ZLIB = 2,
  NBT_PARSE_FLAG_USE_RAW = 3,
} nbt_parse_flags_t;
```

#### Description
Represents flags which can be provided to `nbt_parse`. See `nbt_parse` for their meanings.

### `nbt_write_flags_t`

#### Definition
```c
typedef enum {
  NBT_WRITE_FLAG_USE_GZIP = 1,
  NBT_WRITE_FLAG_USE_ZLIB = 2,
  NBT_WRITE_FLAG_USE_RAW = 3
} nbt_write_flags_t;
```

#### Description
Represents flags which can be provided to `nbt_write`. See `nbt_write` for their meanings.

## Functions

### `nbt_parse`

#### Definition
```c
nbt_tag_t* nbt_parse(nbt_reader_t reader, int parse_flags);
```

#### Description
Parses a stream of bytes provided by `reader` into memory. Both raw and compressed streams are supported, with both zlib and Gzip formatted compressed streams being supported.

#### Parameters
* `reader`: The `nbt_reader_t` struct used to provide input.
* `parse_flags`: Flags used to control parsing.
  * `NBT_PARSE_FLAG_FORCE_GZIP`: Used to force Gzip decompression (as used by most .nbt files)
  * `NBT_PARSE_FLAG_FORCE_ZLIB`: Used to force zlib decompression (as used by chunks stored in .mca files).
  * `NBT_PARSE_FLAG_FORCE_RAW`: Used to force no decompression.

#### Return Value
The root tag of the parsed NBT structure, or `NULL` if parsing was unsuccessful.  
This value is dynamically allocated and should be freed using `nbt_free_tag`.

### `nbt_write`

#### Definition
```c
void nbt_write(nbt_writer_t writer, nbt_tag_t* tag, int write_flags);
```

#### Description
Converts the NBT tag structure `tag` to bytes and writes them to a stream provided by `writer`. Both raw and compressed streams are supported, with both zlib and Gzip formatted compressed streams being supported.

#### Parameters
* `writer`: The `nbt_writer_t` struct used to provide output.
* `tag`: The tag structure to be written.
* `write_flags`: Flags used to control parsing.
  * `NBT_WRITE_FLAG_FORCE_GZIP`: Used to force Gzip decompression (as used by most .nbt files)
  * `NBT_WRITE_FLAG_FORCE_ZLIB`: Used to force zlib decompression (as used by chunks stored in .mca files).
  * `NBT_WRITE_FLAG_FORCE_RAW`: Used to force no decompression.

#### Return Value
None.

### `nbt_new_tag_xxx` (where `xxx` is a type)

#### Definition
```c
nbt_tag_t* nbt_new_tag_byte(int8_t value);
nbt_tag_t* nbt_new_tag_short(int16_t value);
nbt_tag_t* nbt_new_tag_int(int32_t value);
nbt_tag_t* nbt_new_tag_long(int64_t value);
nbt_tag_t* nbt_new_tag_float(float value);
nbt_tag_t* nbt_new_tag_double(double value);
nbt_tag_t* nbt_new_tag_byte_array(int8_t* value, size_t size);
nbt_tag_t* nbt_new_tag_string(const char* value, size_t size);
nbt_tag_t* nbt_new_tag_list(nbt_tag_type_t type);
nbt_tag_t* nbt_new_tag_compound(void);
nbt_tag_t* nbt_new_tag_int_array(int32_t* value, size_t size);
nbt_tag_t* nbt_new_tag_long_array(int64_t* value, size_t size);
```

#### Description
Creates a new tag of the given type.  
The newly created type will have no name. Use `nbt_tag_set_name` to set its name.  

#### Parameters (`byte`, `short`, `int`, `long`, `float`, `double`)
* `value`: The value with which to initialise the tag.

#### Parameters (`byte_array`, `string`, `int_array`, `long_array`)
* `value`: The array of values with which to initialise the tag. This is copied, so it is safe to modify or free the value passed to this parameter later. In the case of `string`, the copy will be null-terminated for convenience, but embedded nulls may be present.
* `size`: The size of the array of values passed to `value`, in elements.

#### Parameters (`list`)
* `type`: The type of the entries of the list.

#### Parameters (`compound`)
None.

#### Return Value
The newly created tag. This value is dynamically allocated and should be freed using `nbt_free_tag`.

### `nbt_tag_set_name`

#### Definition
```c
void nbt_set_tag_name(nbt_tag_t* tag, const char* name, size_t size);
```

#### Description
Sets the name of the tag.

#### Parameters
* `tag`: The tag to set the name of. This is copied, so it is safe to modify or free the value passed to this parameter later.
* `name`: The new name of the tag.
* `size`: The size of the name, in bytes.

#### Return Value
None.

### `nbt_tag_list_append`

#### Definition
```c
void nbt_tag_list_append(nbt_tag_t* list, nbt_tag_t* value);
```

#### Description
Appends a tag to the end of a list tag.

#### Parameters
* `list`: The list tag to append to.
* `value`: The tag to be appended.

#### Return Value
None.

### `nbt_tag_list_get`

#### Definition
```c
nbt_tag_t* nbt_tag_list_get(nbt_tag_t* tag, size_t index);
```

#### Description
Gets the tag at index `index` of list tag `tag`.  
This is equivalent to `tag->tag_list.value[index]` and is provided for consistency with `nbt_tag_compound_get`.

#### Parameters
* `tag`: The list tag.
* `index`: The index to get.

#### Return Value
The tag at `index`.

### `nbt_tag_compound_append`

#### Definition
```c
void nbt_tag_compound_append(nbt_tag_t* compound, nbt_tag_t* value);
```

#### Description
Appends a tag to a compound tag.  
The key of the tag is simply its name.  
The result of appending multiple tags with the same name is undefined.

#### Parameters
* `tag`: The compound tag to append to.
* `value`: The tag to append.

#### Return Value
None.

### `nbt_tag_compound_get`

#### Definition
```c
nbt_tag_t* nbt_tag_compound_get(nbt_tag_t* tag, const char* key);
```

#### Description
Gets the tag with key `key` in compound tag `tag`.

#### Parameters
* `tag`: The list tag.
* `key`: The key to search for.

#### Return Value
The tag with key `key`, or `NULL` if no tag with that key was found.

### `nbt_free_tag`

#### Definition
```c
void nbt_free_tag(nbt_tag_t* tag);
```

#### Description
Frees the memory allocated for `tag`.  
In the case of list and compound tags, this function is called recursively on all children, so they do not need to be freed manually.

#### Parameters
* `tag`: The tag to free.

#### Return Value
None.
