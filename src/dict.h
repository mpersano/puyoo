#ifndef DICT_H_
#define DICT_H_

#include <string.h>

template <typename T>
class dict
{
public:
	dict()
	{
		::memset(buckets_, 0, sizeof buckets_);
	}

	~dict()
	{
		for (size_t i = 0; i < NUM_BUCKETS; i++) {
			if (buckets_[i])
				delete buckets_[i];
		}
	}

	// N.B.: ownership of `value' is passed to dict
	void put(const char *key, T *value)
	{
		size_t index = hash(key)%NUM_BUCKETS;
		buckets_[index] = new entry(key, value, buckets_[index]);
	}

	T *get(const char *key)
	{
		return const_cast<T *>(static_cast<const dict<T> *>(this)->get(key));
	}

	const T *get(const char *key) const
	{
		for (entry *p = buckets_[hash(key)%NUM_BUCKETS]; p; p = p->next_) {
			if (!::strcmp(p->key_, key))
				return p->value_;
		}
		return 0;
	}

private:
	unsigned hash(const char *key) const
	{
		unsigned h = 0;
		for (const char *p = key; *p; p++)
			h = (h << 4) + static_cast<unsigned>(*p);
		return h;
	}

	struct entry {
		entry(const char *key, T *value, entry *next)
		: key_(new char[::strlen(key) + 1])
		, value_(value)
		, next_(next)
		{ ::strcpy(key_, key); }

		~entry()
		{
			delete[] key_;
			delete value_;
			if (next_) delete next_;
		}

		char *key_;
		T *value_;
		entry *next_;
	};

	enum { NUM_BUCKETS = 311 };
	entry *buckets_[NUM_BUCKETS];

	dict(const dict&); // disable copy ctor
	dict& operator=(const dict&); // disable assignment
};

#endif // DICT_H_
