//#include "type.h"
#include "hpet.hpp"
#include <list>
#include <mutex>
#include <condition_variable>    // std::condition_variable
namespace LPPCDEVICE
{
	template<typename T >
	class TaskQueue
	{
		using HandlFun = std::function<void(T&)>;
		class TaskInfo
		{
		public:
			TaskInfo(T t, uint64_t nDealy, HandlFun f) : m_nDelayTime(nDealy), m_t(t), m_fun(f)
			{
				m_hpetTimer.start();
			};
		public:
			bool checkTimer()
			{
				bool ret = false;
				if (m_hpetTimer.getDuration() > (long long)(m_nDelayTime))
				{
					ret = true;
				}
				return ret;
			};
			T				m_t;
			uint64_t		m_nDelayTime;
			HandlFun		m_fun;
			HPETTimer		m_hpetTimer;

		};
		using TaskList = std::list<shared_ptr<TaskInfo>>;
	public:
		TaskQueue() = default;
		~TaskQueue() = default;
		void initTaskQueue();
		void stopTaskQueue() { m_bTaskStart = false; };
		void addTask(T t, uint64_t nDelayTime, HandlFun f);
		void program();
	private:
		TaskList	m_TaskList;
		std::mutex		m_mtx;
		std::condition_variable m_cv;
		bool		m_bTaskStart;
		bool		m_bLocked{ false };
	};

	template<typename T>
	void TaskQueue<T>::addTask(T t, uint64_t nDelayTime, HandlFun f)
	{
		m_TaskList.push_back(make_shared<TaskInfo>(t, nDelayTime, f));
		if (m_bLocked)
		{
			std::unique_lock <std::mutex> lck(m_mtx);
			m_cv.notify_one();
		}
	}

	template<typename T>
	void TaskQueue<T>::program()
	{
		m_TaskList.remove_if([](shared_ptr<TaskInfo> iter) {    
			bool ret = false;
			if (iter->checkTimer())
			{
				iter->m_fun(iter->m_t);
				ret = true;
			}
			return ret;
		});
	}

	template<typename T>
	void TaskQueue<T>::initTaskQueue()
	{
		m_bTaskStart = true;
		std::thread queueThread([&]()
		{
			while (m_bTaskStart)
			{
				if (m_TaskList.empty())
				{
					m_bLocked = true;
					std::unique_lock <std::mutex> lck(m_mtx);
					m_cv.wait(lck);
				}
				program();
				//std::this_thread::sleep_for(std::chrono::microseconds(50));
			}
		});
		queueThread.detach();
	}
}

