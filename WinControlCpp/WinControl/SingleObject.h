#pragma once

template<typename T>
class CSingleObject
{
public:
	CSingleObject(void){}
	~CSingleObject(void){}

	static T& Instance()
	{
		if (m_pobj == 0)
			m_pobj = new T;

		return *m_pobj;
	}
private:
	static T*   m_pobj;
	CSingleObject(const CSingleObject&);
	void operator = (const CSingleObject&);
};

#define INTIALIZE_SINGLEOBJ(T)  T* CSingleObject<T>::m_pobj;
