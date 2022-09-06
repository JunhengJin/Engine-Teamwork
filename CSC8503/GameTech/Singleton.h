#pragma once
#include <stddef.h>
#include <mutex>


template <class S>
class Singleton
{
public:
	//Provide global access to the only instance of this class
	static S* Instance()
	{
		//This if statement prevents the costly thread Lock-step being required each time the instance is requested
		if (!m_pInstance)
		{
			//Lock /is/ required here though, to prevent multiple threads initialising multiple instances
			// of the class when it turns out it has not been initialised yet
			std::lock_guard<std::mutex> lock(m_Constructed);

			//Check to see if a previous thread has already initialised an instance in the time it took
			// to acquire a lock.
			if (!m_pInstance)
			{
				m_pInstance = new S();
			}
		}
		return m_pInstance;
	}

	//Provide global access to release/delete this class
	static void Release()
	{
		//Technically this could have another enclosing if statement, but speed is much less of a problem as
		// this should only be called once in the entire program.
		std::lock_guard<std::mutex> lock(m_Constructed);
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

protected:
	//Only allow the class to be created and destroyed by itself
	Singleton() {}
	virtual ~Singleton() {}


private:
	//Prevent the class from being copied either by '=' operator or by copy constructor
	Singleton(Singleton const&) {}
	Singleton& operator=(Singleton const&) {}

	//Keep a static instance pointer to refer to as required by the rest of the program
	static std::mutex m_Constructed;
	static S* m_pInstance;
};

//Finally make sure that the instance is initialised to NULL at the start of the program
template <class S> std::mutex Singleton<S>::m_Constructed;
template <class S> S* Singleton<S>::m_pInstance = NULL;