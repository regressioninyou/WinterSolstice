#pragma once 
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <queue>
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
				std::unique_lock<std::mutex> lock(mutexx);
				Woker_Queue.push_back(info);
				Wake();
			}
			void AddTaskTop(const Himeko::WokerInfo<void()>& info) {
				std::unique_lock<std::mutex> lock(mutexx);
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
					if (occupy == true) {
						(*boundTask)();
						return boundTask->get_future();
					}
					std::unique_lock<std::mutex> lock(mutexx);
					Himeko::WokerInfo<void()> info;
					info.WokerFunction = [this, boundTask]() {(*boundTask)(); };
					info.ExecuteCount = 1;
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
					std::unique_lock<std::mutex> lock(mutexx);
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
			uint32_t GetWokerCount() {
				std::unique_lock<std::mutex> lock(mutexx);
				return Woker_Queue.size();
			}
			void Worker() {
				id = std::this_thread::get_id();
				while (alive) {
					//std::pair<int, std::function<void()>> fn = { 0,[]() {} };
					WokerInfo<void()> info = { []() {} ,"Defult Function Info",1 };
					woker_ss.acquire();
					{
						std::unique_lock<std::mutex> lock(mutexx);
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
							std::unique_lock<std::mutex> lock(mutexx);
							//worker_que.push_back(fn);
							Woker_Queue.push_back(info);
							Wake();
						}
					}
					occupy = false;
					//if (worker_que.empty()) woker_end = true;
					if (!alive && Woker_Queue.empty()) {
						if (!ansyc) await_relese.release();
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
			std::mutex mutexx;
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

				return worker_pool[worker_index]->AnsycTask<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
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
	}
}