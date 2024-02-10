# Changelog

## v2.0.0
This version revamps the entire API in order to improve usability and performance.
This means that the interfaces of most functions have changed. Here are some of the general changes:
- Most functions now return a value of type `nbt_result_t` to allow for proper error checking.
	Functions which previously returned something else (e.g. an `nbt_tag_t`), now take an 'out' parameter.
- Support for custom allocators has been greatly improved. Functions which allocate memory now take an
	additional `alloc` parameter pointing to an `nbt_allocator_t` object. If you just want to use the
	standard allocation functions (`malloc`/`realloc`/`free`), you can pass a `NULL` pointer instead.

- The biggest change is that `nbt_tag_t` is generally handled by value rather than by pointer.
	- This reduces the number of dynamic memory allocations which need to happen.
	- Note that many functions still take pointers to tags, but they *do not* take ownership of these tags.
- The functions for creating `nbt_tag_t`s are now named `nbt_new_<type>` rather than `nbt_new_tag_<type>`. These functions also now return the `nbt_tag_t` by value.
	- The names of some of the elements have changed. `value` is now `elements` for array, list and compound types, and `size` is now `length` for string types.
- The functions `nbt_tag_list_append`, `nbt_tag_list_get`, `nbt_tag_compound_append` and `nbt_tag_compound_get` have been renamed to `nbt_list_append`, `nbt_list_get`, `nbt_compound_append` and `nbt_compound_get` respectively.
- Version is now indicated by three `#define`s: `LIBNBT_VERSION_MAJOR`, `LIBNBT_VERSION_MINOR` and `LIBNBT_VERSION_PATCH`.
	- For this release, these are defined as `2`, `0` and `0`, respectively.
- Removed `NBT_NO_STDINT` (it wasn't particularly useful in hindsight.)

## v1.0.1
- The implementation now compiles as C++.
- Other than that, this version is mostly for compatibility.
- The `LIBNBT_VERSION` is set to `11` for this release.
