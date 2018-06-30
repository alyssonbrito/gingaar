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

#include "../include/NclDocument.h"
#include "../include/layout/RegionBase.h"
#include "../include/descriptor/DescriptorBase.h"
#include "../include/switches/RuleBase.h"
#include "../include/transition/TransitionBase.h"
#include "../include/connectors/ConnectorBase.h"
#include "../include/components/ContextNode.h"
#include "../include/metainformation/Metadata.h"
#include "../include/metainformation/Meta.h"
#include <util/log.h>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
NclDocument::NclDocument( const std::string & idParam ) :
		body( NULL ),
		connectorBase( NULL ),
		descriptorBase( NULL ),
		documentAliases( NULL ),
		documentBase( NULL ),
		documentLocations( NULL ),
		id( idParam ),
		regionBases( new std::map<int, bptnl::RegionBase*> ),
		ruleBase( NULL ),
		transitionBase( NULL ),
		metainformationList( new std::vector<bptnm::Meta*> ),
		metadataList( new std::vector<bptnm::Metadata*> ) {
	APRINT_NCL3("<==>");

}

NclDocument::~NclDocument() {
	if (regionBases != NULL) {
		std::map<int, bptnl::RegionBase*>::iterator it;
		for (it = regionBases->begin(); it != regionBases->end(); ++it)
			delete it->second;
		regionBases->clear();
		delete regionBases;
		regionBases = NULL;
	}
	if (metadataList != NULL) {
		std::vector<bptnm::Metadata*>::iterator it;
		for (it = metadataList->begin(); it != metadataList->end(); ++it)
			delete (*it);
		metadataList->clear();
		delete metadataList;
		metadataList = NULL;
	}
	if (metainformationList != NULL) {
		std::vector<bptnm::Meta*>::iterator it;
		for (it = metainformationList->begin(); it != metainformationList->end(); ++it)
			delete (*it);
		metainformationList->clear();
		delete metainformationList;
		metainformationList = NULL;
	}
	if (documentBase != NULL) {
		std::vector<NclDocument*>::iterator it;
		for (it = documentBase->begin(); it != documentBase->end(); ++it)
			delete (*it);
		documentBase->clear();
		delete documentBase;
		documentBase = NULL;
	}
	if (documentAliases != NULL) {
		documentAliases->clear();
		delete documentAliases;
		documentAliases = NULL;
	}
	if (documentLocations != NULL) {
		documentLocations->clear();
		delete documentLocations;
		documentLocations = NULL;
	}

	if (descriptorBase != NULL) {
// 		descriptorBase->clear();
		delete descriptorBase;
		descriptorBase = NULL;
	}


	if(transitionBase != NULL) {
		delete	transitionBase;
		transitionBase = NULL;
	}

	if(connectorBase != NULL) {
		delete	connectorBase;
		connectorBase = NULL;
	}

	if (body != NULL) {
		delete body;
		body = NULL;
	}
}

bool NclDocument::addDocument( NclDocument* document, const std::string & alias, const std::string & location ) {
	APRINT_NCL3("==> alias[%s] location[%s]",alias.c_str(),location.c_str());
	if (document == NULL) {
		APRINT_NCL3("<== alias[%s]",alias.c_str());
		return false;
	}

	if (documentBase == NULL) {
		documentBase = new std::vector<NclDocument*>;
		documentAliases = new std::map<std::string, NclDocument*>;
		documentLocations = new std::map<std::string, NclDocument*>;
	}

	if (documentAliases->find( alias ) != documentAliases->end() || documentLocations->find( location ) != documentLocations->end()) {
		APRINT_NCL3("<== alias[%s]",alias.c_str());
		return false;
	}

	documentBase->push_back( document );
	(*documentAliases)[alias] = document;
	(*documentLocations)[location] = document;
	APRINT_NCL3("<== alias[%s]",alias.c_str());
	return true;
}

void NclDocument::clear() {
	id = "";

	if (documentBase != NULL) {
		documentBase->clear();
		documentAliases->clear();
		documentLocations->clear();
		documentBase = NULL;
		documentAliases = NULL;
		documentLocations = NULL;
	}

	if (descriptorBase != NULL) {
// 			descriptorBase->clear();
		delete descriptorBase;
		descriptorBase = NULL;
	}

	if (regionBases != NULL) {
		regionBases->clear();
		delete regionBases;
		regionBases = NULL;
	}

	if (ruleBase != NULL) {
		ruleBase->clear();
		ruleBase = NULL;
	}

	if (transitionBase != NULL) {
		transitionBase->clear();
		transitionBase = NULL;
	}

	if (connectorBase != NULL) {
		connectorBase->clear();
		connectorBase = NULL;
	}

	if (metainformationList != NULL) {
		metainformationList->clear();
	}

	if (metadataList != NULL) {
		metadataList->clear();
	}
}
bptncon::Connector* NclDocument::getConnector( const std::string & connectorId ) {
	bptncon::Connector* connector;
	std::vector<NclDocument*>::iterator i;
	std::string::size_type index;
	std::string prefix, suffix;
	std::string location;
	NclDocument *document;

	if (connectorBase != NULL) {
		connector = connectorBase->getConnector( connectorId );
		if (connector != NULL) {
			return connector;
		}
	}

	if (documentBase != NULL) {
		for (i = documentBase->begin(); i != documentBase->end(); ++i) {
			connector = (*i)->getConnector( connectorId );
			if (connector != NULL) {
				return connector;
			}
			else{
				//check if it is a connector base imported from importNCL
                                // that case the connector base was not imported yet. If so
                                // add this Connector base into Document and then try find the connectorid again
				index = connectorId.find_first_of( "#" );
				if (index != std::string::npos) { 
					prefix = connectorId.substr( 0, index );
					index++;
					suffix = connectorId.substr( index, connectorId.length() - index );	
					if (documentAliases->find( prefix ) != documentAliases->end()) {
						document =   (*documentAliases)[prefix];
						location = getDocumentLocation(document);						
						bptncon::ConnectorBase* base = (*i)->getConnectorBase();
						base->addBase(base,prefix,location);
						connector = (*i)->getConnector( connectorId );
						if (connector != NULL) {
							return connector;
						}
					}	
				}
			}
		}
	}

	return NULL;
}


bptncon::ConnectorBase* NclDocument::getConnectorBase() {
	return connectorBase;
}

bptnt::Transition* NclDocument::getTransition( const std::string & transitionId ) {
	bptnt::Transition* transition;
	int i, size;
	NclDocument* document;

	if (transitionBase != NULL) {
		transition = transitionBase->getTransition( transitionId );
		if (transition != NULL) {
			return transition;
		}
	}

	if (documentBase != NULL) {
		size = documentBase->size();
		for (i = 0; i < size; i++) {
			document = (*documentBase)[i];
			transition = document->getTransition( transitionId );
			if (transition != NULL) {
				return transition;
			}
		}
	}

	return NULL;
}

bptnt::TransitionBase* NclDocument::getTransitionBase() {
	NclDocument* document;
	int i, size;
	/*TODO: this implementation will not work if NCL Documents
	define are more than 1 TransitionBases*/			
	if (documentBase != NULL) {
		size = documentBase->size();
		for (i = 0; i < size; i++) {
			document = (*documentBase)[i];
			if (document->transitionBase != NULL){
				return document->transitionBase;
			}
		}
	}
	return transitionBase;
}


bptnd::GenericDescriptor* NclDocument::getDescriptor( const std::string & descriptorId ) {
	bptnd::GenericDescriptor* descriptor;
	std::vector<NclDocument*>::iterator i;
	std::string::size_type index;
	std::string prefix, suffix;
	std::string locationSTR;
	NclDocument *documentV;

	if (descriptorBase != NULL) {
		descriptor = descriptorBase->getDescriptor( descriptorId );
		if (descriptor != NULL) {
			return descriptor;
		}
	}

	if (documentBase != NULL) {
		for (i = documentBase->begin(); i != documentBase->end(); ++i) {
			descriptor = (*i)->getDescriptor( descriptorId );
			if (descriptor != NULL) {
				return descriptor;
			}else{
				index = descriptorId.find_first_of( "#" );
				if (index != std::string::npos) {
					prefix = descriptorId.substr( 0, index );
					index++;
					suffix = descriptorId.substr( index, descriptorId.length() - index );	
					if (documentAliases->find( prefix ) != documentAliases->end()) {
						documentV =   (*documentAliases)[prefix];
						locationSTR = getDocumentLocation(documentV);						
						bptnd::DescriptorBase* Dbase = (*i)->getDescriptorBase();
						Dbase->addBase(Dbase,prefix,locationSTR);
						descriptor = (*i)->getDescriptor( descriptorId );
						if (descriptor != NULL) {
							return descriptor;
						}
					}	
				}
			}
		}
	}
	return NULL;
}

bptnd::DescriptorBase* NclDocument::getDescriptorBase() {
	return descriptorBase;
}

NclDocument* NclDocument::getDocument( const std::string & documentId ) {
	std::vector<NclDocument*>::iterator i;

	if (documentBase == NULL) {
		return NULL;
	}

	for (i = documentBase->begin(); i != documentBase->end(); ++i) {
		if ((*i)->getId() != "" && (*i)->getId() == documentId) {
			return (*i);
		}
	}
	return NULL;
}

std::string NclDocument::getDocumentAlias( NclDocument* document ) {
	std::map<std::string, NclDocument*>::iterator i;

	if (documentBase == NULL) {
		return "";
	}

	for (i != documentAliases->begin(); i != documentAliases->end(); ++i) {
		if (i->second == document) {
			return i->first;
		}
	}

	return "";
}

bptnc::ContextNode* NclDocument::getBody() {
	return body;
}

std::string NclDocument::getDocumentLocation( NclDocument* document ) {
	std::map<std::string, NclDocument*>::iterator i;

	if (documentBase == NULL) {
		return "";
	}

	for (i = documentLocations->begin(); i != documentLocations->end(); ++i) {
		if (i->second == document) {
			return i->first;
		}
	}

	return "";
}

std::vector<NclDocument*>* NclDocument::getDocuments() {
	if (documentBase != NULL) {
		return documentBase;
	} else {
		return NULL;
	}
}

const std::string &NclDocument::getId() {
	return id;
}

bptnc::Node* NclDocument::getNodeLocally( const std::string & nodeId ) {			
	if (body != NULL) {
		if (body->getId() == nodeId) {
			return body;
		} else {
			return body->recursivelyGetNode( nodeId );
		}

	} else {
		return NULL;
	}
}

bptnc::Node* NclDocument::getNode( const std::string & nodeId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	NclDocument* document;
	bptnc::Node* node = NULL;

	index = nodeId.find_first_of( "#" );
	if (index == std::string::npos) {
		suffix=nodeId;
	} else if (index == 0) {
		suffix=nodeId.substr( 1, nodeId.length() - 1 );
	} else {
		prefix = nodeId.substr( 0, index );
		index++;
		suffix = nodeId.substr( index, nodeId.length() - index );
	}

	if (prefix.empty()) {
		node = getNodeLocally( suffix );
	}

	if (documentBase != NULL && node==NULL) {
		if(!prefix.empty() && documentAliases->find( prefix ) != documentAliases->end()) {
			document = (*documentAliases)[prefix];
			node = document->getNode( suffix );
		}
		
		if(node==NULL){
			std::map<std::string, NclDocument*>::iterator it;
			for (it = documentAliases->begin(); it != documentAliases->end(); it++) {
				document = static_cast<NclDocument*>( it->second );
				node = document->getNode( suffix );
				if (node) {
					APRINT_NCL3("got it");
					return node;
				}
			}
		}	
	}
	return node;	
}

bptnl::LayoutRegion* NclDocument::getRegion( const std::string & regionId ) {
	APRINT_NCL3("==> regionId[%s]",regionId.c_str());
	bptnl::LayoutRegion* region=NULL;
	std::map<int, bptnl::RegionBase*>::iterator i;
	std::map<int, bptnl::RegionBase*>* r;
	std::string::size_type index;

	r = getRegionBases();
	i = r->begin();
	while (i != r->end()) {
		region = getRegion( regionId, i->second );
		if (region != NULL) {
			APRINT_NCL3("<== got it");
			return region;
		}
		++i;
	}
	APRINT_NCL3("<== NULL");
	return NULL;
}

bptnl::LayoutRegion* NclDocument::getRegion( const std::string &regionId, bptnl::RegionBase* regionBase ) {
	APRINT_NCL3("==> regionId[%s]",regionId.c_str());
	bptnl::LayoutRegion* region;
	std::vector<NclDocument*>::iterator i;
	std::string::size_type index;
	std::string prefix, suffix;

	if (regionBase != NULL) {
		APRINT_NCL3("---");
		region = regionBase->getRegion( regionId );
		if (region != NULL) {
			APRINT_NCL3("<== regionId[%s]",regionId.c_str());
			return region;
		}
	}

	if (documentBase != NULL) {
		APRINT_NCL3("---");
		for (i = documentBase->begin(); i != documentBase->end(); ++i) {
			APRINT_NCL3("---");
			region = (*i)->getRegion( regionId );
			if (region != NULL) {
				APRINT_NCL3("<== regionId[%s]",regionId.c_str());
				return region;
			}
			else
			{
				index = regionId.find_first_of( "#" );
				if (index != std::string::npos) { 
					prefix = regionId.substr( 0, index );
					index++;
					suffix = regionId.substr( index, regionId.length() - index );   
					APRINT_NCL3("---");
					region = (*i)->getRegion(suffix);
					if (region != NULL) {
						return region;
					}
				}
			}
		}
	}
	APRINT_NCL3("<== NULL regionId[%s]",regionId.c_str());
	return NULL;
}

bptnl::RegionBase* NclDocument::getRegionBase( int devClass ) {
	std::map<int, bptnl::RegionBase*>::iterator i;

	i = regionBases->find( devClass );
	if (i == regionBases->end()) {
		return NULL;
	}

	return i->second;
}

bptnl::RegionBase* NclDocument::getRegionBase( const std::string & regionBaseId ) {
	APRINT_NCL3("<==> regionBase[%s]",regionBaseId.c_str());
	std::map<int, bptnl::RegionBase*>::iterator i;

	i = regionBases->begin();
	while (i != regionBases->end()) {
		if (i->second->getId() == regionBaseId) {
			return i->second;
		}
		++i;
	}

	return NULL;
}

std::map<int, bptnl::RegionBase*>* NclDocument::getRegionBases() {
	APRINT_NCL3("==>");
	std::map<int, bptnl::RegionBase*>* r;
	std::vector<NclDocument*>::iterator i;
	if (documentBase != NULL) {
		for (i = documentBase->begin(); i != documentBase->end(); ++i) {
			r = (*i)->getRegionBases();
			if (r != NULL) {
				std::map<int, bptnl::RegionBase*>::iterator it;
				for (it = r->begin(); it != r->end(); ++it){
					APRINT_NCL3("---");
					addRegionBase(it->second);
				}
			}
		}
	}
	APRINT_NCL3("<== return");
	return regionBases;
}

bptns::Rule* NclDocument::getRule( const std::string & ruleId ) {
	bptns::Rule* rule;
	std::vector<NclDocument*>::iterator i;

	if (ruleBase != NULL) {
		rule = ruleBase->getRule( ruleId );
		if (rule != NULL) {
			return rule;
		}
	}

	if (documentBase != NULL) {
		for (i = documentBase->begin(); i != documentBase->end(); ++i) {
			rule = (*i)->getRule( ruleId );
			if (rule != NULL) {
				return rule;
			}
		}
	}

	return NULL;
}

bptns::RuleBase* NclDocument::getRuleBase() {
	return ruleBase;
}

std::vector<bptns::RuleBase*>* NclDocument::getImportedRuleBases() {
    APRINT_NCL3("==>");
    std::vector<NclDocument*>::iterator i;
    std::vector<bptns::RuleBase*>*ruleBases = new std::vector<bptns::RuleBase*>;
    bptns::RuleBase* rb;
    for (i = documentBase->begin(); i != documentBase->end(); ++i) {
    	APRINT_NCL3("for each document...");
        rb = (*i)->getRuleBase( );
        if (rb != NULL) {
            APRINT_NCL3("...add a base");
            ruleBases->push_back(rb);
        }                                       
    }
    APRINT_NCL3("<==");
    return ruleBases;
}

std::vector<bptnd::DescriptorBase*>* NclDocument::getImportedDescriptorBases() {
    APRINT_NCL3("==>");
    std::vector<NclDocument*>::iterator i;
    std::vector<bptnd::DescriptorBase*>*descriptorBases = new std::vector<bptnd::DescriptorBase*>;
    bptnd::DescriptorBase* db;
    
    if (documentBase != NULL) {
    	for (i = documentBase->begin(); i != documentBase->end(); ++i) {
    		APRINT_NCL3("for each document...");
        	db = (*i)->getDescriptorBase( );
        	if (db != NULL) {
            	APRINT_NCL3("...add a base");
            	descriptorBases->push_back(db);
        	}                                       
    	}
	}
    APRINT_NCL3("<==");
    return descriptorBases;
}

bool NclDocument::removeDocument( NclDocument* document ) {
	std::string alias, location;
	std::vector<NclDocument*>::iterator i;

	if (documentBase == NULL) {
		return false;
	}

	alias = getDocumentAlias( document );
	location = getDocumentLocation( document );
	for (i = documentBase->begin(); i != documentBase->end(); ++i) {
		if (*i == document) {
			documentBase->erase( i );
			documentAliases->erase( alias );
			documentLocations->erase( location );
			return true;
		}
	}
	return false;
}

void NclDocument::setConnectorBase( bptncon::ConnectorBase* connectorBase ) {
	this->connectorBase = connectorBase;
}

void NclDocument::setTransitionBase( bptnt::TransitionBase* transitionBase ) {
	this->transitionBase = transitionBase;
}

void NclDocument::setDescriptorBase( bptnd::DescriptorBase* descriptorBase ) {
	this->descriptorBase = descriptorBase;
}

void NclDocument::setDocumentAlias( NclDocument* document, const std::string & alias ) {
	APRINT_NCL3("==> alias[%s]",alias.c_str());
	std::string oldAlias;

	if (documentBase != NULL) {
		APRINT_NCL3("documentBase != NULL");
		oldAlias = getDocumentAlias( document );
		documentAliases->erase( oldAlias );
		(*documentAliases)[alias] = document;
	}
	APRINT_NCL3("<== alias[%s]",alias.c_str());
}

void NclDocument::setBody( bptnc::ContextNode* node ) {
	body = node;
}

void NclDocument::setDocumentLocation( NclDocument* document, const std::string & location ) {

	std::string oldLocation;

	if (documentBase != NULL) {
		oldLocation = getDocumentLocation( document );
		documentLocations->erase( oldLocation );
		(*documentLocations)[location] = document;
	}
}

void NclDocument::setId( const std::string & id ) {
	this->id = id;
}

void NclDocument::addRegionBase( bptnl::RegionBase* regionBase ) {
  APRINT_NCL3("==> id[%s] deviceClass[%d]",regionBase->getId().c_str(),regionBase->getDeviceClass());
  std::map<int, bptnl::RegionBase*>::iterator i;
  int dClass;

  dClass = regionBase->getDeviceClass();
  i = regionBases->find( dClass );
  if (i != regionBases->end()) {
    APRINT_NCL3("Trying to add the same regionbase. WARN WARN i->[%s]",i->second->getId().c_str());
    LWARN("NclDocument", "Trying to add the same regionBase, deviceClass='%i'", dClass);
    //LGESP absorbing the regino from another reginobase 
    i->second->absorbRegionBase(regionBase);
  } else {
    (*regionBases)[dClass] = regionBase;
  }
  APRINT_NCL3("<== ");
}

void NclDocument::setRuleBase( bptns::RuleBase* ruleBase ) {
	this->ruleBase = ruleBase;
}

void NclDocument::addMetainformation( bptnm::Meta* meta ) {
	if (meta != NULL) {
		metainformationList->push_back( meta );
	}
}

void NclDocument::addMetadata( bptnm::Metadata* metadata ) {
	if (metadata != NULL) {
		metadataList->push_back( metadata );
	}
}

std::vector<bptnm::Meta*>* NclDocument::getMetainformation() {
	return metainformationList;
}

std::vector<bptnm::Metadata*>* NclDocument::getMetadata() {
	return metadataList;
}

void NclDocument::removeRegionBase( const std::string & regionBaseId ) {
	std::map<int, bptnl::RegionBase*>::iterator i;

	i = regionBases->begin();
	while (i != regionBases->end()) {
		if (i->second->getId() == regionBaseId) {
			regionBases->erase( i );
			return;
		}
		++i;
	}
}

void NclDocument::removeMetainformation( bptnm::Meta* meta ) {
	if (meta != NULL) {
		std::vector<bptnm::Meta*>::iterator i;
		i = metainformationList->begin();
		while (i != metainformationList->end()) {
			if (*i == meta) {
				metainformationList->erase( i );
				break;
			}

			if (i != metainformationList->end()) {
				++i;
			}
		}
	}
}

void NclDocument::removeMetadata( bptnm::Metadata* metadata ) {
	if (metadata != NULL) {
		std::vector<bptnm::Metadata*>::iterator i;
		i = metadataList->begin();
		while (i != metadataList->end()) {
			if (*i == metadata) {
				metadataList->erase( i );
				break;
			}

			if (i != metadataList->end()) {
				++i;
			}
		}
	}
}

void NclDocument::removeAllMetainformation() {
	metainformationList->clear();
}

void NclDocument::removeAllMetadata() {
	metadataList->clear();
}
}
}
}
}
