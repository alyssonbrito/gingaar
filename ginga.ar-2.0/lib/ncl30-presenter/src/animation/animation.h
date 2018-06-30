#pragma once

#include <canvas/types.h>
#include <canvas/color.h>
#include <util/types.h>
#include <boost/function.hpp>
#include <string>
#include <vector>
#include <map>

#include <cmath>

#include <ncl30/util/functions.h>

namespace canvas {
	class Surface;
}

namespace player {
class System;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
class FormatterRegion;
}
}

namespace animation {

namespace bptgnmp = ::br::pucrio::telemidia::ginga::ncl::model::presentation;

typedef boost::function<void( void )> Callback;

class Animation
{
public:
	Animation( double duration, double by, player::System *system );
	virtual ~Animation();

	virtual bool start( bptgnmp::FormatterRegion *formatter );
	virtual bool step();
	virtual void stop();
	virtual void onEnd( const Callback &callback );
	virtual void addProperty( const std::string &property, double startValue, double endValue );
	virtual int addSurface(canvas::Surface* surface, double startValue, double endValue );

protected:
	virtual bool init();
	virtual bool applyStep();

	int getMajorComponent( const canvas::Color & color );
	void prepareAnimationColorKeyFrames(const std::string &prop, long startValue, long endValue);
	canvas::Color createByForColor ( const canvas::Color & color, double _by );
	bool stepColorAnim ( ::util::DWORD & remainingTime, int & stepsInRemainingTime );
	bool applyStepColorAnim ();

protected:
	int frameCountBG;
	bool _isColorAnim;

	bptgnmp::FormatterRegion *_formatter;

	int _remainingSteps;

	int _totalSteps;
	int _stepNumber;
	double _stepTime;
	double _duration;
	double _by;

	std::map<std::string, std::vector<double> > _values;
	Callback _onStop;
	::util::id::Ident _timer;
	::util::DWORD _startTime;
	player::System *_sys;
	canvas::Surface* _surface;
};

}
}
}
}
}
}
