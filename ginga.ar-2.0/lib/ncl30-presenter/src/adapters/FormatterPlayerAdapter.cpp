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

#include "../../include/adapters/FormatterPlayerAdapter.h"
#include "../animation/animation.h"
#include <ncl30/util/functions.h>
#include <ncl30/transition/TransitionBase.h>
#include "../../include/FormatterMediator.h"
#include <gingaplayer/player.h>
#include <gingaplayer/device.h>
#include <gingaplayer/system.h>
#include <gingaplayer/property/types.h>
#include <gingaplayer/property/propertyimpl.h>
#include <util/mcr.h>
#include <util/log.h>
#include "../../dtv-gingaplayer/src/player/soundplayer.h"
#include "../../dtv-gingaplayer/src/player/soundproperties.h"
namespace prop = player::property::type;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;

#include "../../include/adapters/ApplicationPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application;

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

//	Player properties
#define check_int( value, checkedValue, check ) \
	try { \
		checkedValue = boost::lexical_cast<int>( value ); \
	} catch (boost::bad_lexical_cast &) { \
		check = false; \
	} \

#define check_float( value, checkedValue, check ) \
	try { \
		checkedValue = boost::lexical_cast<float>( value ); \
	} catch (boost::bad_lexical_cast &) { \
		check = false; \
	} \

#define check_bool( value, checkedValue, check ) \
	if (value == "true") { \
		checkedValue = true; \
	} else if( value == "false") { \
		checkedValue = false; \
	} else { \
		check = false; \
	} \
	
#define init_bool false
#define init_int 0
#define init_float 0.0


#define APPLY_PROPERTY( type, name, value ) \
	bool check = true; \
	type checkedValue = init_##type; \
	check_##type( value, checkedValue, check ); \
	if ( check ) { \
		result = _player->setProperty( name, checkedValue ); \
	} else { \
		LERROR("FormatterPlayerAdapter", "%s invalid value, %s expected; value=%s", name.c_str(), #type, value.c_str() ); \
	} \

#define APPLY_INVERSE_PROPERTY( type, name, value ) \
	bool check = true; \
	type checkedValue = init_##type; \
	check_##type( value, checkedValue, check ); \
	if ( check ) { \
		result = _player->setProperty( name, 1 -  checkedValue ); \
	} else { \
		LERROR("FormatterPlayerAdapter", "%s invalid value, %s expected; value=%s", name.c_str(), #type, value.c_str() ); \
	} \

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {

namespace impl {
static FormatterPlayerAdapter::KeyPressCallback _onKey;
}

FormatterPlayerAdapter::FormatterPlayerAdapter( PlayerAdapterManager* manager, player::System *system ) {
	typeSet.insert( "FormatterPlayerAdapter" );
	this->manager = manager;
	this->object = NULL;
	_player = NULL;
	_isKeyhandler = false;
	_sys = system;
	_keyToPlayer = false;
	_keyToFormatter = false;
}

FormatterPlayerAdapter::~FormatterPlayerAdapter() {
	object = NULL;
	manager = NULL;
	stopPlayer();
	_player->device()->destroy( _player );
}

bool FormatterPlayerAdapter::instanceOf( string s ) {
	if (!typeSet.empty()) {
		return (typeSet.find( s ) != typeSet.end());
	} else {
		return false;
	}
}

void FormatterPlayerAdapter::setPlayer( player::Player* player ) {
	_player = player;
}

void FormatterPlayerAdapter::stopPlayer() {
	_sys->delInputListener( this );
	player::Player::OnStatusChanged fnc;
	_player->onStopped( fnc );
}

set<string> FormatterPlayerAdapter::getTypeSet( void ) const {
	return typeSet;
}

PlayerAdapterManager* FormatterPlayerAdapter::getManager( void ) const {
	return manager;
}

player::Player* FormatterPlayerAdapter::getPlayer( void ) const {
	return _player;
}

player::System* FormatterPlayerAdapter::getSystem() {
	return _sys;
}

ExecutionObject* FormatterPlayerAdapter::getObject( void ) const {
	return object;
}

void FormatterPlayerAdapter::setObject( ExecutionObject* o ) {
	object = o;
	if (_player != NULL) {
		_player->onStopped( boost::bind( &PlayerAdapterManager::stop, getManager(), object, false ) );
	}
}

bool FormatterPlayerAdapter::createPlayer() {
	APRINT_NCLP("==>");
	vector<Anchor*>* anchors;
	vector<Anchor*>::iterator i;

	vector<FormatterEvent*>* events;
	vector<FormatterEvent*>::iterator j;

	NodeEntity* dataObject;
	PropertyAnchor* property;
	Node *referredNode;

	bool result = true;
	//TODO player player->addListener(this);

	if (object == NULL) {
		APRINT_NCLP("<==");
		return false;
	}

	result &= setDescriptorParams();

	dataObject = (NodeEntity*) (object->getDataObject());

	// inherits properties and interfaces from original node
	if (dataObject->instanceOf( "ReferNode" )){
		referredNode = (Node*) (((bptnr::ReferNode*) dataObject)->getReferredEntity());
		anchors = ((ContentNode*) referredNode)->getAnchors();
		if (anchors != NULL) {
			APRINT_NCLP("--- adding anchors from referred media  ---");
			i = anchors->begin();
			while (i != anchors->end()) {
				if ((*i)->instanceOf( "PropertyAnchor" )) {
					property = ((PropertyAnchor*) (*i));
					APRINT_NCLP("--- propertyName: %s", property->getPropertyName().c_str());
					result &= setPropertyToPlayer( property->getPropertyName(), property->getPropertyValue() );
				}
				++i;
			}
		}
	}


	if ((dataObject->instanceOf( "ContentNode" )) || (dataObject->instanceOf( "ReferNode" ))){
		APRINT_NCLP("---");
		anchors = ((ContentNode*) dataObject)->getAnchors();
		if (anchors != NULL) {
			APRINT_NCLP("---");
			i = anchors->begin();
			while (i != anchors->end()) {
				APRINT_NCLP("---");
				if ((*i)->instanceOf( "PropertyAnchor" )) {
					property = ((PropertyAnchor*) (*i));

					result &= setPropertyToPlayer( property->getPropertyName(), property->getPropertyValue() );
				}
				++i;
			}
		}
	}


	object->getDescriptor()->getFormatterRegion()->prepareOutputDisplay( this );

	events = object->getEvents();
	if (events != NULL) {
		j = events->begin();
		while (j != events->end()) {
			if (*j != NULL && (*j)->instanceOf( "AttributionEvent" )) {
				property = ((AttributionEvent*) *j)->getAnchor();
				((AttributionEvent*) (*j))->setValueMaintainer( this );
			}
			++j;
		}
		delete events;
		events = NULL;
	}
	APRINT_NCLP("<==");
	return result;
}

#define setIfNotEmpty( prop, value ) \
		if( !value.empty() ) { \
			result &= setPropertyToPlayer( prop, value ); \
		} \

bool FormatterPlayerAdapter::setDescriptorParams() {
	bool result = true;
	CascadingDescriptor* descriptor = object->getDescriptor();
	if (descriptor != NULL) {
		if (descriptor->focusDecoration() != NULL) {
			setIfNotEmpty( "focusBorderColor", descriptor->focusDecoration()->getFocusBorderColor() );
			setIfNotEmpty( "focusBorderTransparency", descriptor->focusDecoration()->getFocusBorderTransparency() );
			setIfNotEmpty( "focusBorderWidth", descriptor->focusDecoration()->getFocusBorderWidth() );
			setIfNotEmpty( "focusSrc", descriptor->focusDecoration()->getFocusSrc() );
			setIfNotEmpty( "selBorderColor", descriptor->focusDecoration()->getSelBorderColor() );
			setIfNotEmpty( "focusSelSrc", descriptor->focusDecoration()->getFocusSelSrc() );
		}
		vector<Parameter*>* params = descriptor->getParameters();
		vector<Parameter*>::iterator param;
		for (param = params->begin(); param != params->end(); param++) {
			setPropertyToPlayer( (*param)->getName(), (*param)->getValue() );
			delete (*param);
		}
		delete params;
	}
	return result;
}

void FormatterPlayerAdapter::setProperty( const std::string &propertyName, const std::string &propertyValue ) {
	APRINT_NCLP("<==> propertyName[%s], propertyValue[%s]",propertyName.c_str(),propertyValue.c_str());
	_properties[propertyName] = propertyValue;
}

const char *FormatterPlayerAdapter::toPlayerProperty( const std::string &propertyName ) {
	APRINT_PROPERTY("<==> propertyName[%s]",propertyName.c_str());
	if (propertyName == "background") {
		return "backgroundColor";
	} else if (propertyName == "transparency") {
		return "opacity";
	}
	return propertyName.c_str();
}

bool FormatterPlayerAdapter::setPropertyToPlayer( const std::string &name, const std::string &value ) {
	APRINT_NCLP("<==> name[%s] value[%s]",name.c_str(),value.c_str());
	APRINT_PROPERTY("<==> name[%s] value[%s]",name.c_str(),value.c_str());
	bool result = true;

	std::string propertyName = toPlayerProperty( name );
	prop::Type property = player::property::getProperty( propertyName.c_str() );
	std::string propValue = value;

	if (property != prop::null && value.empty()) {
		switch ( property ) {
			case prop::explicitDur:
			case prop::rgbChromakey:
			case prop::focusIndex:
			case prop::moveUp:
			case prop::moveDown:
			case prop::moveRight:
			case prop::moveLeft: {
				APRINT_PROPERTY("---");
				propValue = "nill";
				break;
			}
			default: {
				APRINT_PROPERTY("---");
				break;
			}
		}
	}

	if (propValue.empty()) {
		APRINT_PROPERTY("<== return[true]");
		return true;
	}
	setProperty( name, propValue );
	if (property != prop::null && !propValue.empty()) {
		switch ( property ) {
			case prop::top:
			case prop::bottom:
			case prop::left:
			case prop::right:
			case prop::width:
			case prop::height:
			case prop::bounds:
			case prop::location:
			case prop::size:
			case prop::zIndex: {
				APRINT_PROPERTY("---");
				result = object->setProperty( propertyName, propValue );
				_player->updateImage();
				break;
			}
			case prop::transIn:
			case prop::transOut: {
				APRINT_PROPERTY("---");
				std::vector<std::string> transIds;
				std::string value;
				std::string attValue = propValue;
				bptnt::TransitionBase* transitionBase;
				bptnt::Transition* transition;
				transitionBase = manager->getFormatterMediator()->getTransitionBase();
				if (transitionBase != NULL) {
					boost::split( transIds, attValue, boost::is_any_of( ";" ) );
					if (!transIds.empty()) {
						std::vector<std::string>::iterator i;
						i = transIds.begin();
						while (i != transIds.end()) {
							value = (*i);
							boost::trim( value );
							*i = value;
							transition = transitionBase->getTransition( value );
							if (transition != NULL) {
								result = object->getDescriptor()->getFormatterRegion()->addTransition( transition, property == prop::transIn );
								break;
							}
							++i;
						}
					}
					transIds.clear();
				}
				break;
			}
			case prop::explicitDur: {
				APRINT_PROPERTY("---");
				std::string param = propValue;
				CascadingDescriptor* descriptor = object->getDescriptor();
				result = descriptor->setExplicitDuration( param );
				break;
			}
			case prop::opacity: {
				APRINT_PROPERTY("---");
				if (ncl_util::isPercentualValue( propValue )) {
					result = _player->setProperty( propertyName, 1.0f - ncl_util::getPercentualValue( propValue ));
				} else {
					APPLY_INVERSE_PROPERTY( float, propertyName, propValue);
				}
				break;
			}
//			case prop::backgroundColor: {
//				if (propValue[0] != '#') {
//					propValue = ncl_util::intToHex ( boost::lexical_cast<int>( propValue ) );
//				}
//				result = _player->setProperty( propertyName, propValue );
//				break;
//			}
			case prop::focusBorderTransparency:
			case prop::balanceLevel:
			case prop::trebleLevel:
			case prop::bassLevel: {
				APRINT_PROPERTY("---");
				if (ncl_util::isPercentualValue( propValue )) {
					result = _player->setProperty( propertyName, ncl_util::getPercentualValue( propValue ) );
				} else {
					APPLY_PROPERTY( float, propertyName, propValue );
				}
				break;
			}
			case prop::soundLevel: {
				APRINT_PROPERTY("---");
				
				if (ncl_util::isPercentualValue( propValue )) {
					
					result = _player->setProperty( propertyName, ncl_util::getPercentualValue( propValue ) );
					
					if (_player->instanceOf( "SoundPlayer" ))
					{
						((player::SoundPlayer *)_player)->_sound->setSoundPropertiesLevel(ncl_util::getPercentualValue( propValue ));
					}
				}
				else
				{
					APPLY_PROPERTY( float, propertyName, propValue );

					if (_player->instanceOf( "SoundPlayer" ))
					{					
						((player::SoundPlayer *)_player)->_sound->setSoundPropertiesLevel(ncl_util::getDoubleValue(propValue)/100);					
					}
				}
				break;
			}			
			case prop::visible: {
				APRINT_NCLP( "visible[%s][%s]", propertyName.c_str(), propValue.c_str());
				APPLY_PROPERTY(bool, propertyName, propValue);
				break;
			}
			case prop::fontSize:
			case prop::focusBorderWidth: {
				std::string aux;
				aux = getProperSize( propValue );
				APPLY_PROPERTY( int, propertyName, aux );
				break;
			}
			case prop::focusSelSrc:
			case prop::focusSrc: {
				std::string url;
				if (getManager()->getUrl( propValue, url )) {
					result = _player->setProperty( propertyName, url );
				}
				break;
			}
			case prop::focusIndex: {
				object->getDescriptor()->setFocusIndex( propValue );
				result = true;
				break;
			}
			case prop::moveDown: {
				object->getDescriptor()->setMoveDown( propValue );
				result = true;
				break;
			}
			case prop::moveLeft: {
				object->getDescriptor()->setMoveLeft( propValue );
				result = true;
				break;
			}
			case prop::moveRight: {
				object->getDescriptor()->setMoveRight( propValue );
				result = true;
				break;
			}
			case prop::moveUp: {
				object->getDescriptor()->setMoveUp( propValue );
				result = true;
				break;
			}
			case prop::src: {
				//	Filter properties
				LWARN("FormatterPlayerAdapter", "property %s not supported", propertyName.c_str() );
				result = false;
				break;
			}
			default: {
				APRINT_NCLP( "visible[%s][%s]",propertyName.c_str(),propValue.c_str());
				result = _player->setProperty( propertyName, propValue );
				break;
			}
		};
	}

	LDEBUG("FormatterPlayerAdapter","set: property=%s, value=%s, result=%d", name.c_str(), propValue.c_str(), result );
	return result;
}

std::string FormatterPlayerAdapter::getProperSize( const std::string &value ){
	std::string aux;
	std::size_t i = 0;
	float number;

	aux = value;
	i = aux.find("pt",0);
	if (i!=string::npos) {
		aux = aux.substr(0,i);
		number = atoi(aux.c_str());
		number = number * FormatterPlayerAdapter::FONT_PT_TO_PX;
		aux = boost::lexical_cast<std::string>(int(number));
	} else {
		i = aux.find("px",0);
		if (i!= string::npos) {
			aux = aux.substr(0,i);
		}
	}

	return aux;
}

bool FormatterPlayerAdapter::setCurrentEvent( FormatterEvent* /*event*/) {
	return false;
}

bool FormatterPlayerAdapter::setPropertyValue( AttributionEvent* event, const std::string &value ) {

	std::string propName = (event->getAnchor())->getPropertyName();
	if (!setPropertyToPlayer( propName, value )) {
		if (this->instanceOf( "ApplicationPlayerAdapter" )) {
			((ApplicationPlayerAdapter*) this)->setCurrentEvent( event );
		}
	}

	if (!this->instanceOf( "ApplicationPlayerAdapter" )) {
		event->stop();
	}
	return true;
}

bool FormatterPlayerAdapter::startAnimation( AttributionEvent* event, const std::string &value, bptna::Animation* animation ) {
	APRINT_NCLP("==> value[%s] duration[%s]",value.c_str(),animation->getDuration().c_str());

	double duration = 0;
	double by = 0;

	double startValueLeft =0, startValueTop=0, endValueLeft=0, endValueTop=0;
	double startValueWidth=0 , startValueHeight=0, endValueWidth=0, endValueHeight=0;

	try {
		duration = boost::lexical_cast<float>( animation->getDuration() );
		//duration = getTimeValue( const std::string &value, bool throwException ) 
	} catch (boost::bad_lexical_cast &e) {
		LWARN("FormatterPlayerAdapter", "Conversion error duration[%s]",animation->getDuration().c_str());
		APRINT_NCLP("ERROR/WARN could not convert duration [%s]",animation->getDuration().c_str());
	}

	try{
		by = boost::lexical_cast<double>( animation->getBy() );
	}
	catch (boost::bad_lexical_cast &e) {
		LWARN("FormatterPlayerAdapter", "Conversion error by[%s]",animation->getBy().c_str());
		APRINT_NCLP("ERROR could not convert _by_[%s]",animation->getBy().c_str());
	}
	APRINT_NCLP("duration[%f] by[%f]",duration,by);

	std::string property = toPlayerProperty ( event->getAnchor()->getPropertyName() );
	LayoutRegion* region = object->getDescriptor()->getFormatterRegion()->getLayoutRegion();
	double startValue = 0;
	prop::Type propertyName = player::property::getProperty( property.c_str() );

	if (propertyName != prop::null) {
		switch ( propertyName ) {
			case prop::top: {
				startValue = ( double ) region->getTop();
				break;
			}
			case prop::bottom: {
				startValue = ( double ) region->getBottom();
				break;
			}
			case prop::left: {
				startValue = ( double ) region->getLeft();
				break;
			}
			case prop::right: {
				startValue = ( double ) region->getRight();
				break;
			}
			case prop::width: {
				startValue = ( double ) region->getWidth();
				break;
			}
			case prop::height: {
				startValue = ( double ) region->getHeight();
				break;
			}
			case prop::zIndex: {
				startValue = ( double ) region->getZIndex();
				break;
			}
			case prop::opacity: {
				std::string temp = getPropertyValue( event );

				boost::algorithm::trim( temp );
				temp = ( temp.length() == 0 ? "0.0" : temp);

				if (ncl_util::isPercentualValue( temp )) {
					startValue = ncl_util::getPercentualValue ( temp );
				}
				else {
					try{
						startValue = boost::lexical_cast < double >( temp );
					} catch (boost::bad_lexical_cast &e) {
						LWARN("FormatterPlayerAdapter", "Conversion error opacity start value [%s]", temp.c_str());
					}
				}
				break;
			}
			case prop::backgroundColor: {
				std::string temp = getPropertyValue( event );
				startValue = ( double ) ncl_util::webRGBtoInt( temp );
				break;
			}
			case prop::rgbChromakey: {
				APRINT_NCLP("rgbChromakey");
				std::string temp = getPropertyValue( event );
				startValue = ( double ) ncl_util::webRGBtoInt( temp );
				break;
			}
			case prop::location: {
				startValueLeft = ( double ) region->getLeft();
				startValueTop = ( double ) region->getTop();
			}break;
			case prop::size: {
				startValueWidth = ( double ) region->getWidth();
				startValueHeight = ( double ) region->getHeight();
			}break;
			case prop::bounds: {
				APRINT_NCLP("bounds");
				startValueLeft = ( double ) region->getLeft();
				startValueTop = ( double ) region->getTop();
				startValueWidth = ( double ) region->getWidth();
				startValueHeight = ( double ) region->getHeight();
			}break;
			case prop::fontColor: {
				APRINT_NCLP("fontColor");
				std::string temp = getPropertyValue( event );
				if (temp[0]!='#'){
					APRINT_NCLP("getRGBColor");
					temp = ncl_util::getRGBColor(temp);
				}
				startValue = ( double ) ncl_util::webRGBtoInt( temp );
				APRINT_NCLP("startValue[%s][%f]",temp.c_str(),startValue);
			}break;
			case prop::fontSize: {
				APRINT_NCLP("fontSize");
				std::string temp = getPropertyValue( event );
				startValue = ( double ) ncl_util::getDoubleValue( temp );
				APRINT_NCLP("startValue[%s][%f]",temp.c_str(),startValue);
			}break;
				//sound
			case prop::soundLevel: 
			case prop::balanceLevel: 
			case prop::trebleLevel: 
			case prop::bassLevel: {
				APRINT_NCLP("soundAnimation[%s]",property.c_str());
				std::string temp = getPropertyValue( event );
				boost::algorithm::trim( temp );
				temp = ( temp.length() == 0 ? "0.0" : temp);
				if (ncl_util::isPercentualValue( temp )) {
					startValue = ncl_util::getPercentualValue ( temp );
				}
				else {
					startValue = ( double ) ncl_util::getDoubleValue( temp );
				}//isPecentual
			}break;
			default: {
				LWARN("FormatterPlayerAdapter", "property %s not supported in animation", property.c_str() );
				APRINT_NCLP("<== Return[false] property[%s] not supported in animation", property.c_str() );
				return false;
			}//default
		}//switch
	}//if prop not null

	double endValue = 0.0;

	if (propertyName == prop::backgroundColor || propertyName == prop::fontColor || 
				propertyName == prop::rgbChromakey) {
		APRINT_NCLP("webRGB");
		endValue = ( double ) ncl_util::webRGBtoInt( value );
	}else if (( propertyName == prop::opacity) ||
				( propertyName == prop::soundLevel || propertyName == prop::trebleLevel ||
				propertyName == prop::balanceLevel || propertyName == prop::bassLevel )){
			if (ncl_util::isPercentualValue( value )) {
				endValue = ncl_util::getPercentualValue ( value );
			} else {
				try{
					endValue = boost::lexical_cast < double >( value );
				} catch (boost::bad_lexical_cast &e) {
					LWARN("FormatterPlayerAdapter", "Conversion error opacity end value [%s]", value.c_str());
				}//catch
			}//isPercentual

	}else if (propertyName == prop::location){
		std::string l,t;
		APRINT_NCLP("location");
		ncl_util::splitString( value,",",&l,&t);
		APRINT_NCLP("endValue[%s] left[%s] top[%s]",value.c_str(),l.c_str(),t.c_str());
		try{
			endValueLeft = boost::lexical_cast<double>(l);
			endValueTop = boost::lexical_cast<double>(t);
		}catch(boost::bad_lexical_cast &e){
			LWARN("FormatterPlayerAdapter", "Conversion error location left[%s] top[%s]", l.c_str(),t.c_str());
			APRINT_ERROR("Conversion error location left[%s] top[%s]", l.c_str(),t.c_str());
		}
  
	}else if (propertyName == prop::size){
		std::string w,h;
		APRINT_NCLP("size");
		ncl_util::splitString( value,",",&w,&h);
		APRINT_NCLP("endValue[%s] width[%s] height[%s]",value.c_str(),w.c_str(),h.c_str());

		try{
			//TODO: check all places where  we have to use ncl::getPixel instead of
			//just call boost lexical cast
			endValueWidth = ncl_util::getPixelValue(w,true);
			endValueHeight = ncl_util::getPixelValue(h,true);
		}catch(boost::bad_lexical_cast &e){
			LWARN("FormatterPlayerAdapter", "Conversion error location left[%s] top[%s]", w.c_str(),h.c_str());
			APRINT_ERROR("Conversion error location left[%s] top[%s]", w.c_str(),h.c_str());
		}//catch
	}else if (propertyName == prop::bounds){
		std::string l,t;
		std::string w,h;
		APRINT_NCLP("location");
		ncl_util::splitString( value,",",&l,&t,&w,&h);
		APRINT_NCLP("endValue[%s] left[%s] top[%s]",value.c_str(),l.c_str(),t.c_str());
		APRINT_NCLP("endValue[%s] width[%s] height[%s]",value.c_str(),w.c_str(),h.c_str());
		try{
			endValueLeft = boost::lexical_cast<double>(l);
			endValueTop = boost::lexical_cast<double>(t);
			endValueWidth = boost::lexical_cast<double>(w);
			endValueHeight = boost::lexical_cast<double>(h);
		}catch(boost::bad_lexical_cast &e){
			LWARN("FormatterPlayerAdapter", "Conversion error location left[%s] top[%s]", l.c_str(),t.c_str());
		}
  
	}else {
			//general case
			try{
				endValue = startValue + duration;
				endValue = boost::lexical_cast < double > ( value );
			} catch (boost::bad_lexical_cast &e) {
				LWARN("FormatterPlayerAdapter", "Conversion error by[%s]",value.c_str());
				APRINT_NCLP("ERROR could not convert __endValue__");
		}
	}//properTuName==

	APRINT_NCLP("---");
	bptgnan::Animation *a = new bptgnan::Animation( duration, by, _sys );
	if (propertyName== prop::location){
		APRINT_NCLP("location");
		a->addProperty( "left", startValueLeft, endValueLeft );
		a->addProperty( "top", startValueTop, endValueTop );
	}else if ( propertyName== prop::size){
		APRINT_NCLP("size");
		a->addProperty( "width", startValueWidth, endValueWidth );
		a->addProperty( "height", startValueHeight, endValueHeight );
	}else if ( propertyName== prop::bounds){
		APRINT_NCLP("bounds");
		a->addProperty( "left", startValueLeft, endValueLeft );
		a->addProperty( "top", startValueTop, endValueTop );
		a->addProperty( "width", startValueWidth, endValueWidth );
		a->addProperty( "height", startValueHeight, endValueHeight );
	}else{
		//general case
		APRINT_NCLP("generalCase");
		a->addProperty( property, startValue, endValue );
	}
	a->onEnd( boost::bind( &AttributionEvent::stop, event ) );
	APRINT_NCLP("<== <-- startAnimation()");
	return object->getDescriptor()->getFormatterRegion()->startAnimation( a );
}

bool FormatterPlayerAdapter::hasPrepared() {
	FormatterEvent* mEv;
	short st;
	/*
	 if (object == NULL || player == NULL) {
	 return false;
	 }

	 presented = player->hasPresented();
	 if (presented) {
	 return false;
	 }

	 presented = player->isForcedNaturalEnd();
	 if (presented) {
	 return false;
	 }
	 */
	if (object != NULL) {
		mEv = object->getMainEvent();
		if (mEv != NULL && !object->instanceOf( "ApplicationExecutionObject" )) {
			st = mEv->getCurrentState();
			if (st != EventUtil::ST_SLEEPING) {
				return true;
			}
		}
	}
	return false;
}

bool FormatterPlayerAdapter::setKeyHandler( bool isHandler ) {
	_isKeyhandler = isHandler;
	return isHandler;
}

bool FormatterPlayerAdapter::prepare( ExecutionObject* object, FormatterEvent* event ) {
	CascadingDescriptor* descriptor;
	bool result;
	double explicitDur=10;

	if (hasPrepared()) {
		LWARN("FormatterPlayerAdapter", "prepare, returns false, because player is already prepared" );
		return false;
	}
	/*
	 if (!lockObject()) {
	 if (player != NULL) {
	 if (player->hasPresented() || player->isForcedNaturalEnd()) {
	 while (!lockObject());

	 } else {
	 return false;
	 }
	 }
	 }*/

	if (object == NULL) {
		LWARN("FormatterPlayerAdapter", "Trying to prepare a NULL object." );
		return false;
	}

	this->object = object;
	result = createPlayer();

	if (event->instanceOf( "PresentationEvent" )) {
		if (_player != NULL && _player->instanceOf( "SoundPlayer" ))
		{
			setObject( object );
		}
		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			std::string duration = descriptor->getExplicitDuration();
			if (duration != "nill") {
				duration.erase(std::remove_if(duration.begin(), duration.end(), ncl_util::is_alpha_or_space()), duration.end());

				try{
					explicitDur = boost::lexical_cast<double>( duration ) * 1000;
				}
				catch (boost::bad_lexical_cast &) {
					LWARN("FormatterPlayerAdapter", "Conversion error [%s]", duration.c_str());
				}

				object->removeEvent( event );
				bptnt::Transition* transition = descriptor->getFormatterRegion()->getTransOut();
				double dur = 0;
				if (transition) {
					dur = std::min(transition->getDur(), explicitDur);
				}
				static_cast<PresentationEvent*>(event)->setEnd( explicitDur - dur );
				object->addEvent( event );
			}
		}
	}

	if (event->instanceOf("AnchorEvent") && static_cast<AnchorEvent*>(event)->getAnchor()->instanceOf("TextAnchor")) {
		TextAnchor* anchor = static_cast<TextAnchor*>(static_cast<AnchorEvent*>(event)->getAnchor());
		if (static_cast<AnchorEvent*>(event)->getAnchor()->instanceOf("DirectTextAnchor")) {
			std::pair<std::string, std::string> param = std::make_pair( "_beginText", anchor->getText() );
			getPlayer()->setProperty( "attributionEvent", param );
			std::pair<std::string, std::string> value = std::make_pair( "_beginPosition", anchor->getPosition() );
			getPlayer()->setProperty( "attributionEvent", value );
		} else if (static_cast<AnchorEvent*>(event)->getAnchor()->instanceOf("ReverseTextAnchor")) {
			std::pair<std::string, std::string> param = std::make_pair( "_endText", anchor->getText() );
			getPlayer()->setProperty( "attributionEvent", param );
			std::pair<std::string, std::string> value = std::make_pair( "_endPosition", anchor->getPosition() );
			getPlayer()->setProperty( "attributionEvent", value );
		}
	}

	if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
		object->prepare( (PresentationEvent*) event, 0 );
		return result;
	} else {
		return false;
	}
}

bool FormatterPlayerAdapter::start() {
	APRINT_NCLP("==>");


	//check if the parent context is visible
	bool visible = _player->isVisible() && getObject()->getParentsVisibility();

	APRINT_NCLP("isVisible[%d]",visible);

	if (object != NULL) {
		if (object->canStart()) {
			if (visible && object->getDescriptor()->getFormatterRegion()->haveTransitionIn()) {
				APRINT_NCLP("setplayer Visible[false]");
				_player->setProperty( "visible", false );
				APRINT_NCLP("visible=true");
				visible = true;
				object->setTransitionInVisible(true);
			}
			double seek = object->getExpectedStartTime();
			
			double duration = object->getExpectedDurationTime();
			if (_player->play(seek)) {
				//if(duration > 0)
				//LGESP-870
				if(!ncl_util::isInfinity(duration) && duration > 0)
				{
					_player->createTimer( (util::DWORD) duration*1000, boost::bind( &PlayerAdapterManager::onStopTimer, getManager(), getObject(), false ) );
				}
				util::key::Keys keys;
				#define SAVE_KEY( c, n, g ) keys.insert( static_cast<util::key::type>(util::key::c) );
				UTIL_KEY_LIST(SAVE_KEY);
				#undef SAVE_KEY
				player::input::InputCallback fnc = boost::bind( &br::pucrio::telemidia::ginga::ncl::adapters::FormatterPlayerAdapter::userEventReceived, this, _1, _2 );
				_sys->addInputListener( this, fnc, keys );
				object->start();
				processTransitions();
				object->getDescriptor()->getFormatterRegion()->transitionIn();
				APRINT_NCLP("setplayer Visible[%d]",visible);
				_player->setProperty( "visible", visible );
				object->setTransitionInVisible(false);

				FormatterEvent *event = object->getMainEvent();
				if (event->instanceOf( "PresentationEvent" )) {
					AnchorEvent *anchorEvent = static_cast<AnchorEvent*>(event);
					if (anchorEvent->getAnchor()->instanceOf( "LabeledAnchor" )) {
						std::string interfaceId = static_cast<LabeledAnchor*>(anchorEvent->getAnchor())->getLabel();
						if (!interfaceId.empty()) {
							getPlayer()->setProperty( "presentationEvent", interfaceId );
						}
					}
				}


				return true;
			}

			object->setTransitionInVisible(false);
		}
	}
	return false;
}

void FormatterPlayerAdapter::processTransitions( void ) {
	if (object != NULL) {
		EventTransition* t = object->getNextTransition();
		if (t != NULL && !ncl_util::isInfinity( t->getTime() )) {
			_player->createTimer( (util::DWORD) t->getTime(), boost::bind( &PlayerAdapterManager::onTransitionTimer, getManager(), getObject(), t->getTime() ) );
		}
	}
}

bool FormatterPlayerAdapter::stop( bool proccessTransitions /*=true*/) {

	if (proccessTransitions && object->getDescriptor()->getFormatterRegion()->haveTransitionOut()) {
		object->getDescriptor()->getFormatterRegion()->transitionOut();
		return false;
	}

	FormatterEvent* mainEvent;
	vector<FormatterEvent*>* events = NULL;

	stopPlayer();

	if (object == NULL) {
		return false;

	} else {
		mainEvent = object->getMainEvent();
		events = object->getEvents();
	}

	if (mainEvent != NULL && mainEvent->instanceOf( "PresentationEvent" )) {
		if (checkRepeat( (PresentationEvent*) mainEvent )) {
			return true;
		}
	}

	if (events != NULL) {
		vector<FormatterEvent*>::iterator i;
		i = events->begin();
		while (i != events->end()) {
			if (*i != NULL && (*i)->instanceOf( "AttributionEvent" )) {
				((AttributionEvent*) (*i))->setValueMaintainer( NULL );
			}
			//LGESP-820
			else if (*i != NULL && (*i)->instanceOf( "FormatterEvent" )) {
				((FormatterEvent*) (*i))->stop();
			}

			if (i != events->end()) {
				++i;
			}
		}
		delete events;
		events = NULL;
	}

	if (object != NULL) {
		object->getDescriptor()->getFormatterRegion()->unprepareOutputDisplay();
		object->stop();
		object->unprepare();
		return true;
	}
	return false;
}

bool FormatterPlayerAdapter::pause() {
	if (object != NULL && object->pause()) {
		_player->pause( true );
		//TODO player->notifyReferPlayers(EventUtil::TR_PAUSES);
		return true;

	} else {
		return false;
	}
}

bool FormatterPlayerAdapter::resume() {
	if (object != NULL && object->resume()) {
		_player->pause( false );
		// player->notifyReferPlayers(EventUtil::TR_RESUMES);
		return true;
	}
	return false;
}

bool FormatterPlayerAdapter::abort() {
	return object != NULL && object->abort();
}

bool FormatterPlayerAdapter::checkRepeat( PresentationEvent* event ) {
	if (event->getRepetitions() > 1) {
		_player->stop();
		//TODO player->notifyReferPlayers(EventUtil::TR_STOPS);
		if (object != NULL) {
			object->stop();
		}
		//player->setImmediatelyStart(true);
		return true;
	}
	return false;
}

string FormatterPlayerAdapter::getPropertyValue( AttributionEvent* event ) {
	std::string value = "";
	std::string name = event->getAnchor()->getPropertyName();
	if (event != NULL) {
		value = _properties[name];
	}
	if (value == ""){
		// Workaround: somehow, the value is float and not string..
		if (name == "soundLevel" || name == "balanceLevel" || name == "trebleLevel" || name == "bassLevel")
		{
			player::Property *p = _player->getProperty(name);
			if (p){
				player::PropertyImpl<float> *prop = static_cast<player::PropertyImpl<float>*>(p);
				value = boost::lexical_cast<std::string>(prop->getValue());
			}//if p
		}
		else {
			player::Property *p = _player->getProperty(name);
			if (p){
				player::PropertyImpl<std::string> *prop = static_cast<player::PropertyImpl<std::string>*>(p);
				value = (std::string) prop->getValue();
			}//if p
		}
 	}//value empty
	return value;
}

util::DWORD FormatterPlayerAdapter::uptime() const {
	return _player->uptime();
}

void FormatterPlayerAdapter::userEventReceived( ::util::key::type key, bool isUp ) {
	APRINT_NCLP( "<==>" );
	LWARN("FormatterPlayerAdapter", "userEventReceived" );

	if (object) {
		bool handled = false;
		if (!_keyToFormatter &&  (isKeyHandler() || _keyToPlayer)) {
			_player->dispatchKey( key, isUp );
			handled = true;
		} else if (!isUp) {
			object->selectionEvent(key, uptime());
			handled = true;
		}
		if ( (handled || isUp) && !impl::_onKey.empty()) {
			impl::_onKey( key );
 		}
 	}

	_keyToPlayer = false;
	_keyToFormatter = false;
}

void FormatterPlayerAdapter::timeShift( const std::string &/*direction */) {
}

bool FormatterPlayerAdapter::isKeyHandler( void ) const {
	return _isKeyhandler;
}

void FormatterPlayerAdapter::onKeyPress( const FormatterPlayerAdapter::KeyPressCallback &callback ) {
	impl::_onKey = callback;
}

void FormatterPlayerAdapter::setPlayerKeyHandler() {
	_keyToPlayer = true;
}

void FormatterPlayerAdapter::setFormatterKeyHandler() {
	_keyToFormatter = true;
}

}
}
}
}
}
}
