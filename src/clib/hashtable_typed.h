/* Extends hashtable.h with a macro to create typed hashtable for additional safety. */
#ifndef HASHTABLE_TYPED_H
#define HASHTABLE_TYPED_H
#include "hashtable.h"

#define HASHTABLE_REGISTER_TYPE(name, key_type, value_type, hash_func, equals_func)										\
	typedef struct { hashtable h; } hashtable_ ## name;																	\
																														\
	typedef struct { hashtable_iterator h; } hashtable_iterator_ ## name;												\
																														\
	typedef struct { key_type key; value_type value; } hashtable_entry_ ## name;										\
																														\
	inline static uint64_t hashtable_hash_ ## name(key_type *key) {														\
		return hash_func(*key);																							\
	} 																													\
																														\
	inline static bool hashtable_equals_ ## name(key_type *a, key_type *b) {											\
		return equals_func(*a, *b);																						\
	} 																													\
																														\
	inline static bool hashtable_create_ ## name(hashtable_ ## name *ht, size_t capacity) {								\
		return hashtable_create(&ht->h, capacity, capacity * sizeof(key_type), capacity * sizeof(value_type));			\
	}																													\
																														\
	inline static void hashtable_destroy_ ## name(hashtable_ ## name *ht) {												\
		hashtable_destroy(&ht->h);																						\
	}																													\
																														\
	inline static size_t hashtable_bucket_count_ ## name(hashtable_ ## name *ht) {										\
		return hashtable_bucket_count(&ht->h);																			\
	}																													\
																														\
	inline static bool hashtable_put_ ## name(hashtable_ ## name *ht, key_type key, value_type value) {					\
		return hashtable_put(																							\
			&ht->h,																										\
			(uint64_t (*)(void *)) hashtable_hash_ ## name,																\
			(bool (*)(void *, void *)) hashtable_equals_ ## name,														\
			&key,																										\
			sizeof(key_type),																							\
			&value,																										\
			sizeof(value_type));																						\
	}																													\
																														\
	inline static bool hashtable_remove_ ## name(hashtable_ ## name *ht, key_type key) {								\
		return hashtable_remove(																						\
			&ht->h,																										\
			(uint64_t (*)(void *)) hashtable_hash_ ## name,																\
			(bool (*)(void *, void *)) hashtable_equals_ ## name,														\
			&key,																										\
			sizeof(key_type),																							\
			sizeof(value_type));																						\
	}																													\
																														\
	inline static hashtable_entry_ ## name hashtable_get_entry_ ## name(hashtable_ ## name *ht, key_type key) {			\
		uint8_t * ue = hashtable_get_entry(																				\
			&ht->h,																										\
			(uint64_t (*)(void *)) hashtable_hash_ ## name,																\
			(bool (*)(void *, void *)) hashtable_equals_ ## name,														\
			&key,																										\
			sizeof(key_type),																							\
			sizeof(value_type));																						\
		hashtable_entry_ ## name e;																						\
		e.key = *(key_type *) ue;																						\
		e.value = *((value_type *) ue + sizeof(value_type));															\
		return e;																										\
	}																											\
																														\
	inline static value_type * hashtable_get_ ## name(hashtable_ ## name *ht, key_type key) {								\
		return (value_type *) hashtable_get(																			\
			&ht->h,																										\
			(uint64_t (*)(void *)) hashtable_hash_ ## name,																\
			(bool (*)(void *, void *)) hashtable_equals_ ## name,														\
			&key,																										\
			sizeof(key_type),																							\
			sizeof(value_type));																						\
	}																													\
																														\
	inline static hashtable_iterator_ ## name hashtable_iterate_ ## name(hashtable_ ## name *ht) {						\
		hashtable_iterator_ ## name it;																					\
		it.h = hashtable_iterate(&ht->h);																				\
		return it;																										\
	}																													\
																														\
	inline static bool hashtable_iterate_next_ ## name(hashtable_iterator_ ## name *it) {								\
		return hashtable_iterate_next(&it->h, sizeof(key_type), sizeof(value_type));									\
	}																													\
																														\
	inline static key_type * hashtable_iterate_key_ ## name(hashtable_iterator_ ## name *it) {							\
		return (key_type *) hashtable_iterate_key(&it->h);																\
	}																													\
																														\
	inline static value_type * hashtable_iterate_value_ ## name(hashtable_iterator_ ## name *it) {						\
		return (value_type *) hashtable_iterate_value(&it->h, sizeof(key_type));										\
	}

#endif