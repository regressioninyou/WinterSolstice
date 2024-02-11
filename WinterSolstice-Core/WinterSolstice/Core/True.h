#pragma once 
#include <thread>
#include <atomic>
#include <semaphore>
#include <condition_variable>
#include <unordered_map>
namespace WinterSolstice {
	class True
	{
	public:
		void PlumeLock() {
			if (!CheckThread())
			{
				wait.acquire();
				Map[std::this_thread::get_id()] = true;
			}
		}
		void PlumeUnLock() {
			wait.release();
			Map[std::this_thread::get_id()] = false;
		}

		void StaffLock() {
			Elysia.lock();
			//	//wait.acquire();
		}
		void StaffunLock() {
			//	//wait.release();
			Elysia.unlock();
		}

		operator std::mutex& () {
			return Elysia;
		}


		std::counting_semaphore<1> wait{ 1 };
		static std::mutex Elysia;
	private:
		bool CheckThread() {
			MapWait.acquire();
			auto id = std::this_thread::get_id();
			if (Map.find(id) == Map.end()) {
				Map[id] = true;
				MapWait.release();
				return false;
			}
			MapWait.release();
			return Map[id];
		}
		std::counting_semaphore<1> MapWait{ 1 };
		std::unordered_map<std::thread::id, bool> Map;
	};
}