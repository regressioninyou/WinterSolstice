#pragma once 
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <queue>
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <semaphore>
#include <condition_variable>
#include "WinterSolstice/Core/StructBase.h"
#define Loop -1024
namespace WinterSolstice {
	namespace Himeko {
		class BaseThread {
		public:
			BaseThread(bool detach = true) :alive(true), ansyc(!detach) {
				if (detach)
					std::thread(&BaseThread::Worker, this).detach();
			}
			~BaseThread() {
				kill();
			}
			void AddTask(std::function<void()> task, int executeCount = 1) {
				//std::unique_lock<std::mutex> lock(mutexx);
				//worker_que.push_back(std::pair{ executeCount,task });
				//Wake();
				Himeko::WokerInfo<void()> ortho;
				ortho.ExecuteCount = executeCount;
				ortho.WokerFunction = std::move(task);
				AddTask(ortho);
			}
			void AddTaskTop(std::function<void()> task, int executeCount = 1) {
				//std::unique_lock<std::mutex> lock(mutexx);
				//worker_que.push_front(std::pair{ executeCount,task });
				//Wake();
				Himeko::WokerInfo<void()> ortho;
				ortho.ExecuteCount = executeCount;
				ortho.WokerFunction = std::move(task);
				AddTaskTop(ortho);
			}
			void AddTask(const Himeko::WokerInfo<void()>& info) {
				if (std::this_thread::get_id() == id)
				{
					try {
						info.WokerFunction();
					}
					catch (...) {
						Kiana_CORE_WARN("BaseThread::AddTask: Caught exception in worker thread {0}", info.FunctionInfo);
					}
					return;
				}
				std::lock_guard<std::recursive_mutex> lock(mutexx);
				Woker_Queue.push_back(info);
				Wake();
			}
			void AddTaskTop(const Himeko::WokerInfo<void()>& info) {
				std::lock_guard<std::recursive_mutex> lock(mutexx);
				Woker_Queue.push_front(info);
				Wake();
			}
			void AddTaskAwait(std::function<void()> task) {
				if (std::this_thread::get_id() == this->id)
				{
					try
					{
						task();
					}
					catch (const std::exception& ex)
					{
						auto error = ex.what();
						Kiana_CORE_ERROR(error);
					}
					return;
				}
				std::counting_semaphore<1> l{ 0 };

				AddTask([&]()
					{
						task();
						l.release();
					});
				l.acquire();
			}
			void AddTaskAwait(const Himeko::WokerInfo<void()>& info) {
				if (std::this_thread::get_id() == this->id)
				{
					try
					{
						info.WokerFunction();
					}
					catch (const std::exception& ex)
					{
						auto error = ex.what();
						Kiana_CORE_ERROR(error);
					}
					return;
				}
				std::counting_semaphore<1> l{ 0 };

				AddTask([&]()
					{
						info.WokerFunction();
						l.release();
					});
				l.acquire();
			}
			void AddTaskAwaitTop(std::function<void()> task) {
				if (std::this_thread::get_id() == this->id)
				{
					try
					{
						task();
					}
					catch (const std::exception& ex)
					{
						auto error = ex.what();
						Kiana_CORE_ERROR(error);
					}
					return;
				}
				std::counting_semaphore<1> l{ 0 };

				AddTaskTop([&]()
					{
						task();
						l.release();
					});
				l.acquire();
			}
			template <class F, class... Args>
			auto AnsycTask(F&& f, Args &&... args) -> std::future<decltype(f(args...))> {
				using return_type = decltype(f(args...));

				auto boundTask = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

				{
					Himeko::WokerInfo<void()> info;
					info.WokerFunction = [this, boundTask]() {(*boundTask)(); };
					info.ExecuteCount = 1;
					if (occupy == true) {
						info.WokerFunction();
						std::lock_guard<std::recursive_mutex> lock(mutexx);
						info.WokerFunction = [boundTask]() {};
						Wake();
						return std::move(boundTask->get_future());
					}
					std::lock_guard<std::recursive_mutex> lock(mutexx);
					Woker_Queue.push_back(info);
					//AnsycTaskInfo()
					//worker_que.push_back(std::pair{ 1,[this, boundTask]() {(*boundTask)(); } });
					Wake();
				}
				return boundTask->get_future();
			}
			template <class F, class... Args>
			auto AnsycTaskShared(F&& f, Args &&... args) -> std::shared_ptr<std::packaged_task<decltype(f(args...))()>> {
				using return_type = decltype(f(args...));

				auto boundTask = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

				{
					std::lock_guard<std::recursive_mutex> lock(mutexx);
					Himeko::WokerInfo<void()> info;
					info.WokerFunction = [this, boundTask]() {(*boundTask)(); };
					info.ExecuteCount = 1;
					Woker_Queue.push_back(info);
					Wake();
				}
				return boundTask;
			}
			template <class F, class... Args>
			auto AnsycTaskInfo(const Himeko::WokerInfo<F>& info, Args &&... args) -> std::future<decltype(info.WokerFunction(args...))> {
				using return_type = decltype(info.WokerFunction(args...));

				auto boundTask = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<std::function<F>>(info.WokerFunction), std::forward<Args>(args)...));

				{
					if (occupy == true) {
						(*boundTask)();
						return boundTask->get_future();
					}
					std::unique_lock<std::mutex> lock(mutexx);
					//info.WokerFunction = std::move();
					Himeko::WokerInfo<void()> ortho{ [this, boundTask]() {(*boundTask)(); } ,info.FunctionInfo,info.ExecuteCount };
					Woker_Queue.push_back(ortho);
					Wake();
				}
				return boundTask->get_future();
			}

			void Wake() {
				woker_ss.release();
			}
			bool kill() {
				alive = false;
				Wake();
				if (!ansyc)
					await_relese.acquire();
				return Woker_Queue.empty();
			}
			bool IsCurrentThread() {
				return id == std::this_thread::get_id();
			}
			uint32_t GetWokerCount() {
				std::lock_guard<std::recursive_mutex> lock(mutexx);
				return Woker_Queue.size();
			}
			void Worker() {
				id = std::this_thread::get_id();
				while (alive) {
					//std::pair<int, std::function<void()>> fn = { 0,[]() {} };
					WokerInfo<void()> info = { []() {} ,"Defult Function Info",1 };
					woker_ss.acquire();
					{
						std::lock_guard<std::recursive_mutex> lock(mutexx);
						if (!Woker_Queue.empty())
						{
							occupy = true;
							//WokerState.acquire();
							//fn = std::move(worker_que.front());
							info = std::move(Woker_Queue.front());
							Woker_Queue.pop_front();
						}
					}
					try
					{
						//fn.second();
						info.WokerFunction();
					}
					catch (const std::exception& e)
					{
						std::cout << info.FunctionInfo << e.what() << "\n";
						Kiana_CORE_WARN("Runtime {0} Error:{1}", info.FunctionInfo, e.what());
					}
					{
						if (alive && (info.ExecuteCount == Loop || --info.ExecuteCount > 0)) {
							std::lock_guard<std::recursive_mutex> lock(mutexx);
							//worker_que.push_back(fn);
							Woker_Queue.push_back(info);
							Wake();
						}
					}
					occupy = false;
					//if (worker_que.empty()) woker_end = true;
					if (!alive && Woker_Queue.empty()) {
						if (!ansyc)
							await_relese.release();
						return;
					}
				}
#if 0
				while (ansyc) {
					std::pair<int, std::function<void()>> fn = { 0,[]() {} };
					woker_ss.acquire();
					if (!worker_que.empty())
					{
						occupy = true;
						fn = std::move(worker_que.front());
						worker_que.pop_front();
					}
					try
					{
						fn.second();
					}
					catch (const std::exception& e)
					{
						Kiana_CORE_WARN("Runtime Error:", e.what());
					}
					occupy = false;
					if (!alive && worker_que.empty()) {
						//await_relese.release();
						return;
					}
					{
						if (alive && (fn.first == Loop || --fn.first > 0)) {
							std::unique_lock<std::mutex> lock(mutexx);
							worker_que.push_back(fn);
							Wake();
						}
					}
				}

#endif // 0

			}
		private:
			bool ansyc = false;
			bool woker_end = true;
			bool alive;
			bool occupy = false;
			std::thread::id id;
			std::deque<WokerInfo<void()>> Woker_Queue;
			std::counting_semaphore<1> await_relese{ 0 };
			std::counting_semaphore<64> woker_ss{ 0 };
			std::recursive_mutex mutexx;
			std::counting_semaphore<1> WokerState{ 1 };
		};

		class ThreadPool {
		public:
			ThreadPool(short numThreads) :nowThread(numThreads) {
				if (nowThread > maxThread) {
					Kiana_CORE_WARN("Runtime Thread Max size Out of Range must size > 16! you set size :", numThreads);
					nowThread = maxThread;
				}
				for (short i = 0; i < numThreads; i++)
				{
					worker_pool.emplace_back(new BaseThread());
				}
			}

			~ThreadPool() {
				for (auto& th : worker_pool)
				{
					delete th;
				}
				worker_pool.clear();
			}
			void AddTask(std::function<void()> fn) {
				auto worker_index = this->round_robin.fetch_add(1) % this->nowThread;
				worker_pool[worker_index]->AddTask(fn);
			}
			void AddTask(std::function<void()> fn, int Count) {
				auto worker_index = this->round_robin.fetch_add(1) % this->nowThread;
				worker_pool[worker_index]->AddTask(fn, Count);
			}
			void AddAwaitTask(std::function<void()> fn) {
				auto worker_index = this->round_robin.fetch_add(1) % this->nowThread;
				worker_pool[worker_index]->AddTaskAwait(fn);
			}
			template <class F, class... Args>
			auto AnsycTask(F&& f, Args &&... args) -> std::future<decltype(f(args...))> {

				auto worker_index = this->round_robin.fetch_add(1) % this->nowThread;

				return std::forward<std::future<decltype(f(args...))>>(worker_pool[worker_index]->AnsycTask<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
#if  0
				std::pair<uint32_t, BaseThread*> pair = { worker_pool[0]->GetWokerCount(),worker_pool[0] };
				for (size_t i = 1; i < worker_pool.size(); i++) {
					if (pair.first > worker_pool[i]->GetWokerCount()) {
						pair.first = worker_pool[i]->GetWokerCount();
						pair.second = worker_pool[i];
					}
				}
				return pair.second->AnsycTask<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
#endif //  0
			}
			template <class F, class... Args>
			auto AnsycTaskShared(F&& f, Args &&... args) -> std::shared_ptr<std::packaged_task<decltype(f(args...))()>> {

				auto worker_index = this->round_robin.fetch_add(1) % this->nowThread;

				return worker_pool[worker_index]->AnsycTaskShared<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
			}
			template <class F, class... Args>
			auto AnsycTaskBefor(uint32_t begin, uint32_t end, F&& f, Args &&... args) -> std::future<decltype(f(args...))> {
				if (end > nowThread || begin > nowThread || end < 0 || begin < 0) {
					if (nowThread < maxThread) {
						end = nowThread + 1; begin = nowThread + 1;
						worker_pool.push_back(new BaseThread());
					}
					else
					{
						throw std::runtime_error("Thread out of range");
					}
				}
				auto worker_index = this->round_robin.fetch_add(1) % (this->nowThread - begin);

				return worker_pool[worker_index + begin]->AnsycTask<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
#if 0
				std::pair<uint32_t, BaseThread*> pair = { worker_pool[begin]->GetWokerCount(),worker_pool[begin] };
				for (size_t i = begin; i < end; i++) {
					if (pair.first > worker_pool[i]->GetWokerCount()) {
						pair.first = worker_pool[i]->GetWokerCount();
						pair.second = worker_pool[i];
					}
				}
				return pair.second->AnsycTask<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
#endif // 0
			}
		private:
			std::atomic<int> round_robin;
			std::vector<BaseThread*> worker_pool;
			short maxThread = 16;
			short nowThread;
		};

		class SynchronizeThread {
		public:
			SynchronizeThread(bool detach = true) :alive(true), Sychronize(detach)
			{
				if (detach)
					std::thread(&SynchronizeThread::Run, this).detach();
				MinimizeRun = []() {};
			}
			~SynchronizeThread() {}

			void Begin()
			{
				for (auto begin : Final_Woker_Begin) {
					//Queue_Woker.push_back(begin);
					begin.WokerFunction();
				}
				SetStart(true);
			}
			void End()
			{
				for (auto end : Final_Woker_End)
					//Queue_Woker.push_back(end);
					end.WokerFunction();
				SetStart(false);
			}
			void AddMinimizedTask(std::function<void()> fn) {
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = fn;
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				Queue_Woker.push_back(info);
			}

			void AddTask(std::function<void()> task) {
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				AddTaskInfo(info);
			}
			void AddTaskInfo(Himeko::WokerInfo<void()> info) {
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				Queue_Woker.push_back(info);
				ReleaseOne();
			}
			void AddTaskTop(std::function<void()> task) {
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				AddTaskInfo(info);
			}
			void AddTaskTopInfo(Himeko::WokerInfo<void()> info) {
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				Queue_Woker.push_front(info);
				ReleaseOne();
			}
			void AddTaskAwait(std::function<void()> task) {
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				AddTaskAwaitInfo(info);
			}
			void AddTaskAwaitInfo(const Himeko::WokerInfo<void()>& info) {
				if (std::this_thread::get_id() == this->id)
				{
					try
					{
						info.WokerFunction();
					}
					catch (const std::exception& ex)
					{
						auto error = ex.what();
						Kiana_CORE_ERROR(error);
					}
					return;
				}
				std::counting_semaphore<1> l{ 0 };

				AddTask([&]()
					{
						info.WokerFunction();
						l.release();
					});
				l.acquire();
			}
			void AddTaskAwaitTop(std::function<void()> task) {
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				AddTaskAwaitTopInfo(info);
			}
			void AddTaskAwaitTopInfo(const Himeko::WokerInfo<void()>& info) {
				if (std::this_thread::get_id() == this->id)
				{
					try
					{
						info.WokerFunction();
					}
					catch (const std::exception& ex)
					{
						auto error = ex.what();
						Kiana_CORE_ERROR(error);
					}
					return;
				}
				std::counting_semaphore<1> l{ 0 };

				AddTaskTop([&]()
					{
						info.WokerFunction();
						l.release();
					});
				l.acquire();
			}

			template <class F, class... Args>
			auto AnsycTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
				using return_type = decltype(f(std::forward<Args>(args)...));

				// 创建一个包含任务函数和参数的 Himeko::WokerInfo 对象
				Himeko::WokerInfo<return_type()> info;
				info.WokerFunction = std::forward<F>(f);

				// 调用 AnsycTaskInfo 函数，并将任务信息和参数传递给它
				return AnsycTaskInfo(info, std::forward<Args>(args)...);
			}

			template <class F, class... Args>
			auto AnsycTaskInfo(const Himeko::WokerInfo<F>& info, Args&&... args) -> std::future<decltype(info.WokerFunction(std::forward<Args>(args)...))> {
				using return_type = decltype(info.WokerFunction(std::forward<Args>(args)...));

				// 创建一个包装任务，将任务函数和参数绑定
				auto boundTask = std::make_shared<std::packaged_task<return_type()>>(std::bind(info.WokerFunction, std::forward<Args>(args)...));

				{
					// 加锁保护共享资源
					std::lock_guard<std::recursive_mutex> lock(Queue_mutex);

					// 将绑定的任务添加到任务队列中
					Himeko::WokerInfo<void()> ortho;
					ortho.WokerFunction = [this, boundTask]() { (*boundTask)(); };
					ortho.ExecuteCount = info.ExecuteCount;
					ortho.FunctionInfo = info.FunctionInfo;
					Queue_Woker.push_back(ortho);

					// 释放一个信号量，表示有新的任务加入队列
					ReleaseOne();
				}

				// 返回与任务关联的 std::future 对象
				return boundTask->get_future();
			}

			void ReleaseOne() {
				woker.release();
			}
			void FinalTaskBegin(std::function<void()> task)
			{
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				Final_Woker_Begin.push_back(info);
			}
			void FinalTaskEnd(std::function<void()> task)
			{
				Himeko::WokerInfo<void()> info;
				info.WokerFunction = task;
				Final_Woker_End.push_back(info);
			}
			void FrameBegin() {
				frame.release();
			}
			void FrameEnd() {
				frame.acquire();
			}
			bool TryFrameAcquire() {
				return frame.try_acquire();
			}
			void ExecuteWokerQueue() {
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				while (!Queue_Woker.empty()) {
					Queue_Woker.back().WokerFunction();
					Queue_Woker.pop_back();
				}
			}
			//bool isAlive() {};
			void Kill() {
				alive = false;
				SetExecute(false);
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				std::vector<WokerInfo<void()>> kill;
				for (auto& que : Queue_Woker) {
					kill.push_back(que);
				}
				Queue_Woker.clear();
				for (auto& que : kill) {
					que.WokerFunction();
				}
			}
			void SetExecute(bool set) {
				std::shared_lock<std::shared_mutex> lock(Execute_mutex);
				Execute = set;
			}
			bool GetExecute() {
				std::shared_lock<std::shared_mutex> lock(Execute_mutex);
				return Execute;
			}

			bool GetStart() {
				return start;
			}
			void SetStart(bool set) {
				if (std::this_thread::get_id() != id)
					std::unique_lock<std::shared_mutex> lock(RWMutexStart); // 获取写入锁
				start = set;
			}

			bool GetMinimized() {
				return Minimized;
			}
			void SetMinimized(bool value) {
				if (std::this_thread::get_id() != id)
					std::unique_lock<std::shared_mutex> lock(RWMutexMinimized); // 获取写入锁
				Minimized = value;
			}
			void ClearQueue_Woker() {
				std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
				Queue_Woker.clear();
			}
			void Run()
			{
				id = std::this_thread::get_id();
				while (alive)
				{
					if (!GetMinimized()) {
						frame.acquire();
						Begin();
						while (!Queue_Woker.empty() || GetExecute()) {
							Himeko::WokerInfo<void()> info;
							info.WokerFunction = []() {};
							woker.acquire();
							{
								std::lock_guard<std::recursive_mutex> lock(Queue_mutex);
								info = std::move(Queue_Woker.back());
								Queue_Woker.pop_back();
							}
							try
							{
								info.WokerFunction();
							}
							catch (const std::exception&)
							{
								Kiana_CORE_WARN("Runtime Error:{0}", info.FunctionInfo);
							}
						}
						End();
					}
					else {
						MinimizeRun();
					}
				}
			}
			void SetMinimizeRun(std::function<void()> task) {
				MinimizeRun = std::move(task);
			}
		private:
			std::thread::id id;
			bool alive;
			bool start = false;
			bool Execute = false;
			bool Minimized = false;
			bool Sychronize = false;
			std::function<void()> MinimizeRun;
			std::shared_mutex RWMutexStart;
			std::shared_mutex Execute_mutex;
			std::shared_mutex RWMutexMinimized;
			std::recursive_mutex Queue_mutex;
			std::counting_semaphore<4> frame{ 0 };
			std::counting_semaphore<64> woker{ 0 };
			std::deque<WokerInfo<void()>> Queue_Woker;
			std::vector<WokerInfo<void()>> Final_Woker_Begin;
			std::vector<WokerInfo<void()>> Final_Woker_End;
		};
	}
}