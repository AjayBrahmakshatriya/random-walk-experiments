#ifndef _RW_BUCKET_QUEUE_H
#define _RW_BUCKET_QUEUE_H

#include "intrinsics.h"
#include <assert.h>

namespace rw_bucket {

template <class ET>
inline ET writeAddRet(ET *a, ET b) {
  volatile ET newV, oldV; 
  do {oldV = *a; newV = oldV + b;}
  while (!CAS(a, oldV, newV));
  return oldV;
}

template <typename ET>
struct bucket {
	ET* elems;
	int top;	
	int max_size;

	bucket() = delete;
	bucket(const bucket&) = delete;
	bucket(bucket&) = delete;
	bucket(bucket&&) = delete;
	void operator=(const bucket&) = delete;

	bucket(int m) {
		max_size = m;
		elems = new ET[m]();
		top = 0;
	}
	~bucket() {
		delete[] elems;
	}
	
	void push(const ET &e) {
		int pos = writeAddRet(&top, 1);
		assert(pos < max_size);

		elems[pos] = e;
	}		
	
};

template <typename ET>
struct bucket_queue {
	bucket<ET> *buckets;
	//std::vector<bucket<ET>> buckets;
	bucket<ET> current;
	int *active_indices;
	int indices_size;	
	int *active_flags;
	
	int max_range;
	int bucket_range;
	int max_buckets;	
	
	bucket_queue(int mr, int br, int bs): current(bs) {
		max_range = mr;
		bucket_range = br;	
		max_buckets = (mr + br - 1) / br;	
		std::cout << "Max buckets = " << max_buckets << std::endl;
		//buckets = new bucket<ET>[max_buckets](bs);	
		buckets = static_cast<bucket<ET>*> (operator new[] (max_buckets * sizeof (bucket<ET>)));

		for (size_t i = 0; i < max_buckets; i++) {
			new (buckets + i) bucket<ET>(bs);
		}

		active_indices = new int[max_buckets]();
		active_flags = new int[max_buckets]();
		indices_size = 0;	
	}
	~bucket_queue() {
		for (size_t i = max_buckets; i > 0; i--) {
			buckets[i-1].~bucket<ET>();
		}
		operator delete[] (static_cast<void*>(buckets));
		//operator delete[] (static_cast<void*>(buckets));
		delete[] active_indices;
		delete[] active_flags;
	}

	int idx_to_bucket(int idx) {
		return idx / bucket_range;
	}

	ET* get_bucket(int* size) {
		if (indices_size == 0) {
			*size = 0;
			return NULL;
		}	

		indices_size--;
		int ret_idx = active_indices[indices_size];
		active_flags[ret_idx] = 0;
		
		bucket<ET>* to_ret = buckets + ret_idx;

		ET* f = to_ret->elems;
		*size = to_ret->top;
		
		to_ret->elems = current.elems;
		to_ret->top = 0;
		current.elems = f;
		current.top = *size;
		
		return f;	
	}	
	void ensure_id_active(int bid) {
		bool changed = CAS(active_flags + bid, 0, 1);
		if (changed) {
			int pos = writeAddRet(&indices_size, 1);
			active_indices[pos] = bid;
		}

	}	
	void add_elem(const ET& e, int idx) {
		int bid = idx_to_bucket(idx);
		buckets[bid].push(e);	
		ensure_id_active(bid);	
	}
};


}

#endif
