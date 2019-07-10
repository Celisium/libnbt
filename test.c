#include <stdio.h>
#define NBT_IMPLEMENTATION
#include "nbt.h"

nbt_reader_t reader;

static size_t reader_read(void* userdata, uint8_t* data, size_t size) {
  return fread(data, 1, size, userdata);
}

static void print_nbt_tree(nbt_tag_t* tag, int indentation) {
  for (int i = 0; i < indentation; i++) {
    printf(" ");
  }

  if (tag->name) {
    printf("%s: ", tag->name);
  }

  switch (tag->type) {
    case NBT_TYPE_END: {
      printf("[end]");
      break;
    }
    case NBT_TYPE_BYTE: {
      printf("%hhd", tag->tag_byte.value);
      break;
    }
    case NBT_TYPE_SHORT: {
      printf("%hd", tag->tag_short.value);
      break;
    }
    case NBT_TYPE_INT: {
      printf("%d", tag->tag_int.value);
      break;
    }
    case NBT_TYPE_LONG: {
      printf("%ld", tag->tag_long.value);
      break;
    }
    case NBT_TYPE_FLOAT: {
      printf("%f", tag->tag_float.value);
      break;
    }
    case NBT_TYPE_DOUBLE: {
      printf("%f", tag->tag_double.value);
      break;
    }
    case NBT_TYPE_BYTE_ARRAY: {
      printf("[byte array]");
      break;
      for (size_t i = 0; i < tag->tag_byte_array.size; i++) {
        printf("%hhd ", tag->tag_byte_array.value[i]);
      }
      break;
    }
    case NBT_TYPE_STRING: {
      printf("%s", tag->tag_string.value);
      break;
    }
    case NBT_TYPE_LIST: {
      printf("\n");
      for (size_t i = 0; i < tag->tag_list.size; i++) {
        print_nbt_tree(tag->tag_list.value[i], indentation + tag->name_size + 2);
      }
      break;
    }
    case NBT_TYPE_COMPOUND: {
      printf("\n");
      for (size_t i = 0; i < tag->tag_compound.size; i++) {
        print_nbt_tree(tag->tag_compound.value[i], indentation + tag->name_size + 2);
      }
      break;
    }
    case NBT_TYPE_INT_ARRAY: {
      printf("[int array]");
      break;
      for (size_t i = 0; i < tag->tag_int_array.size; i++) {
        printf("%d ", tag->tag_int_array.value[i]);
      }
      break;
    }
    case NBT_TYPE_LONG_ARRAY: {
      printf("[long array]");
      break;
      for (size_t i = 0; i < tag->tag_long_array.size; i++) {
        printf("%ld ", tag->tag_long_array.value[i]);
      }
      break;
    }
    default: {
      printf("[error]");
    }
  }

  printf("\n");
}

int main() {

  // Example 1: Loading an NBT file from disk.
  FILE* file = fopen("bigtest-gzip.nbt", "rb");

  reader.read = reader_read;
  reader.userdata = file;

  nbt_tag_t* tag = nbt_parse(reader, NBT_PARSE_FLAG_FORCE_GZIP);

  printf("Example 1:\n");
  print_nbt_tree(tag, 2);

  // Example 2: Creating a new NBT tree from scratch.
  nbt_tag_t* tag_level = nbt_new_tag_compound();
  nbt_set_tag_name(tag_level, "Level", strlen("Level"));

  nbt_tag_t* tag_longtest = nbt_new_tag_long(9223372036854775807);
  nbt_set_tag_name(tag_longtest, "longTest", strlen("longTest"));

  nbt_tag_t* tag_shorttest = nbt_new_tag_short(32767);
  nbt_set_tag_name(tag_shorttest, "shortTest", strlen("shortTest"));

  nbt_tag_t* tag_stringtest = nbt_new_tag_string("HELLO WORLD THIS IS A TEST STRING ÅÄÖ!", strlen("HELLO WORLD THIS IS A TEST STRING ÅÄÖ!"));
  nbt_set_tag_name(tag_stringtest, "stringTest", strlen("stringTest"));

  nbt_tag_compound_append(tag_level, tag_longtest);
  nbt_tag_compound_append(tag_level, tag_shorttest);
  nbt_tag_compound_append(tag_level, tag_stringtest);

  printf("Example 2:\n");
  print_nbt_tree(tag_level, 2);

  return 0; 
}
