#pragma once

#include "animation.h"

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


class TransitionAnimation : public Animation
{
public:
	TransitionAnimation( double duration, double by, player::System *system );
	virtual ~TransitionAnimation();

	virtual bool step();//OKAY
	virtual void addProperty( const std::string &property, double startValue, double endValue );//OK
	virtual int addSurface(canvas::Surface* surface, double startValue, double endValue );//OK
	virtual void stop();

protected:
	double _startProgress,_endProgress;
	virtual bool init();
	virtual bool applyStep();//OK

};

}
}
}
}
}
}
