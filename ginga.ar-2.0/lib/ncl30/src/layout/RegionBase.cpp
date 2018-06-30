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

#include "../../include/layout/RegionBase.h"
#include "../../include/layout/LayoutRegion.h"
#include <boost/lexical_cast.hpp>
#include <util/log.h>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
RegionBase::RegionBase( const std::string &id ) :
		Base( id ) {
	this->regions = new std::map<std::string, LayoutRegion*>;
	this->outputMapRegionId = "";
	this->deviceRegion = NULL;

	typeSet.insert( "RegionBase" );
}

RegionBase::~RegionBase() {
	if (regions != NULL) {
		regions->clear();
		delete regions;
		regions = NULL;
	}
	if (deviceRegion != NULL) {
		delete deviceRegion;
		deviceRegion = NULL;
	}

}

bool RegionBase::addRegion( LayoutRegion* region ) {
	APRINT_NCL3("==> m_id[%s] region[%s]",getId().c_str(),region->getId().c_str());
	std::map<std::string, LayoutRegion*>::iterator i;
	std::string regId;

	regId = region->getId();

	i = regions->find( regId );
	if (i != regions->end()) {
		LWARN("RegionBase", "Trying to add a region with id '%s' twice ", regId.c_str());
		APRINT_NCL3("<== m_id[%s] region[%s]",getId().c_str(),region->getId().c_str());
		return false;

	} else {
		deviceRegion->addRegion( region );
		region->setParent( deviceRegion );
		(*regions)[regId] = region;
		APRINT_NCL3("<== m_id[%s] region[%s]",getId().c_str(),region->getId().c_str());
		return true;
	}
}

bool RegionBase::addBase( Base* base, const std::string &alias, const std::string &location ) {
	APRINT_NCL3("==>m_id[%s] base[%s] alias[%s]",getId().c_str(),base->getId().c_str(),alias.c_str());
	if (base->instanceOf( "RegionBase" )) {
		APRINT_NCL3("<== m_id[%s] base[%s] alias[%s]",getId().c_str(),base->getId().c_str(),alias.c_str());
		return Base::addBase( base, alias, location );
	}
	APRINT_NCL3("<== FALSE m_id[%s] base[%s] alias[%s]",getId().c_str(),base->getId().c_str(),alias.c_str());
	return false;
}
bool RegionBase::addBase( Base* base){
	APRINT_NCL3("<==> base[%s]", base->getId().c_str());
	return addBase(base,base->getBaseAlias(base),base->getBaseLocation(base));
}

LayoutRegion* RegionBase::getRegionLocally( const std::string &regionId ) {
	APRINT_NCL3("==>m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
	std::map<std::string, LayoutRegion*>::iterator childRegions;
	LayoutRegion *region;
	LayoutRegion *auxRegion;

	childRegions = regions->begin();
	while (childRegions != regions->end()) {
		APRINT_NCL3("---");
		region = childRegions->second;
		if (region->getId() == regionId) {
			APRINT_NCL3("<==m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
			return region;
		}
		auxRegion = region->getRegionRecursively( regionId );
		if (auxRegion != NULL) {
			APRINT_NCL3("<==m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
			return auxRegion;
		}
		++childRegions;
	}
	APRINT_NCL3("<== NULL m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
	return NULL;
}

const std::string &RegionBase::getOutputMapRegionId() {
	return outputMapRegionId;
}

LayoutRegion* RegionBase::getRegion( const std::string &regionId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	RegionBase* base;
	LayoutRegion * retreg;

	index = regionId.find_first_of( "#" );
	if (index == std::string::npos) {
		//no alias, get locally
		APRINT_NCL3(" NO ALIAS m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
		LayoutRegion * retRegion = getRegionLocally( regionId );
		if (retRegion) {
			return retRegion;
		}
		// transversion code moved to the latest elseif below.
	}

	//break it down to prefix (alias) and sufix (regionId)
	APRINT_NCL3("--- m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
	prefix = regionId.substr( 0, index );
	index++;
	suffix = regionId.substr( index, regionId.length() - index );
	APRINT_NCL3("sufix[%s]",suffix.c_str());

	//search for alias first
	if (baseAliases->find( prefix ) != baseAliases->end()) {
		base = (RegionBase*) ((*baseAliases)[prefix]);
		APRINT_NCL3("<== getRegion() m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
		return base->getRegion( suffix );

	} else if (baseLocations->find( prefix ) != baseLocations->end()) {
		base = (RegionBase*) ((*baseLocations)[prefix]);
		APRINT_NCL3("<== getRegion() m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
		return base->getRegion( suffix );

	} else {
		//no alias ?
		//transverse anywayss!
		std::map<std::string, Base*>::iterator it;

		for (it = baseAliases->begin(); it != baseAliases->end(); it++) {

			APRINT_NCL3("alias[%s]",(it->first).c_str());
			base = static_cast<RegionBase*>( it->second );
			retreg = base->getRegion( suffix );
			if (retreg) {
				APRINT_NCL3("got it");
				return retreg;
			}
		}
		APRINT_NCL3("<== NULL m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
		return NULL;
	}

	//this code is unreacheable, but to avoid future errors
	//placing it anywayss
	APRINT_NCL3("<== NULL m_id[%s] region[%s]",getId().c_str(),regionId.c_str());
	return NULL;
}

LayoutRegion* RegionBase::getLayout() const {
	return deviceRegion;
}

std::vector<LayoutRegion*>* RegionBase::getRegions() {
	APRINT_NCL3("==>");
	std::map<std::string, LayoutRegion*>::iterator i;
	std::vector<LayoutRegion*>* childRegions;
	childRegions = new std::vector<LayoutRegion*>;
	for (i = regions->begin(); i != regions->end(); ++i) {
		childRegions->push_back( i->second );
	}
	APRINT_NCL3("<==");
	return childRegions;
}

bool RegionBase::removeRegion( LayoutRegion* region ) {
	std::map<std::string, LayoutRegion*>::iterator i;
	for (i = regions->begin(); i != regions->end(); ++i) {
		if (i->second == region) {
			regions->erase( i );
			return true;
		}
	}
	return false;
}

void RegionBase::setDevice( Device device, const std::string &regionId ) {
	_device = device;
	outputMapRegionId = regionId;

	if (deviceRegion != NULL) {
		delete deviceRegion;
		deviceRegion = NULL;
	}
	deviceRegion = new LayoutRegion( _device.name );
	deviceRegion->setDevice( _device );
}

int RegionBase::getDeviceClass() {
	return _device.number;
}

void RegionBase::clear() {
	regions->clear();
	Base::clear();
}
bool RegionBase::absorbRegionBase(RegionBase *rb){
  //ou add base
  APRINT_NCL3("==>");
  std::vector<LayoutRegion*>* eregions = rb->getRegions();
  std::vector<LayoutRegion*>::iterator it; 
  //std::map<std::string, LayoutRegion*>::iterator i;
  for (it = eregions->begin(); it != eregions->end(); ++it) {
    APRINT_NCL3("absording region");
    addRegion((*it));  
  }

  APRINT_NCL3("<==");
  return true;
}

}
}
}
}
}
