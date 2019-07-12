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
The root tag of the parsed NBT structure.
