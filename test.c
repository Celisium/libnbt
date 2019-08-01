#include <stdio.h>
#define NBT_IMPLEMENTATION
#include "nbt.h"

static size_t reader_read(void* userdata, uint8_t* data, size_t size) {
  return fread(data, 1, size, userdata);
}

static size_t writer_write(void* userdata, uint8_t* data, size_t size) {
  return fwrite(data, 1, size, userdata);
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

nbt_tag_t* read_nbt_file(const char* name, int flags) {

  FILE* file = fopen(name, "rb");

  nbt_reader_t reader;

  reader.read = reader_read;
  reader.userdata = file;

  nbt_tag_t* tag = nbt_parse(reader, flags);

  fclose(file);

  return tag;
}

void write_nbt_file(const char* name, nbt_tag_t* tag, int flags) {

  FILE* file = fopen(name, "wb");

  nbt_writer_t writer;

  writer.write = writer_write;
  writer.userdata = file;

  nbt_write(writer, tag, flags);

  fclose(file);
}

int main() {

  // Example 1: Loading an NBT file from disk.
  printf("Reading Example 1:\n");

  nbt_tag_t* tag = read_nbt_file("bigtest_gzip.nbt", NBT_PARSE_FLAG_USE_GZIP);

  print_nbt_tree(tag, 2);

  // Example 2: Creating a new NBT tree from scratch.
  printf("Reading Example 2:\n");

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

  print_nbt_tree(tag_level, 2);

  printf("Writing Example 1:\n");

  write_nbt_file("write_test_raw.nbt", tag_level, NBT_WRITE_FLAG_USE_RAW);

  nbt_tag_t* read_test_1 = read_nbt_file("write_test_raw.nbt", NBT_PARSE_FLAG_USE_RAW);

  print_nbt_tree(read_test_1, 2);

  printf("Writing Example 2:\n");

  write_nbt_file("write_test_zlib.nbt", tag_level, NBT_WRITE_FLAG_USE_ZLIB);

  nbt_tag_t* read_test_2 = read_nbt_file("write_test_zlib.nbt", NBT_PARSE_FLAG_USE_ZLIB);

  print_nbt_tree(read_test_2, 2);

  printf("Writing Example 3:\n");

  write_nbt_file("write_test_gzip.nbt", tag_level, NBT_WRITE_FLAG_USE_GZIP);

  nbt_tag_t* read_test_3 = read_nbt_file("write_test_gzip.nbt", NBT_PARSE_FLAG_USE_GZIP);

  print_nbt_tree(read_test_3, 2);

  // Free the tags.
  nbt_free_tag(tag);
  nbt_free_tag(tag_level);
  nbt_free_tag(read_test_1);
  nbt_free_tag(read_test_2);
  nbt_free_tag(read_test_3);

  return 0; 
}
