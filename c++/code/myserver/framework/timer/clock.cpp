
#include "clock.h"

#ifdef LINUX
#include <sys/time.h>
#else
#include <time.h>
#include <sys/timeb.h>
#endif // LINUX


long Clock::now()
{
#ifdef LINUX
	timeval tv;
	::gettimeofday(&tv, 0);
	return 1000000LL * tv.tv_sec + tv.tv_usec;
#else
	struct timeb tb;
	::ftime(&tb);
	return 1000000LL * static_cast<long>(tb.time) + tb.millitm;
#endif
}

Clock::Clock()
	: _start(0),
	  _elapsed(0),
	  _active(false)
{
}

Clock::~Clock()
{
}

int Clock::elapsed() const
{
	if (!active())
		return _elapsed;

	return static_cast<int>(now() - _start);
}

long Clock::start()
{
	_active = true;
	return _start = now();
}

int Clock::stop()
{
	_elapsed = elapsed();
	_active = false;

	return _elapsed;
}
