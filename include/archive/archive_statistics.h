#pragma once
#include "stdio.h"

//#if defined(WIN32) && !defined(WINCE)
//#pragma pack(1)
//#endif


/*class StatParam
{
	size_t _val;
public:
	StatParam()
	{
		Clear();
	}
	virtual void Set(size_t newVal)
	{
		_val = newVal;
	}
	virtual size_t Get(void) const
	{
		return _val;
	}
	virtual void Clear(void)
	{
		_val = 0;
	}
};*/


template <class T>
class StatParam
{
protected:
	T _val;
public:
	StatParam()
	{
		Clear();
	}
	virtual void Set(T newVal)
	{
		_val = newVal;
	}
	T Get(void) const
	{
		return _val;
	}
	virtual void Clear(void)
	{
		_val = 0;
	}
};


template <class T>
class MinStatParam : public StatParam<T>
{
public:
	MinStatParam()
	{
		Clear();
	}
	virtual void Set(T newVal)
	{
		if (this->_val > newVal || this->_val < 0)
			this->_val = newVal;
	}
/*	T Get(void) const
	{
		return _val;
	}*/
	virtual void Clear(void)
	{
		StatParam<T>::_val = -1;
	}
};


template <typename T>
class MaxStatParam : public StatParam<T>
{
public:
	MaxStatParam()
	{
		StatParam<T>::Clear();
	}
	virtual void Set(T newVal)
	{
		if (StatParam<T>::_val < newVal)
			StatParam<T>::_val = newVal;
	}
};




class PeriodStatParam
{
	unsigned long _cnt;
public:
	//StatParam<long long> Min;
	MinStatParam<long long> Min;
	StatParam<long long> Max;
	StatParam<long long> Avg;
	StatParam<long long> Tot;

	PeriodStatParam()
	{
		Clear();
	}


	void Add(long long val)
	{
//		if ((val < Min.Get() && _cnt != 0) || _cnt == 0)
		if (val < Min.Get() || Min.Get() < 0)
			Min.Set(val);
		if (val > Max.Get())
			Max.Set(val);
		_cnt++;
		Tot.Set(Tot.Get() + val);
		Avg.Set(Tot.Get() / _cnt);
	}

	void Clear()
	{
		Min.Clear();
		Max.Clear();
		Avg.Clear();
		Tot.Clear();
		_cnt = 0;
	}

	unsigned long Cnt(void) const
	{
		return _cnt;
	}
};



class ArchiveStatistics
{
	size_t _maxRecs;
	size_t _maxRequests;
	size_t _numParams;

	//size_t _totalRecs;
	
public:
	//  ол - во записанных значений всего
	StatParam<size_t> TotalRecs;
	// ол - во записанных значений за период
	StatParam<size_t> PerPeriodRecs;
	// ол - во обработанных запросов - всего
	StatParam<size_t> TotalRequests;
	// ол - во обработанных запросов - за период
	StatParam<size_t> PerPeriodRequests;

	// ол - во удаленных значений всего
	StatParam<size_t> TotalDeletedRecs;

	// ол - во удаленных значений за период
	StatParam<size_t> PerPeriodDeletedRecs;


	PeriodStatParam RequestsPerInterval;
	PeriodStatParam RequestsTotal;
	PeriodStatParam WritesPerInterval;
	PeriodStatParam WritesTotal;
	// ¬рем€ записи одного значени€ - за период - "(среднее, мин, макс)"
	PeriodStatParam WriteItemTimePerInterval;
	// ¬рем€ записи одного значени€ - всего -  мин
	MinStatParam<long long> WriteItemTimeMinTotal;
	// ¬рем€ записи одного значени€ - всего - макс
	MaxStatParam<long long> WriteItemTimeMaxTotal;

	// ќперации удалени€
	PeriodStatParam DeletesTotal;
	//¬рем€ удалени€ одного значени€ - за период - "(среднее, мин, макс)
	PeriodStatParam DeleteItemTimePerInterval;
	// ¬рем€ удалени€ одного значени€ -  всего - мин
	MinStatParam<long long> DeleteItemTimeMinTotal;
	// ¬рем€ удалени€ одного значени€ -  всего - макс
	MaxStatParam<long long> DeleteItemTimeMaxTotal;


	StatParam<size_t> CurRecordsToWrite;
	MaxStatParam<long long> MaxMoveTime;


	/*
	//¬рем€ обработки запроса - за период (среднее)
	StatParam<long long> PerPeriodRequestsTimeAvg;
	//¬рем€ обработки запроса - за период (мин)
	StatParam<long long> PerPeriodRequestsTimeMin;
	//¬рем€ обработки запроса - за период (макс)
	StatParam<long long> PerPeriodRequestsTimeMax;
	*/

	/*
	//¬рем€ обработки запроса - всего (среднее)
	StatParam TotalRequestsTimeAvg;
	//¬рем€ обработки запроса - всего (мин)
	StatParam TotalRequestsTimeMin;
	//¬рем€ обработки запроса - всего (макс)
	StatParam TotalRequestsTimeMax;*/
	

	ArchiveStatistics()
	{
		ClearNaxRecs();
		ClearNaxRequests();
		ClearNumParams();
	}
	virtual ~ArchiveStatistics()
	{

	}

	// –азмер очереди записи(_recsToWrite)

	// получает значение максималного кол-ва записей
	size_t NaxRecs(void ) const
	{
		return _maxRecs;
	}

	// ”станавливает новое максимальное значение кол-ва записей если новое значение больше. ¬озвращает максимальное значение записей
	size_t SetNaxRecs(size_t newMaxRecs)
	{
		if (newMaxRecs > _maxRecs)
			_maxRecs = newMaxRecs;
		return _maxRecs;
	}
	void ClearNaxRecs(void) 
	{
		_maxRecs = 0; 
	}

	// ол - во запросов в очереди(_requests)
	size_t NaxRequests(void) const
	{
		return _maxRequests;
	}
	// ”станавливает новое максимальное значение кол-ва запросов если новое значение больше. ¬озвращает максимальное значение запросов
	size_t SetNaxRequests(size_t newMaxRequests)
	{
		if (newMaxRequests > _maxRequests)
			_maxRequests = newMaxRequests;
		return _maxRequests;
	}
	void ClearNaxRequests(void)
	{
		_maxRequests = 0;
	}

	// ол - во архивируемых параметров(_itemsInfo)
	size_t NumParams(void) const
	{
		return _numParams;
	}

	void SetNumParams(size_t newSize)
	{
		_numParams = newSize;
	}
	void ClearNumParams(void)
	{
		_maxRequests = 0;
	}


};
