#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <unistd.h>
// Output lab2

using namespace std;

 class SynchronizedQueue
{
	queue<string> queue_;
	mutex mutex_;
	condition_variable condvar_;

	typedef lock_guard<mutex> lock;
	typedef unique_lock<mutex> ulock;

	void push(string const& val)
	{
		{
			ulock u(mutex_);
			while (!queue_.empty())
				condvar_.wait(u);
		}

		lock l(mutex_);

		queue_.push(val);
		cout << "Send message: " << val << "\n";

		condvar_.notify_one();
	}

public:
	bool work = true;

	void producer(string msg)
	{
		push(msg);
	}

	void pop()
	{
		{
			ulock u(mutex_);
			while (queue_.empty())
				condvar_.wait(u);
		}
		mutex_.lock();

		string retval = queue_.front();
		cout << "Got massage: " << retval << "\n";
		queue_.pop();

		mutex_.unlock();
		condvar_.notify_one();
	}
};

 void consumer(SynchronizedQueue &sync)
 {
	 while (sync.work)
	 {
		 sync.pop();
	 }
	 cout << "Exit consumer" << "\n";
 }

int main()
{
	SynchronizedQueue sync;

	thread thr(consumer, ref(sync));

	for (int i = 0; i <= 10; i++)
	{
		usleep(1000000);
		string msg = to_string(i);
		sync.producer(msg);
	}

	usleep(1000000);
	sync.work = false;
	sync.producer("exit");

	thr.join();
	return 0;
}
