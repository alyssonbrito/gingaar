#include "transitionanimation.h"
#include "../../include/model/FormatterRegion.h"
#include <gingaplayer/player.h>
#include <gingaplayer/system.h>
#include <gingaplayer/property/types.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <canvas/surface.h>

namespace prop = player::property::type;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace animation {
TransitionAnimation::TransitionAnimation( double duration, double by, player::System *system ):
	Animation(duration,by,system){
  APRINT_NCLP("<==> duration[%f] by[%f]",duration,by);
		_startProgress = 0;
		_endProgress = 1;
}

TransitionAnimation::~TransitionAnimation() {
}

void TransitionAnimation::addProperty( const std::string &property, double startValue, double endValue ) {
	if (!_by) {
		_by = 0.04; // ~ 24 FPS
	}
	LDEBUG("TransitionAnimation", "animation, property=%s, startValue=%f, endValue=%f, duration=%d, by=%f", property.c_str(), startValue, endValue, _duration, _by);

  if(_by==0.04 || _by==0){
  }
	_remainingSteps = (int) (_duration / _by);
	double increment = (endValue - startValue) / (_remainingSteps * 1.0);

	if (increment != 0.0 && _remainingSteps) {
		LDEBUG("TransitionAnimation", "animation, steps=%d, increment=%f", _remainingSteps, increment);

		std::string p = property;
		for (int i = 0; i < _remainingSteps; ++i) {
			_values[p].insert( _values[p].begin(), boost::math::iround( startValue + increment * i ) );
		}
		_values[p].insert( _values[p].begin(), endValue );
	}else{
  }
}


bool TransitionAnimation::init() {
	return (_values.size() > .0) || (_surface != NULL);
}

bool TransitionAnimation::step() {

	// adjust remaining steps according to the remaining time
	::util::DWORD remainingTime = (_duration * 1000) - (_formatter->uptime() - _startTime);
	int stepsInRemainingTime = (int) (remainingTime / (_by * 1000));
	if (_remainingSteps > stepsInRemainingTime) {
		_remainingSteps = stepsInRemainingTime + 1;
	}

	if (_remainingSteps) {
		_remainingSteps -= 1;
		if (applyStep() && _remainingSteps>0) {
			float t  = ((_duration * 1000) - (_formatter->uptime() - _startTime))/_remainingSteps;
			_timer = _sys->registerTimer( (util::DWORD) (_by*1000), boost::bind( &bptgnmp::FormatterRegion::stepAnimation, _formatter, this ) );
			return true;
		} else {
			LINFO("TransitionAnimation", "end, duration=%d ms", (_formatter->uptime() - _startTime) );
			_sys->unregisterTimer( _timer );
			_remainingSteps = 0;
		}
	}
	return false;
}

bool TransitionAnimation::applyStep() {

	if (_surface && _surface->HasMask((void*)this)) {
		_surface->MaskStepTransition(_remainingSteps, (void*)this);
		APRINT_NCLP("<== _remainingSteps[%d]",_remainingSteps);
		return true;
	}

	bool result = false;
	_formatter->enabled( false );
	std::map<std::string, std::vector<double> >::iterator it = _values.begin();
	for (; it != _values.end(); ++it) {
		std::string value = boost::lexical_cast<string>( (*it).second[_remainingSteps] );
		std::string property = (*it).first;
		if (property == "transparency") {
			value += "%";
		}
		result = _formatter->setPropertyToPlayer( property, value );
	}
	_formatter->enabled( true );
	_formatter->updateRegionBounds();
	return result;
}

void TransitionAnimation::stop() {
	if (_surface) {
		_surface->MaskRemove(this);
		if (_endProgress!=1){
			APRINT_NCLP("final flush");
			_surface->flush();
		}
		_surface = NULL;
	}
	if (!_onStop.empty()) {
		_onStop();
	}
	APRINT_NCLP("<==");
}

int TransitionAnimation::addSurface(canvas::Surface* surface, double startValue, double endValue )
{
  APRINT_NCLP("==> start:end[%f:%f] _by[%f]",startValue,endValue,_by);
	_startProgress = startValue;
	_endProgress = endValue;
	if (!_by) {
		_by = 0.04; //	~ 24 FPS
	}

	_remainingSteps = (int) (_duration / _by);
	if ((_remainingSteps <= 0) || (endValue < startValue)) {
		return 0;
	}

	_surface = surface;

	return _remainingSteps;
}


}
}
}
}
}
}

