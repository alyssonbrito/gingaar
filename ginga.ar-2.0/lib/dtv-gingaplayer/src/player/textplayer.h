#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>

namespace player {

class TextPlayer: public GraphicPlayer {
public:
	TextPlayer( Device *dev );
	virtual ~TextPlayer();

	static const int DEFAULT_FONT_SIZE = 16;

private:
	bool hasHorizontalScroll();
	bool hasVerticalScroll();
	void drawVerticalScrollBar();

protected:
	virtual void refresh();
	virtual void registerProperties();

	bool readFile( std::string &text );
	void applyFont();
	void applyColor();
	void applyScroll();
	void applyAlign();
	void userEventReceived( ::util::key::type key, bool isUp );

	typedef std::pair<std::string,std::string> AttributionEventData;
	void setAdditionalParameters( const AttributionEventData &data );

private:
	bool _applied;
	bool _scrollApplied;
	int _size;
	std::string _family;
	std::string _style;
	std::string _variant;
	std::string _weight;
	std::string _color;
	std::string _scroll;
	std::string _beginText;
	std::string _endText;
	std::string _align;
	long _beginPosition;
	long _endPosition;
	unsigned int _scrollIndex;
	unsigned int _scrollLimit;

};

}
