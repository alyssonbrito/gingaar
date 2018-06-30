#include "animation.h"
#include "../../include/model/FormatterRegion.h"
#include <gingaplayer/player.h>
#include <gingaplayer/system.h>
#include <gingaplayer/property/types.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <canvas/surface.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace prop = player::property::type;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace animation {

Animation::Animation( double duration, double by, player::System *system ) {
	APRINT_NCLP("<==> duration[%f] by[%f]",duration,by);
	_formatter = NULL;
	_duration = duration;
	_by = by;
	_remainingSteps = 0;
	_sys = system;
	_surface = NULL;
	_isColorAnim = false;
}

Animation::~Animation() {
	APRINT_NCLP("<==> duration[%f] by[%f]",_duration,_by);
	_formatter = NULL;
	_sys->unregisterTimer( _timer );
	if (_surface) {
		_surface->MaskRemove(this);
	}
}

int Animation::getMajorComponent( const canvas::Color & color ) {
	int res = color.r;
	res = ( res < color.g ) ? color.g : res;
	res = ( res < color.b ) ? color.b : res;
	res = ( res < color.r ) ? color.r : res;
	return res;
}

canvas::Color Animation::createByForColor ( const canvas::Color & color, double _by ) {
	int dir_r = ( ( color.r < 0 ) ? -1 : ( color.r > 0 ) ? 1 : 0 );
	int dir_g = ( ( color.g < 0 ) ? -1 : ( color.g > 0 ) ? 1 : 0 );
	int dir_b = ( ( color.b < 0 ) ? -1 : ( color.b > 0 ) ? 1 : 0 );
	return canvas::Color( dir_r * _by, dir_g * _by, dir_b * _by );
}

void Animation::prepareAnimationColorKeyFrames(const std::string &prop, long startValue, long endValue) {
  if (_by > 255){
    _by = 255;
  }
	canvas::Color startColor = canvas::Color ( startValue );
	canvas::Color endColor   = canvas::Color ( endValue );
	canvas::Color incColor   = ( endColor - startColor ) / _by;

	int major = getMajorComponent ( incColor );

	if (major == 0)
		_remainingSteps = 0;
	else
		_remainingSteps = abs ( ( int ) ( _duration / major ) );

	_isColorAnim = true;

	frameCountBG = 1;
	if (_remainingSteps) {
		canvas::Color colorBy = createByForColor ( incColor, _by );

		int currentTime = 0;
		canvas::Color current = startColor;

		while ( currentTime < _duration ) {
			double value = boost::math::iround( current.asNumber() );
			_values[prop].insert( _values[prop].begin(), value );
			currentTime += _remainingSteps;
			current = current + colorBy;
			frameCountBG++;
		}

	}
	_values[prop].insert( _values[prop].begin(), endValue );
}

bool Animation::stepColorAnim (::util::DWORD & remainingTime, int & stepsInRemainingTime ) {

	stepsInRemainingTime = (int) (remainingTime / frameCountBG * 1000);

	if (frameCountBG > stepsInRemainingTime) {
		frameCountBG = stepsInRemainingTime + 1;
	}

	if (frameCountBG) {
		frameCountBG -= 1;
		if (applyStep() && frameCountBG) {
			_timer = _sys->registerTimer( (util::DWORD) ( _remainingSteps * 1000), boost::bind( &bptgnmp::FormatterRegion::stepAnimation, _formatter, this ) );
			return true;
		} else {
			LINFO("Animation", "end, duration=%d ms", (_formatter->uptime() - _startTime) );
			_sys->unregisterTimer( _timer );
			frameCountBG = 0;
		}
	}
	return false;
}

bool Animation::applyStepColorAnim () {
	bool result = false;

	if (_surface && _surface->HasMask((void*)this)) {
		_surface->MaskStepTransition(frameCountBG, (void*)this);
		return true;
	}

	std::map<std::string, std::vector<double> >::iterator it = _values.begin();

	for (; it != _values.end(); ++it) {
		std::string property = (*it).first;

		//if (property == "backgroundColor" ) {
			frameCountBG = ( frameCountBG < 0 ? 0 : frameCountBG );
			std::string value = ncl_util::intToHex ( (*it).second[ frameCountBG ] );
			LDEBUG("Animation", "Color (%s)", value.c_str() );
			result = _formatter->setPropertyToPlayer( property, value );
		//}
	}
	return result;
}

void Animation::addProperty( const std::string &property, double startValue, double endValue ) {
	APRINT_NCLP("==> property[%s] start:end[%f:%f]",property.c_str(),startValue,endValue);
	LDEBUG("Animation", "animation, property=%s, startValue=%f, endValue=%f, duration=%f, by=%f", property.c_str(), startValue, endValue, _duration, _by);

	if (property == "backgroundColor" || property == "fontColor" || property == "rgbChromakey"){
		APRINT_NCLP("RGB case");
		prepareAnimationColorKeyFrames(property,static_cast< long > ( startValue ), static_cast< long > ( endValue ));
	}else {
		double length = endValue - startValue;
		bool rest = false;

		APRINT_NCLP("length[%f] _by[%f] ",length,_by);
		//calculate step number
		if ( _by == 0 || _by == 0.04 ) {
			APRINT_NCLP("---by[set to 1] ");
			if (length>40 && _duration < 30)
				_by = length/12;
			else
				_by = 4;
			_stepNumber = (int) (length/_by);
		}
		else {
			double interval = length/_by;

			_stepNumber = boost::numeric_cast<int>(interval);
			rest = boost::numeric_cast<double>(_stepNumber) != interval;
			//APRINT_NCLP("b[%d] restBoolean[%d] restSimple[%d] restSimpleBoolean[%d]",a,rest,(int)(length/_by),0!=((int)(length/_by)));
			//_stepNumber = a;
		}

		APRINT_NCLP("_stepNumber[%d] ",_stepNumber);
		if (_stepNumber < 0) {
			_stepNumber *= -1;
		}

		APRINT_NCLP("_stepNumber[%d] ",_stepNumber);
		_stepNumber += rest ? 1:0;
		APRINT_NCLP("_stepNumber[%d] ",_stepNumber);

		if ( endValue < startValue && _by > 0) {
			_by *= -1;
			APRINT_NCLP("---negative by[%f]",_by);
		}

		_totalSteps = _stepNumber;
		_stepTime = ( _duration * 1000.0 ) / _stepNumber;
		if (_duration==0){
			_stepNumber = 0;
		}

		if ( _stepNumber > 0 ) {

			std::string p = property;

			if (property=="transparency" || property=="opacity" || property=="soundLevel" || property=="balanceLevel" || property=="trebleLevel" || property=="bassLevel") {
				for (int i = 0; i < _stepNumber; ++i) {
					APRINT_NCLP("going through loop [%d][%f]",i,startValue+_by*i);
					_values[property].insert( _values[property].begin(), startValue + _by * i);//boost::math::iround( startValue + _by * i ) );
				}
			}else{
				for (int i = 0; i < _stepNumber; ++i) {
					APRINT_NCLP("going through loop [%d][%f]",i,startValue+_by*i);
					_values[property].insert( _values[property].begin(), boost::math::iround(startValue + _by * i));
				}//for
			}//else property==
		}else{
			APRINT_NCLP("---");
		}//else _stepNumber>0
		APRINT_NCLP("latest on loop[%f]",endValue);
		_values[property].insert( _values[property].begin(), endValue );
		_stepNumber++;
	}//else property == ...       general property case
	APRINT_NCLP("<== property[%s] start:end[%f:%f] _stepNumber[%d]",property.c_str(),startValue,endValue,_stepNumber);
}

bool Animation::init() {
	APRINT_NCLP ("<==> values.size[%lu] surface[%p] return[%d]",_values.size(),_surface, (_values.size() > .0) || (_surface != NULL));
	return (_values.size() > 0) || (_surface != NULL);
}

bool Animation::start( bptgnmp::FormatterRegion *formatter ) {
	APRINT_NCLP("==>");
	_formatter = formatter;
	if (init()) {
		_startTime = _formatter->uptime();
		step();
		APRINT_NCLP("<== return[true]");
		return true;
	}
	APRINT_NCLP("<== return[false]");
	return false;
}

bool Animation::step() {
	bool res =false;
	APRINT_NCLP("==>");
	// adjust remaining steps according to the remaining time
	::util::DWORD remainingTime = (_duration * 1000) - (_formatter->uptime() - _startTime);

	int stepsInRemainingTime = 0;

	if (_isColorAnim) {
		res = stepColorAnim ( remainingTime, stepsInRemainingTime );
		APRINT_NCLP("<== return[%d]",res);
		return res;
	}
	else {
		APRINT_NCLP("==> _by[%f] _stepTime[%f] _stepNumber[%d]",_by,_stepTime,_stepNumber);
		// adjust remaining steps according to the remaining time
		double remainingTime = (_duration * 1000) - (_formatter->uptime() - _startTime);
		//updateStepTime
		_stepTime = (remainingTime)/ (_stepNumber* 1.0);

		if (_stepNumber>0) {
			_stepNumber -= 1;

			if (applyStep() && _stepNumber>0) {
				remainingTime = (_duration * 1000) - (_formatter->uptime() - _startTime);
				//updateStepTime
				_stepTime = (remainingTime)/ (_stepNumber* 1.0);
				if(remainingTime<=400 && _stepNumber>4){
					//if we timeout,speed up! cut steps
					 APRINT_NCLP("we time out! remainingTime[%f] _stepTime[%f] _stepNumber[%d]",
							 remainingTime,_stepTime,_stepNumber);
					_stepNumber /= 2;
				}
				_timer = _sys->registerTimer( (util::DWORD) (_stepTime-200>0?_stepTime-200:_stepTime), 
						boost::bind( &bptgnmp::FormatterRegion::stepAnimation, _formatter, this ) );
				APRINT_NCLP("<== return TRUE _by[%f] _stepTime[%f] _stepNumber[%d]",_by,_stepTime,_stepNumber);
				return true;
			} else {//iapply Step()&& stepNumber>0
				APRINT_NCLP("---");
				LINFO("Animation", "end, duration=%f ms", (_formatter->uptime() - _startTime) );
				_sys->unregisterTimer( _timer );
				_stepNumber = 0;
				_formatter->stopAnimation(this);
			}//else iapply Step()&& stepNumber>0
		}else{//_stepNumber>0
				APRINT_NCLP("---");
				LINFO("Animation", "end, duration=%f ms", (_formatter->uptime() - _startTime) );
				_sys->unregisterTimer( _timer );
				_stepNumber = 0;
		}//else _stepNumber>0
	}
	APRINT_NCLP("<== return[FALSE] _by[%f] _stepTime[%f] _stepNumber[%d]",_by,_stepTime,_stepNumber);
	return false;
}
bool Animation::applyStep() {
	bool result = false;

	if (_isColorAnim) {
		result = applyStepColorAnim ();
	}
	else {
		APRINT_NCLP("<==> ");
		if (_surface && _surface->HasMask((void*)this)) {
			//_surface->MaskStepTransition(_remainingSteps, (void*)this);
			_surface->MaskStepTransition(_stepNumber, (void*)this);
			APRINT_NCLP("<==");
			return true;
		}

		_formatter->enabled( false );
		std::map<std::string, std::vector<double> >::iterator it = _values.begin();
		for (; it != _values.end(); ++it) {
			//std::string value = boost::lexical_cast<string>( (*it).second[_remainingSteps] );
			std::string value = "";
			std::string property = (*it).first;


			if (property == "transparency" || property == "opacity") {
				value = boost::lexical_cast<string>( boost::math::iround( (*it).second[_stepNumber] * 100.0f ) ) + "%";
			}
			else {
				value = boost::lexical_cast<string>( (*it).second[_stepNumber] );
				APRINT_NCLP("value [%f] [%s]",(*it).second[_stepNumber] ,value.c_str());
			}

			APRINT_NCLP("setProperty[%s] value[%s]",property.c_str(),value.c_str());
			 _formatter->setPropertyToPlayer( property, value );
			result = true;
		}
		_formatter->enabled( true );
		_formatter->updateRegionBounds();
		APRINT_NCLP("<==");
	}
	return result;
}

void Animation::onEnd( const Callback &callback ) {
	APRINT_NCLP("<==>");
	_onStop = callback;
}

void Animation::stop() {
	APRINT_NCLP("<==>");
	if (_surface) {
		_surface->MaskRemove(this);
		_surface->flush();
		_surface = NULL;
	}
	if (!_onStop.empty()) {
		_onStop();
	}
}

int Animation::addSurface(canvas::Surface* surface, double startValue, double endValue )
{
	APRINT_NCLP("==> start:end[%f:%f] _by[%f]",startValue,endValue,_by);
	if (_by == 0) {
		APRINT_NCLP("BY ZERO. SETTING 1");
		_by = 1; //	~ 24 FPS
	}
	//if ((_stepNumber <= 0) || (endValue < startValue)) {
	if (_stepNumber < 0) {
		APRINT_NCLP("<== No more setps. _stepNumber[%d] start:end[%f:%f] _by[%f]",_stepNumber,startValue,endValue,_by);
		return 0;
	}

	_surface = surface;

	APRINT_NCLP("<== _stepNumber[%d]",_stepNumber);
	return _stepNumber;
}


}
}
}
}
}
}

