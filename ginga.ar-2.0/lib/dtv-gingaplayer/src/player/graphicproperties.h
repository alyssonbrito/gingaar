#pragma once

#include "surfaceproperties.h"
#include <string>
#include <typeinfo>

namespace player {

class GraphicProperties : public SurfaceProperties {
public:
	GraphicProperties( canvas::Canvas *canvas );
	virtual ~GraphicProperties( void );

	void clear();
	virtual void registerProperties( Player *player );
	bool isVisible() const;
	std::string rgbChromakey() const;

	void forceTransparentBgColor();

protected:
	//	Properties
	void applyOpacity();
	void applyVisible();
	void applyBackgroundColor();
	void applyChromakey();

private:
	std::string _backgroundColor;
	std::string _chromakey;
	float _opacity;
	bool _visible;
};

}
