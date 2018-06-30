#include "timelinetimer.h"
#include "../system.h"
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace player {

namespace impl {

class Timer {
public:
	Timer( util::DWORD ms, const impl::TimerCallback &fnc )
		: _ms(ms), _fnc(fnc) { assert(!fnc.empty()); }
	virtual ~Timer() {}

	util::DWORD ms() const { return _ms; }
	void dispatch() { _fnc(); }

private:
	util::DWORD _ms;
	impl::TimerCallback _fnc;
};
	
bool CompareTimers::operator()( impl::Timer *x, impl::Timer *y ) { 
	return x->ms() > y->ms(); 
}

}	//	namespace impl


TimeLineTimer::TimeLineTimer( System *sys )
	: _sys( sys )
{
	_state = impl::state::stopped;
	_elapsed = 0;
	std::make_heap( _timers.begin(), _timers.end(), _cmp );
}

TimeLineTimer::~TimeLineTimer()
{
	stop();
	cleanTimers();
}

//	Methods
void TimeLineTimer::addTimer( util::DWORD ms, const impl::TimerCallback &fnc ) {
	cancelTimer();
	
	//	Add timer
	_timers.push_back( new impl::Timer(ms,fnc) );
	std::push_heap( _timers.begin(), _timers.end(), _cmp );
	
	startTimer();
}

void TimeLineTimer::start() {
	if (_state != impl::state::running) {
		_state = impl::state::running;
		_elapsed = 0;
		startTimer();
	}
}

void TimeLineTimer::stop() {
	if (_state > impl::state::stopped) {
		cancelTimer();
		_state = impl::state::stopped;
		_elapsed = 0;
		cleanTimers();
	}
}

void TimeLineTimer::pause() {
	if (_state == impl::state::running) {
		cancelTimer();
		_state = impl::state::paused;
	}
}

void TimeLineTimer::unpause() {
	if (_state == impl::state::paused) {
		_state = impl::state::running;
		startTimer();
	}
}

//	Getters
bool TimeLineTimer::isRunning() const {
	return _state == impl::state::running;
}

util::DWORD TimeLineTimer::uptime() const {
	//LDEBUG("TimeLineTimer", "uptime: state=%d, _elapsed=%d", _state, _elapsed);
	if (_state == impl::state::running) {
		//	Running
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();
		assert( now >= _startTimer );
		impl::pt::time_duration d=now - _startTimer;
		return _elapsed + (util::DWORD) d.total_milliseconds();
	}
	else if (_state == impl::state::paused) {
		return _elapsed;
	}
	else {
		return 0;
	}
}

//	Aux
void TimeLineTimer::cleanTimers() {
	while (!_timers.empty()) {
		removeTimer( _timers.front(), false );
	}
}

void TimeLineTimer::startTimer() {
	if (isRunning()) {
		//	Start timer
		_startTimer = impl::pt::microsec_clock::local_time();

		if (!util::id::isValid(_timerID)) {
			while (!_timers.empty()) {
				//	Get minimum timer
				impl::Timer *timer=_timers.front();

				//	Is timer expired?
				if (_elapsed > timer->ms()) {
					//	Dispatch timer callback and remove from list
					removeTimer( timer, true );
				}
				else {
					//	Start system timer
					util::DWORD min = timer->ms() - _elapsed;
					LINFO("TimeLineTimer", "Timer: min=%d", min);
					_timerID = _sys->registerTimer(
						min,
						boost::bind(&TimeLineTimer::onExpired,this,_1) );
					break;
				}
			}
		}
	}
}

void TimeLineTimer::cancelTimer() {
	if (isRunning()) {
		//	Cancel system timer
		_sys->unregisterTimer( _timerID );

		//	Add elapsed time
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();
		_elapsed = _elapsed + (util::DWORD) (now - _startTimer).total_milliseconds();
	}
}

void TimeLineTimer::removeTimer( impl::Timer *timer, bool dispatch ) {
	assert(timer);
	
	//	Remove from list
	std::pop_heap( _timers.begin(), _timers.end(), _cmp );
	_timers.pop_back();

	//	Dispatch
	if (dispatch) {
		timer->dispatch();
	}

	//	Clean
	delete timer;
}

void TimeLineTimer::onExpired( util::id::Ident & /*id*/ ) {
	assert( util::id::isValid(_timerID) );
	_timerID.reset();

	LINFO("TimeLineTimer", "Timer expired: uptime=%d", uptime());
	
	//	Dispatch
	removeTimer( _timers.front(), true );

	//	Start timer again
	cancelTimer();
	startTimer();
}

}
