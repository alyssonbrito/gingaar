/*
 * Copyright 1999-2002,2004 The Apache Software Foundation.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: DOMTreeErrorReporter.cpp,v 1.2 2008/10/01 20:54:36 marcio Exp $
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/SAXParseException.hpp>
#include "../../../include/framework/DOMTreeErrorReporter.h"
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

void DOMTreeErrorReporter::warning( const SAXParseException& ) {
	//
	// Ignore all warnings.
	//
}

void DOMTreeErrorReporter::error( const SAXParseException& toCatch ) {
	fSawErrors = true;
	XERCES_STD_QUALIFIER cerr << "Error at file \"" << StrX( toCatch.getSystemId() )
			<< "\", line " << toCatch.getLineNumber()
			<< ", column " << toCatch.getColumnNumber()
			<< "\n   Message: " << StrX( toCatch.getMessage() ) << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::fatalError( const SAXParseException& toCatch ) {
	fSawErrors = true;
	XERCES_STD_QUALIFIER cerr << "Fatal Error at file \"" << StrX( toCatch.getSystemId() )
			<< "\", line " << toCatch.getLineNumber() << ", column " << toCatch.getColumnNumber()
			<< "\n   Message: " << StrX( toCatch.getMessage() ) << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::resetErrors() {
	fSawErrors = false;
}

