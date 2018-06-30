#pragma once

#include "surfaceproperties.h"

namespace player {

class GraphicPlayer;

class FocusProperties : public SurfaceProperties {
public:
	FocusProperties( canvas::Canvas *canvas, GraphicPlayer* player );
	virtual ~FocusProperties();
	
	virtual void registerProperties( Player *player );
	GraphicPlayer *getAltPlayer() { return _altPlayer; }
	void update() { refresh(); }

protected:
	void refresh();
	void drawFocus();
	void drawSelected();
	void drawBorder( const std::string &strColor );
	void composeSurface( const std::string &src );

	virtual void calculateBounds( canvas::Rect &rect );
	virtual void getZIndex( int &zIndex );
	
	void delAlt( bool setVis = true );
	bool startAlt( const std::string &src );
	void applyAltBounds();
	void applyAltZIndex();


private:
	bool _applied;
	bool _focus;
	bool _selected;
	std::string _borderColor;
	std::string _selBorderColor;
	int _width;
	int _curWidth;
	float _transparency;
	std::string _focusSource;
	std::string _focusSelSource;
	GraphicPlayer* _player;
	GraphicPlayer* _altPlayer;
	bool _hasAltSel;
	canvas::Rect _altBounds;
	int _altZIndex;
};

}

