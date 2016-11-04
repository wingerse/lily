#ifndef LILY_API_VALUE_H
# define LILY_API_VALUE_H

/* For uint64_t. */
# include <stdint.h>
/* For FILE *. */
# include <stdio.h>

/* This file contains the structures and API needed by foreign functions to
   communicate with Lily. */

# ifndef LILY_STATE
#  define LILY_STATE
typedef struct lily_vm_state_ lily_state;
# endif

typedef struct lily_dynamic_val_    lily_dynamic_val;
typedef struct lily_file_val_       lily_file_val;
typedef struct lily_foreign_val_    lily_foreign_val;
typedef struct lily_function_val_   lily_function_val;
typedef struct lily_generic_val_    lily_generic_val;
typedef struct lily_hash_val_       lily_hash_val;
typedef struct lily_instance_val_   lily_instance_val;
typedef struct lily_list_val_       lily_list_val;
typedef struct lily_string_val_     lily_string_val;
typedef struct lily_value_          lily_value;

/* Put this macro at the top of any struct that you'll send to Lily as a foreign
   value. Don't rely on 'do_not_use', in case it changes in the future. */
#define LILY_FOREIGN_HEADER \
uint32_t refcount; \
uint32_t do_not_use; \
lily_destroy_func destroy_func;

/* This function is called when Lily wishes to destroy the value that has been
   provided. This action may have been triggered by the gc destroying a
   container holding the foreign value, or the foreign value's refcount falling
   to zero.

   The generic value provided is a raw pointer to the foreign struct that was
   provided to Lily earlier. The destroy function is responsible for destroying
   the content of the value, as well as the value itself.

   Destruction of values in Lily is designed to be atomic. A proper destroy
   function should not have any side-effects. */
typedef void (*lily_destroy_func)(lily_generic_val *);

#define DECLARE_SETTERS(name, ...) \
void lily_##name##_boolean(__VA_ARGS__, int); \
void lily_##name##_byte(__VA_ARGS__, uint8_t); \
void lily_##name##_bytestring(__VA_ARGS__, lily_string_val *); \
void lily_##name##_double(__VA_ARGS__, double); \
void lily_##name##_empty_variant(__VA_ARGS__, uint16_t); \
void lily_##name##_file(__VA_ARGS__, lily_file_val *); \
void lily_##name##_foreign(__VA_ARGS__, uint16_t, lily_foreign_val *); \
void lily_##name##_filled_variant(__VA_ARGS__, uint16_t, lily_instance_val *); \
void lily_##name##_hash(__VA_ARGS__, lily_hash_val *); \
void lily_##name##_instance(__VA_ARGS__, uint16_t, lily_instance_val *); \
void lily_##name##_integer(__VA_ARGS__, int64_t); \
void lily_##name##_list(__VA_ARGS__, lily_list_val *); \
void lily_##name##_string(__VA_ARGS__, lily_string_val *); \
void lily_##name##_tuple(__VA_ARGS__, lily_list_val *); \
void lily_##name##_unit(__VA_ARGS__); \
void lily_##name##_value(__VA_ARGS__, lily_value *); \

#define DECLARE_GETTERS(name, ...) \
int                lily_##name##_boolean(__VA_ARGS__); \
uint8_t            lily_##name##_byte(__VA_ARGS__); \
lily_string_val *  lily_##name##_bytestring(__VA_ARGS__); \
double             lily_##name##_double(__VA_ARGS__); \
lily_file_val *    lily_##name##_file(__VA_ARGS__); \
FILE *             lily_##name##_file_raw(__VA_ARGS__); \
lily_function_val *lily_##name##_function(__VA_ARGS__); \
lily_hash_val *    lily_##name##_hash(__VA_ARGS__); \
lily_generic_val * lily_##name##_generic(__VA_ARGS__); \
lily_instance_val *lily_##name##_instance(__VA_ARGS__); \
int64_t            lily_##name##_integer(__VA_ARGS__); \
lily_list_val *    lily_##name##_list(__VA_ARGS__); \
lily_string_val *  lily_##name##_string(__VA_ARGS__); \
char *             lily_##name##_string_raw(__VA_ARGS__); \
lily_value *       lily_##name##_value(__VA_ARGS__);

#define DECLARE_BOTH(name, ...) \
DECLARE_SETTERS(name##_set, __VA_ARGS__) \
DECLARE_GETTERS(name, __VA_ARGS__)

/* Build operations
   These operations have been made to help with dynaloading vars. These will
   wrap over a raw value to provide the lily_value * that the loader expects, so
   that the embedder need not worry about raw moves. */
lily_value *lily_new_value_of_byte(uint8_t);
lily_value *lily_new_value_of_bytestring(lily_string_val *);
lily_value *lily_new_value_of_double(double);
lily_value *lily_new_value_of_enum(uint16_t, lily_instance_val *);
lily_value *lily_new_value_of_file(lily_file_val *);
lily_value *lily_new_value_of_hash(lily_hash_val *);
lily_value *lily_new_value_of_instance(uint16_t, lily_instance_val *);
lily_value *lily_new_value_of_integer(int64_t);
lily_value *lily_new_value_of_list(lily_list_val *);
lily_value *lily_new_value_of_string(lily_string_val *);
lily_value *lily_new_value_of_string_lit(const char *);

/* These are the ids of the predefined variants of Option and Some. */
#define LILY_SOME_ID  14
#define LILY_NONE_ID  15

#define LILY_LEFT_ID  17
#define LILY_RIGHT_ID 18

/* Operations for specific kinds of values. */

/* ByteString operations */
char *lily_bytestring_get_raw(lily_string_val *);
int lily_bytestring_length(lily_string_val *);

/* Dynamic operations */
lily_dynamic_val *lily_new_dynamic_val(void);
DECLARE_BOTH(dynamic, lily_dynamic_val *)

/* File operations */
lily_file_val *lily_new_file_val(FILE *, const char *);
FILE *lily_file_get_raw(lily_file_val *);
void lily_file_ensure_readable(lily_state *, lily_file_val *);
void lily_file_ensure_writeable(lily_state *, lily_file_val *);

/* Function operations */
int lily_function_is_foreign(lily_function_val *);
int lily_function_is_native(lily_function_val *);

/* Instance operations */
lily_instance_val *lily_new_instance_val(int);
DECLARE_BOTH(instance, lily_instance_val *, int);

/* Hash operations */
lily_hash_val *lily_new_hash_numtable(void);
lily_hash_val *lily_new_hash_numtable_sized(int);
lily_hash_val *lily_new_hash_strtable(void);
lily_hash_val *lily_new_hash_strtable_sized(int);
lily_hash_val *lily_new_hash_like_sized(lily_hash_val *, int);
lily_value *lily_hash_find_value(lily_hash_val *, lily_value *);
void lily_hash_insert_value(lily_hash_val *, lily_value *, lily_value *);
void lily_hash_insert_str(lily_hash_val *, lily_string_val *, lily_value *);
int lily_hash_delete(lily_hash_val *, lily_value **, lily_value **);

/* List operations */
lily_list_val *lily_new_list_val_n(int);
DECLARE_BOTH(list, lily_list_val *, int)
int lily_list_num_values(lily_list_val *);

/* String operations */
lily_string_val *lily_new_raw_string(const char *);
lily_string_val *lily_new_raw_string_take(char *);
lily_string_val *lily_new_raw_string_sized(const char *, int);
char *lily_string_get_raw(lily_string_val *);
int lily_string_length(lily_string_val *);

/* Enum operations */
lily_instance_val *lily_new_enum_n(int);
DECLARE_BOTH(variant, lily_instance_val *, int)

/* Stack operations
   Note: Push operations are sourced from vm. */
lily_value *lily_pop_value(lily_state *);
void lily_drop_value(lily_state *);
DECLARE_SETTERS(push, lily_state *)

DECLARE_SETTERS(return, lily_state *)
void lily_return_value_noref(lily_state *, lily_value *);

/* Calling, and argument fetching */
void lily_prepare_call(lily_state *, lily_function_val *);
void lily_exec_prepared(lily_state *, int);
void lily_exec_simple(lily_state *, lily_function_val *, int);

int lily_arg_class_id(lily_state *, int);
int lily_arg_count(lily_state *);
int lily_arg_instance_for_id(lily_state *, int, lily_instance_val **);
void lily_result_return(lily_state *);

/* Result operations */
DECLARE_GETTERS(arg, lily_state *, int)
DECLARE_GETTERS(result, lily_state *)

/* General operations. Special care should be taken with these. */

void lily_deref(lily_value *);
void lily_assign_value(lily_value *, lily_value *);
void lily_assign_value_noref(lily_value *, lily_value *);
lily_value *lily_copy_value(lily_value *);
int lily_eq_value(lily_state *, lily_value *, lily_value *);
int lily_value_is_derefable(lily_value *);
uint16_t lily_value_class_id(lily_value *);

/* Raise an exception within the interpreter. */
void lily_DivisionByZeroError(lily_state *, const char *, ...);
void lily_IndexError(lily_state *, const char *, ...);
void lily_IOError(lily_state *, const char *, ...);
void lily_KeyError(lily_state *, const char *, ...);
void lily_RuntimeError(lily_state *, const char *, ...);
void lily_ValueError(lily_state *, const char *, ...);

/* Miscellaneous operations. These are only valid in a function extending the
   interpreter. If they are called outside of the interpreter's parsing loop,
   the interpreter is likely to crash. */

/* This flushes and provides the interpreter's msgbuf. Callers always flush
   first, so don't worry about flushing when done.
   The full struct, and methods for it are defined in lily_api_msgbuf.h */
struct lily_msgbuf_ *lily_get_msgbuf(lily_state *);

/* This provides the interpreter's msgbuf, but does not flush it. Most of the
   time, a caller should use the regular lily_get_msgbuf function, because
   callers are expected to flush msgbuf before use (instead of after). */
struct lily_msgbuf_ *lily_get_msgbuf_noflush(lily_state *);

/* If you are not adding a foreign class to Lily, then you can ignore this. This
   function fetches the class id table for the currently-entered function. From
   there, you can pick away the class ids.
   Class ids are stored, 0-indexed, in the same order as the top of your
   dynaload table.
   The class id provided is later used by Lily for printing and comparing
   instances of your class. */
uint16_t *lily_get_cid_table(lily_state *);

int lily_is_valid_utf8(const char *);

/* Call this if your function is acting as the constructor for a native class.
   If your class is being inherited from, it will provide the class and set the
   properties for you.
   If your class constructor is being called directly, it will create a new
   instance with the id and # of values provided.
   In both cases, the instance is setup for being returned. The function should
   then plug in the properties it needs to. */
void lily_ctor_setup(lily_state *, lily_instance_val **, uint16_t, int);

#endif