#include "intrinsics.h"
#include <algorithm>
#include "../defs.h"
#include "bucket_queue.h"


int main(int argc, char* argv[]) {
	rw_bucket::bucket_queue<int> bq(1000, 100, 100);
	for (int i = 0; i < 1000; i++) {
		bq.add_elem(i, i);
	}

	int s;
	while(1) {
		int * elems = bq.get_bucket(&s);
		if (elems) {
			std::cout << "Size = " << s << std::endl;
		} else
			break;
	}

}
