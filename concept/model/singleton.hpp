#pragma once
#include "concept/base/noncopyable.h"
#include "concept/error/errorbase.hpp"
#include "concept/base/string_format.hpp"
#include <mutex>
namespace model
{
	enum class singletonMode
	{
		donot_care_order = 0,
		order,
		thread_only,
	};


	template<class T>
	class singleton_base :public base::noncopyable
	{
	public:
		template<typename... Args>
		static T* getInstance(Args... args)
		{
			static T t(args...);
			return &t;
		}
	};
	

	template<class T>
	class thread_singleton_base
	{
	public:
		template<typename... Args>
		static T* getInstance(Args... args)
		{
			thread_local static T t(args...);
			return &t;
		}
	};
	template<class T>
	class thread_singleton :public thread_singleton_base<T> {};


	/*
	根据顺序在销毁的时候销毁
	先调用createInstance的单例最后才会释放，这样可以保证使用多个单例
	单例之间如果互相调用，不能确保被其他单例使用的单例是最后一个释放的单例
	使用这个则可以确定最后一个释放的只要在最早的时候调用即可
	https://www.cnblogs.com/cthon/p/9196723.html
	*/
	template<class T>
	class singleton_order_base :public base::noncopyable
	{
	private:
		static void destoryInstance()
		{
			if (kInstance) 
			{
				auto ptr = kInstance;
				kInstance = nullptr;
				delete ptr;
			}
		}

	public:
		template<typename... Args>
		static auto getInstance()
		{
			std::call_once(connection_init_flag, []() {
				kInstance = new T();
				atexit(singleton_order_base<T>::destoryInstance);
			});
			return kInstance;
		}
	private:
		static T* kInstance;
		static std::once_flag connection_init_flag;
	};
	template<class T>
	T* singleton_order_base<T>::kInstance = nullptr;
	template<class T>
	std::once_flag singleton_order_base<T>::connection_init_flag;

	template<class T, singletonMode mode = singletonMode::order>
	class singleton;


	template<class T>
	class singleton<T, singletonMode::donot_care_order> :public singleton_base<T> {};

	template<class T>
	class singleton<T, singletonMode::order> :public singleton_order_base<T> {};

	template<class T>
	class singleton<T, singletonMode::thread_only> :public thread_singleton_base<T> {};
}