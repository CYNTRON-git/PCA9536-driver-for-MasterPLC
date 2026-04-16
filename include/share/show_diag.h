#pragma once
class ShowDiag
{
protected:
	int _TimeIntervalFordiagInfo;

private:
	int64_t _CurTimeIntervalFordiagInfo;
    int64_t _LastTimeFordiagInfo;

public:

	static const int DisabledShowDiag = -1;
	ShowDiag(int showTimeInterval)
	{
		_TimeIntervalFordiagInfo = showTimeInterval;
		_CurTimeIntervalFordiagInfo = 0;
		_LastTimeFordiagInfo = 0;
	}

	bool IsTimeElasped()
	{
		if (_TimeIntervalFordiagInfo >= 0)
		{
            _CurTimeIntervalFordiagInfo = getTimePoint() / FT_MILLISECOND;
            if (_CurTimeIntervalFordiagInfo - _LastTimeFordiagInfo > _TimeIntervalFordiagInfo)
			{
				_LastTimeFordiagInfo = _CurTimeIntervalFordiagInfo;
				return true;
			}
		}
		return false;
	}

	virtual void ShowDiagInfo(const char* reason) = 0;

};

