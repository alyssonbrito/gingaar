/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "../../../include/model/FormatterRegion.h"
#include "../../../include/model/CascadingDescriptor.h"
#include "../../../include/adapters/FormatterPlayerAdapter.h"
#include "../../animation/transitionanimation.h"
#include <ncl30/util/functions.h>
#include <ncl30/transition/Transition.h>

#include <gingaplayer/player.h>
#include <canvas/point.h>
#include <canvas/size.h>
#include <canvas/rect.h>
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"

#include <boost/lexical_cast.hpp>

#include <gingaplayer/player.h>
#include <canvas/surface.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
FormatterRegion::FormatterRegion( string objectId, bptgnmp::CascadingDescriptor* descriptor ) {

	_objectId = objectId;
	_player = NULL;
	_selected = false;
	_enabled = true;
	if (descriptor != NULL) {
		_region = descriptor->getRegion()->cloneRegion();
		_transitionIn = getSupportedTransition( descriptor->getInputTransitions() );
		_transitionOut = getSupportedTransition( descriptor->getOutputTransitions() );
	}
	_sys = NULL;
}

FormatterRegion::~FormatterRegion() {
  APRINT_NCLP("<==>");
	CLEAN_ALL( bptgnan::Animation *, _animations );
	if (_region != NULL) {
		DEL( _region );
	}
}

bool FormatterRegion::updateRegionBounds() {
	bool result = true; //	TODO false, Es porque hay un llamado donde no tiene player aun
	if (_player != NULL && _enabled) {
		canvas::Rect region( _region->getLeft(), _region->getTop(), _region->getWidth(), _region->getHeight() );

		LDEBUG("FormatterRegion", "bounds: x=%d, y=%d, w=%d, h=%d, z=%d", region.x, region.y, region.w, region.h, _region->getZIndex());
		result &= _player->getPlayer()->setProperty( "bounds", region );
		result &= _player->getPlayer()->setProperty( "zIndex", _region->getZIndex() );
	}

	return result;
}

LayoutRegion* FormatterRegion::getLayoutRegion() {
	return _region;
}

void FormatterRegion::prepareOutputDisplay( FormatterPlayerAdapter* player ) {
	_player = player;
	updateRegionBounds();
	_sys = _player->getSystem();
}

void FormatterRegion::unprepareOutputDisplay( void ) {
  APRINT_NCLP("<==>");
	_player = NULL;
	CLEAN_ALL( bptgnan::Animation *, _animations );
}

bool FormatterRegion::isVisible() {
	return _player->getPlayer()->isVisible();
}

bool FormatterRegion::isSelected() {
	return _selected;
}

bool FormatterRegion::setSelection( bool selected ) {
	if (_selected != selected) {
		_selected = selected;
		_player->getPlayer()->setProperty( "focusSelected", _selected );
		return _selected;
	}
	return false;
}

void FormatterRegion::setFocus( bool focus ) {
	_player->getPlayer()->setProperty( "focus", focus );
}

bool FormatterRegion::setPropertyToPlayer( const std::string &name, const std::string &value ) {
	APRINT_PROPERTY("<==> name[%s] value[%s]",name.c_str(),value.c_str());
	return _player->setPropertyToPlayer( name, value );
}

//	Animations
bool FormatterRegion::startAnimation( bptgnan::Animation* animation ) {
	APRINT_NCLP("==>");
	//we add here (instead of doing inside the 'then')
	// becase maybe there is just one animatinon step
	//so, before the animation->start() returns, the stopAnimation()
	//is called.
	_animations.push_back( animation );
	if (animation->start( this )) {
		APRINT_NCLP("<== return [TRUE]");
		return true;
	} else {
		_animations.pop_back();
		DEL(animation)
		APRINT_NCLP("<== return[FALSE]");
		return false;
	}
}

void FormatterRegion::stopAnimation( bptgnan::Animation* animation ) {
	APRINT_NCLP("==>");
	std::vector<bptgnan::Animation*>::iterator it = std::find( _animations.begin(), _animations.end(), animation );
	if (it != _animations.end()) {
		APRINT_NCLP("---");
		bptgnan::Animation *a = (*it);
		_animations.erase( it );
		a->stop();
		delete a;
		LINFO("FormatterRegion", "animation removed");
	}
	APRINT_NCLP("<==");
}

void FormatterRegion::stepAnimation( bptgnan::Animation* animation ) {
	APRINT_NCLP("==>");
	if (!animation->step()) {
		APRINT_NCLP("<--> stopAnimation");
		stopAnimation( animation );
	}
	APRINT_NCLP("<==");
}

void FormatterRegion::enabled( bool enable ) {
	_enabled = enable;
}

util::DWORD FormatterRegion::uptime() const {
	return _player->uptime();
}

void FormatterRegion::transitionIn( void ) {
  APRINT_NCLP("<==>");
	if (haveTransitionIn()) {
		processTransition( _transitionIn, true );
	}
}

void FormatterRegion::transitionOut( void ) {
  APRINT_NCLP("<==>");
	if (haveTransitionOut()) {
		processTransition( _transitionOut, false );
	}
}

bool FormatterRegion::haveTransitionIn( void ) {
	return _transitionIn != NULL;
}

bool FormatterRegion::haveTransitionOut( void ) {
	return _transitionOut != NULL;
}

bptnt::Transition* FormatterRegion::getSupportedTransition( vector<bptnt::Transition*>* transitions ) {
  APRINT_NCLP("<==>");
	bptnt::Transition* transition = NULL;
	if (!transitions->empty()) {
		unsigned int i;
		for (i = 0; i < transitions->size(); i++) {
			transition = (*transitions)[i];
			switch ( transition->getType() ) {
				case Transition::TYPE_FADE: {
					return transition;
					break;
				}
				case Transition::TYPE_BARWIPE: {
					return transition;
					break;
				}
				case Transition::TYPE_IRISWIPE: {
					return transition;
					break;
				}
				case Transition::TYPE_CLOCKWIPE: {
					return transition;
					break;
				}
				case Transition::TYPE_SNAKEWIPE: {
					return transition;
					break;
				}
				default:
					break;
			}
		}
	}
	return NULL;
}

void FormatterRegion::processTransition( bptnt::Transition* transition, bool onPlay ) {
	switch ( transition->getType() ) {
		case Transition::TYPE_FADE: {
			fade( transition, onPlay );
			break;
		}
		case Transition::TYPE_BARWIPE:
		case Transition::TYPE_IRISWIPE:
		case Transition::TYPE_CLOCKWIPE:
		case Transition::TYPE_SNAKEWIPE: {
			canvas::Surface* surface = _player->getPlayer()->getSurface();
			if (!surface) {
				return;
			}

			int duration = (int) transition->getDur() / 1000;
			double startProgress = transition->getStartProgress();
			double endProgress = transition->getEndProgress();

			bptgnan::Animation *anim = new bptgnan::TransitionAnimation( duration, 0, _sys );
			if (anim != NULL) {
				int stepCount = anim->addSurface(surface, startProgress, endProgress);
				surface->MaskCreate(onPlay, (void*)transition, stepCount, (void*)anim);
				if (!surface->HasMask((void*)anim)) {
					DEL(anim)
					return;
				}
				if (!onPlay) {
					anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
				}
				startAnimation( anim );
			}
			break;
		}
		default:
			break;
	};
}

void FormatterRegion::barWipe( bptnt::Transition* transition, bool onPlay ) {
  APRINT_NCLP("<==>");
	int duration = (int) transition->getDur() / 1000;
	double startProgress = transition->getStartProgress();
	double endProgress = transition->getEndProgress();
 
	bptgnan::Animation *anim = new bptgnan::TransitionAnimation( duration, 0, _sys );

	switch ( transition->getSubtype() ) {
		case bptnt::Transition::SUBTYPE_BARWIPE_LEFTTORIGHT: {
			int w = _region->getWidth();
			int l = _region->getLeft();
			if (onPlay) {
				anim->addProperty( "width", startProgress * w, endProgress * w );
				if (transition->getDirection() == Transition::DIRECTION_REVERSE) {
					anim->addProperty( "left",  (l + w),  l );
				}
			} else {
				anim->addProperty( "width", endProgress * w, startProgress * w );
				if (transition->getDirection() != Transition::DIRECTION_REVERSE) {
					anim->addProperty( "left", l, endProgress * w );
				}
			}
			break;
		}
		case bptnt::Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM: {
			int h = _region->getHeight();
			int t = _region->getTop();
			if (onPlay) {
				anim->addProperty( "height", startProgress * h, endProgress * h );
				if (transition->getDirection() == Transition::DIRECTION_REVERSE) {
					anim->addProperty( "top",  (t + h),  t );
				}
			} else {
				anim->addProperty( "height", endProgress * h, startProgress * h );
				if (transition->getDirection() != Transition::DIRECTION_REVERSE) {
					anim->addProperty( "top", t, endProgress * t + h );
				}
			}
			break;
		}

		default:
			break;
	};

	if (anim != NULL) {
		if (!onPlay) {
			anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
		}
    APRINT_NCLP("|--");
		startAnimation( anim );
	}

}

bool FormatterRegion::addTransition( bptnt::Transition* transition, bool isTransIn ) {
  APRINT_NCLP("<==>");
	vector<bptnt::Transition*> transitions;
	transitions.push_back(transition);
	if (getSupportedTransition( &transitions )) {
		if (isTransIn) {
			_transitionIn = transition;
		} else {
			_transitionOut = transition;
		}
		return true;
	}
	return false;
}

bptnt::Transition* FormatterRegion::getTransOut() {
	return _transitionOut;
}

void FormatterRegion::irisWipe( bptnt::Transition* transition, bool onPlay ) {
  APRINT_NCLP("<==>");
	int duration = (int) transition->getDur() / 1000;
	double startProgress = transition->getStartProgress();
	double endProgress = transition->getEndProgress();

	bptgnan::Animation *anim = new bptgnan::TransitionAnimation( duration, 0, _sys );

	int w = _region->getWidth();
	int l = _region->getLeft();
	int h = _region->getHeight();
	int t = _region->getTop();

	switch ( transition->getSubtype() ) {
		case bptnt::Transition::SUBTYPE_IRISWIPE_RECTANGLE: {
			if (onPlay) {	// transIn
				if (transition->getDirection() == Transition::DIRECTION_REVERSE) {
						anim->addProperty( "width", endProgress * w, startProgress * w + 1 );
						anim->addProperty( "height", endProgress * h, startProgress * h + 1 );
						anim->addProperty( "left", l + w/2 - endProgress * w/2, l + w/2 - startProgress * w/2 );
						anim->addProperty( "top", t + h/2 - endProgress * h/2 , t + h/2 - startProgress * h/2 );
				} else {
						anim->addProperty( "width", startProgress * w, endProgress * w );
						anim->addProperty( "height", startProgress * h, endProgress * h );
						anim->addProperty( "left", l + w/2 - startProgress * w/2, l + w/2 - endProgress * w/2 );
						anim->addProperty( "top", t + h/2 - startProgress * h/2, t + h/2 - endProgress * h/2 );
				}
			} else {	// transOut
				if (transition->getDirection() == Transition::DIRECTION_REVERSE) {
						anim->addProperty( "width", endProgress * w, startProgress * w + 1 );
						anim->addProperty( "height", endProgress * h, startProgress * h + 1 );
						anim->addProperty( "left", l + w/2 - endProgress * w/2, l + w/2 - startProgress * w/2 );
						anim->addProperty( "top", t + h/2 - endProgress * h/2 , t + h/2 - startProgress * h/2 );
				} else {
						anim->addProperty( "width", startProgress * w, endProgress * w );
						anim->addProperty( "height", startProgress * h, endProgress * h);
						anim->addProperty( "left", l + w/2 - startProgress * w/2, l + w/2 - endProgress * w/2 );
						anim->addProperty( "top", t + h/2 - startProgress * h/2, t + h/2 - endProgress * h/2 );
				}
			}
		}
			break;
		//TODO:	implement iris diamond transformation
		case bptnt::Transition::SUBTYPE_IRISWIPE_DIAMOND: {
			if (onPlay) {
			} else {
			}
		}
			break;
		default:
			break;
	};

	if (anim != NULL) {
		if (!onPlay) {
			anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
		}
		startAnimation( anim );
	}
}

void FormatterRegion::fade( bptnt::Transition* transition, bool onPlay ) {
	int duration = (int) transition->getDur() / 1000;
	double startProgress = (1.0 - transition->getStartProgress()) * 100;
	double endProgress = (1.0 - transition->getEndProgress()) * 100;

	bptgnan::Animation *anim = new bptgnan::TransitionAnimation( duration, 0, _sys );
	if (anim==NULL){
		return;
	}

	switch ( transition->getSubtype() ) {
		case bptnt::Transition::SUBTYPE_FADE_CROSSFADE:
		default:
		{
			if (onPlay) {
				anim->addProperty( "transparency", startProgress, endProgress ); //	100 -> 0
			} else {
				anim->addProperty( "transparency", endProgress, startProgress); //	0 -> 100
			}
		}
	};

	if (!onPlay) {
		anim->onEnd( boost::bind( &PlayerAdapterManager::stop, _player->getManager(), _player->getObject(), false ) );
	}
	startAnimation( anim );

}

}
}
}
}
}
}
}

