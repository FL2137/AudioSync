#pragma once
#include <mutex>
#include <condition_variable>


class Semaphore {
public:
	Semaphore(int _count = 0) {
		count = _count;
	}

	inline void raise() {
		std::unique_lock<std::mutex> lock(mutex);
		count++;
		var.notify_one();
	}

	inline void wait() {
		std::unique_lock<std::mutex> lock(mutex);
		while (count == 0) {
			var.wait(lock);
		}
		count--;
	}

	inline void setCount(int newCount) {
		count = newCount;
	}

private:
	std::condition_variable_any var;
	std::mutex mutex;
	int count;
};

