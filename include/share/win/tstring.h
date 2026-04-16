#pragma once

namespace std
{
#ifdef _UNICODE
  typedef wstring tstring;
  typedef wstringstream tstringstream;
  typedef wofstream tofstream;
#else
  typedef string tstring;
  typedef stringstream tstringstream;
  typedef ofstream tofstream;
#endif
}

