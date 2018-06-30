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

#include "../../include/privatebase/PrivateBaseManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

PrivateBaseManager::PrivateBaseManager() {
	this->privateBases = new map<string, PrivateBaseContext*>;
}

PrivateBaseManager::~PrivateBaseManager() {
	map<string, PrivateBaseContext*>::iterator i;

	if (privateBases != NULL) {
		i = privateBases->begin();
		while (i != privateBases->end()) {
			delete i->second;
			++i;
		}
		delete privateBases;
		privateBases = NULL;
	}
}

void PrivateBaseManager::release() {
}

void PrivateBaseManager::createPrivateBase( string id ) {
	PrivateBaseContext* privateBaseContext;

	if (privateBases->count( id ) == 0) {
		privateBaseContext = new PrivateBaseContext();
		privateBaseContext->createPrivateBase( id );
		(*privateBases)[id] = privateBaseContext;
	}
}

NclDocument* PrivateBaseManager::addDocument( string id, string location ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addDocument( location );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::embedDocument( string id, string docId, string nodeId, string location ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->embedDocument( docId, nodeId, location );
	}

	return NULL;
}

void* PrivateBaseManager::addVisibleDocument( string id, string location ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addVisibleDocument( location );
	}

	return NULL;
}

string PrivateBaseManager::getDocumentLocation( string id, string docId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getDocumentLocation( docId );
	}

	return "";
}

string PrivateBaseManager::getEmbeddedDocumentLocation( string id, string parentDocId, string nodeId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getEmbeddedDocumentLocation( parentDocId, nodeId );
	}

	return "";
}

NclDocument* PrivateBaseManager::getDocument( string id, string docId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getDocument( docId );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::getEmbeddedDocument( string id, string parentDocId, string nodeId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getEmbeddedDocument( parentDocId, nodeId );
	}

	return NULL;
}

vector<NclDocument*>* PrivateBaseManager::getDocuments( string id ) {
	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getDocuments();
	}

	return NULL;
}

NclDocument* PrivateBaseManager::removeDocument( string id, string docId ) {
	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeDocument( docId );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::removeEmbeddedDocument( string baseId, string parentDocId, string nodeId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( baseId );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeEmbeddedDocument( parentDocId, nodeId );
	}

	return NULL;
}

void PrivateBaseManager::clear( string id ) {
	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		privateBaseContext->clear();
	}
}

LayoutRegion* PrivateBaseManager::addRegion( string id, string documentId, string regionBaseId, string regionId, string xmlRegion ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addRegion( documentId, regionBaseId, regionId, xmlRegion );
	}

	return NULL;
}

LayoutRegion* PrivateBaseManager::removeRegion( string id, string documentId, string regionBaseId, string regionId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeRegion( documentId, regionBaseId, regionId );
	}

	return NULL;
}

RegionBase* PrivateBaseManager::addRegionBase( string id, string documentId, string xmlRegionBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addRegionBase( documentId, xmlRegionBase );
	}

	return NULL;
}

RegionBase* PrivateBaseManager::removeRegionBase( string id, string documentId, string regionBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeRegionBase( documentId, regionBaseId );
	}

	return NULL;
}

Rule* PrivateBaseManager::addRule( string id, string documentId, string xmlRule ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addRule( documentId, xmlRule );
	}

	return NULL;
}

Rule* PrivateBaseManager::removeRule( string id, string documentId, string ruleId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeRule( documentId, ruleId );
	}

	return NULL;
}

RuleBase* PrivateBaseManager::addRuleBase( string id, string documentId, string xmlRuleBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addRuleBase( documentId, xmlRuleBase );
	}

	return NULL;
}

RuleBase* PrivateBaseManager::removeRuleBase( string id, string documentId, string ruleBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeRuleBase( documentId, ruleBaseId );
	}

	return NULL;
}

Transition* PrivateBaseManager::addTransition( string id, string documentId, string xmlTransition ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addTransition( documentId, xmlTransition );
	}

	return NULL;
}

Transition* PrivateBaseManager::removeTransition( string id, string documentId, string transitionId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeTransition( documentId, transitionId );
	}

	return NULL;
}

TransitionBase* PrivateBaseManager::addTransitionBase( string id, string documentId, string xmlTransitionBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addTransitionBase( documentId, xmlTransitionBase );
	}

	return NULL;
}

TransitionBase* PrivateBaseManager::removeTransitionBase( string id, string documentId, string transitionBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeTransitionBase( documentId, transitionBaseId );
	}

	return NULL;
}

Connector* PrivateBaseManager::addConnector( string id, string documentId, string xmlConnector ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addConnector( documentId, xmlConnector );
	}

	return NULL;
}

Connector* PrivateBaseManager::removeConnector( string id, string documentId, string connectorId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeConnector( documentId, connectorId );
	}

	return NULL;
}

ConnectorBase* PrivateBaseManager::addConnectorBase( string id, string documentId, string xmlConnectorBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addConnectorBase( documentId, xmlConnectorBase );
	}

	return NULL;
}

ConnectorBase* PrivateBaseManager::removeConnectorBase( string id, string documentId, string connectorBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeConnectorBase( documentId, connectorBaseId );
	}

	return NULL;
}

GenericDescriptor* PrivateBaseManager::addDescriptor( string id, string documentId, string xmlDescriptor ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addDescriptor( documentId, xmlDescriptor );
	}

	return NULL;
}

GenericDescriptor* PrivateBaseManager::removeDescriptor( string id, string documentId, string descriptorId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeDescriptor( documentId, descriptorId );
	}

	return NULL;
}

DescriptorBase* PrivateBaseManager::addDescriptorBase( string id, string documentId, string xmlDescriptorBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addDescriptorBase( documentId, xmlDescriptorBase );
	}

	return NULL;
}

DescriptorBase* PrivateBaseManager::removeDescriptorBase( string id, string documentId, string descriptorBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeDescriptorBase( documentId, descriptorBaseId );
	}

	return NULL;
}

Base* PrivateBaseManager::addImportBase( string id, string documentId, string docBaseId, string xmlImportBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addImportBase( documentId, docBaseId, xmlImportBase );
	}

	return NULL;
}

Base* PrivateBaseManager::removeImportBase( string id, string documentId, string docBaseId, string documentURI ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeImportBase( documentId, docBaseId, documentURI );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::addImportedDocumentBase( string id, string documentId, string xmlImportedDocumentBase ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addImportedDocumentBase( documentId, xmlImportedDocumentBase );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::removeImportedDocumentBase( string id, string documentId, string importedDocumentBaseId ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeImportedDocumentBase( documentId, importedDocumentBaseId );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::addImportNCL( string id, string documentId, string xmlImportNCL ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addImportNCL( documentId, xmlImportNCL );
	}

	return NULL;
}

NclDocument* PrivateBaseManager::removeImportNCL( string id, string documentId, string documentURI ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->removeImportNCL( documentId, documentURI );
	}

	return NULL;
}

Node* PrivateBaseManager::addNode( string id, string documentId, string compositeId, string xmlNode ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addNode( documentId, compositeId, xmlNode );
	}

	return NULL;
}

InterfacePoint* PrivateBaseManager::addInterface( string id, string documentId, string nodeId, string xmlInterface ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addInterface( documentId, nodeId, xmlInterface );
	}

	return NULL;
}

Link* PrivateBaseManager::addLink( string id, string documentId, string compositeId, string xmlLink ) {

	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->addLink( documentId, compositeId, xmlLink );
	}

	return NULL;
}

PrivateBaseContext* PrivateBaseManager::getPrivateBaseContext( string id ) {
	map<string, PrivateBaseContext*>::iterator i;
	PrivateBaseContext* privateBaseContext = NULL;

	i = privateBases->find( id );
	if (i != privateBases->end()) {
		privateBaseContext = i->second;
	}

	return privateBaseContext;
}

PrivateBase* PrivateBaseManager::getPrivateBase( string id ) {
	PrivateBaseContext* privateBaseContext;

	privateBaseContext = getPrivateBaseContext( id );
	if (privateBaseContext != NULL) {
		return privateBaseContext->getPrivateBase();
	}

	return NULL;
}
}
}
}
}
}

