// WinControl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ContrlDB.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

struct data_chunk
{
	char m_data = 'q';
	data_chunk(char c) : m_data(c) {
	}
};

static bool more = true;
std::mutex mut;
std::mutex print_mut;
std::queue<data_chunk> data_queue;	// 用于线程间通信的队列 
std::condition_variable data_cond;

bool more_data_to_prepare()
{
	return more;
}

data_chunk prepare_data()
{
	char x = 'q';
	std::cin >> x;
	if (x == 'q')
	{
		more = false;
	}
	return data_chunk(x);
}

void process(data_chunk& data)
{
	std::lock_guard<mutex> PrintLock(print_mut);
	std::cout << "			process data: " << data.m_data << std::endl;
}

bool is_last_chunk(data_chunk& data)
{
	if (data.m_data == 'q')
	{
		return true;
	}

	return false;
}

void data_preparation_thread()
{
	while (more_data_to_prepare())
	{
		{
			std::lock_guard<mutex> PrintLock(print_mut);
			printf("preparation work thread(%d) start\n", ::GetCurrentThreadId());
		}

		data_chunk const data = prepare_data();
		{
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(data);
		}

		{
			std::lock_guard<mutex> PrintLock(print_mut);
			printf("preparation work thread(%d) wake up process worker thread\n", ::GetCurrentThreadId());
		}
		
		data_cond.notify_all();
	}
}

void data_processing_thread()
{
	while (true)
	{
		{
			std::lock_guard<mutex> PrintLock(print_mut);
			printf("		process work thread(%d) start to wait\n", ::GetCurrentThreadId());
		}
		
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, []{return !data_queue.empty(); });

		{
			std::lock_guard<mutex> PrintLock(print_mut);
			printf("		process work thread(%d) start to work\n", ::GetCurrentThreadId());
		}
		
		data_chunk data = data_queue.front();
		data_queue.pop();
		lk.unlock();

		Sleep(1000);
		process(data);
		if (is_last_chunk(data))
			break;
	}
}

int test()
{
	std::thread PrepareationWorkerThread(data_preparation_thread);
	std::thread arrProcessWorkerThreads[4];
	for (int i = 0; i < WORKERS_THREAD_NUM; i++)
	{
		arrProcessWorkerThreads[i] = std::thread(data_processing_thread);
	}

	PrepareationWorkerThread.join();
	for (auto &thread : arrProcessWorkerThreads)
	{
		thread.join();
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("GetCurrentProcessId() %d\n", ::GetCurrentProcessId());
	printf("GetCurrentThreadId() %d\n", ::GetCurrentThreadId());
	//ContrlDB db;
	//db.DBConnect();

	//ORDER_INFO_VEC vecOrderInfo;
	//db.Ex_ReadOrder(vecOrderInfo);
	test();
	//printf("vecOrderInfo size:%d\n", vecOrderInfo.size());
	system("pause");
	return 0;
}

