#pragma once

#include "../player.h"
#include <canvas/types.h>
#include <util/keydefs.h>

namespace canvas {
	class System;
	class Surface;
}

namespace player {

namespace fit {
namespace mode {

enum type  {
	fill,
	hidden,
	meet,
	meetBest,
	slice
};

}	//	namespace mode
}	//	namespace fit

namespace property {
	class FocusHandler;
}

class GraphicProperties;
class FocusProperties;

class GraphicPlayer : public player::Player {
public:
	explicit GraphicPlayer( Device *dev );
	virtual ~GraphicPlayer();

	virtual canvas::Surface* getSurface() const;
	virtual bool isVisible() const;
	canvas::Surface* surface() const;

protected:
	//	Initialization
	virtual bool startPlay();
	virtual bool startPlay(double time);
	virtual void stopPlay();
	virtual void registerProperties();
	virtual void beginRefresh();
	virtual void onPropertyChanged();

	//	Events
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void onPositionChanged( const canvas::Point &point );
	virtual void onResizeChanged( const canvas::Rect &newBounds );

	void forceTransparentBgColor();

	//	Getters
	canvas::Canvas *canvas() const;
	fit::mode::type getFitMode() const;
	std::string rgbChromakey() const;
	virtual void dispatchKey( util::key::type key, bool isUp );

	void updateImage();

private:
	std::string _fitMode;
	GraphicProperties *_graphic;
	FocusProperties *_focus;
};

}
