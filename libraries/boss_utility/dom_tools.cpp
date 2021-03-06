/* dom_tools.cpp    $Revision: 1.3 $ $Date: 2006/02/19 20:55:41 $
   Copyright (C) 2000 University of Bonn.
   http://www.ikp.uni-bonn.de/boss

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
   @author Karlheinz Stoeber
   @author Philip Gross
*/

#include "dom_tools.h"
#include <xercesc/util/PlatformUtils.hpp>

static bool doEscapes = false;

ostream & operator << (ostream & target, const XMLCh * x)
{
  //for(unsigned i = 0; i < xercesc::XMLString::stringLen(x); i++) cout << i << "." << (int)x[i] << "." << (char)x[i] << " ";
  //target << "(" <<  (char*)x << ")\t";
  char *p = xercesc::XMLString::transcode(x);
  target << p;
  xercesc::XMLString::release(&p);
  return target;
}

void outputContent(ostream& target, const XMLCh* toWrite)
{
  //char* toWrite = xercesc::XMLString::transcode(toWriteXML);
  const char*    chars  = xercesc::XMLString::transcode(toWrite);
  if (doEscapes == false)
  {
    target << toWrite;
  }
  else
  {
    int            length = strlen(chars);
    
    int index;
    for (index = 0; index < length; index++)
    {
      switch (chars[index])
      {
      case /*chAmpersand*/ 0x26 :
	target << "&amp;";
	break;
	
      case /*chOpenAngle*/ 0x3C :
	target << "&lt;";
	break;
	
      case /*chCloseAngle*/ 0x3E :
	target << "&gt;";
	break;
	
      case /*chDoubleQuote*/ 0x22 :
	target << "&quot;";
	break;
	
      default:
	// If it is none of the special characters, print it as it is
	target << chars[index]; //.substr(index, 1);
	break;
      }
    }
  }
  
  xercesc::XMLString::release((char**)&chars);
  return;
}

ostream & operator << (ostream & target, xercesc::DOMNode * toWrite)
{
  // Retrieve the name and value for convenience
  const XMLCh*   nodeName = toWrite->getNodeName();
  const XMLCh*   nodeValue = toWrite->getNodeValue();

  switch (toWrite->getNodeType())
    {
    case xercesc::DOMNode::TEXT_NODE:
      {
	outputContent(target, nodeValue);
	break;
      }

    case xercesc::DOMNode::PROCESSING_INSTRUCTION_NODE :
      {
	target  << "<?"
		<< nodeName
		<< ' '
		<< nodeValue
		<< "?>";
	break;
      }

    case xercesc::DOMNode::DOCUMENT_NODE :
      {
	// Bug here:  we need to find a way to get the encoding name
	//   for the default code page on the system where the
	//   program is running, and plug that in for the encoding
	//   name.
#if XERCES_VERSION_MAJOR == 3
	target << "<?xml version='1.0' encoding='"<< ((xercesc::DOMDocument*)toWrite)->getInputEncoding() <<"' ?>\n";
#else if XERCES_VERSION_MAJOR == 2
	target << "<?xml version='1.0' encoding='"<< ((xercesc::DOMDocument*)toWrite)->getActualEncoding() <<"' ?>\n";
#endif
	xercesc::DOMNode* child = toWrite->getFirstChild();
	while( child != 0)
	  {
	    target << child << endl;
	    child = child->getNextSibling();
	  }

	break;
      }

    case xercesc::DOMNode::ELEMENT_NODE :
      {
	// Output the element start tag.
	target << '<' << nodeName;

	// Output any attributes on this element
	xercesc::DOMNamedNodeMap* attributes = toWrite->getAttributes();
	int attrCount = attributes->getLength();
	for (int i = 0; i < attrCount; i++)
	  {
	    xercesc::DOMNode*  attribute = attributes->item(i);

	    target  << ' ' << attribute->getNodeName()
		    << " = \"";
	    //  Note that "<" must be escaped in attribute values.
	    outputContent(target, attribute->getNodeValue());
	    target << '"';
	  }

	//
	//  Test for the presence of children, which includes both
	//  text content and nested elements.
	//
	xercesc::DOMNode* child = toWrite->getFirstChild();
	if (child != 0)
	  {
	    // There are children. Close start tag, and output children.
	    target << ">";
	    while( child != 0)
	      {
		target << child;
		child = child->getNextSibling();
	      }

	    // Done with children. Output the end tag.
	    target << "</" << nodeName << ">";
	  }
	else
	  {
	    //
	    //  There were no children. Output the short form close of the
	    //  element start tag, making it an empty-element tag.
	    //
	    target << "/>";
	  }
	break;
      }

    case xercesc::DOMNode::ENTITY_REFERENCE_NODE:
      {
	xercesc::DOMNode* child;
	for (child = toWrite->getFirstChild(); child != 0; child = child->getNextSibling())
	  target << child;
	break;
      }

    case xercesc::DOMNode::CDATA_SECTION_NODE:
      {
	target << "<![CDATA[" << nodeValue << "]]>";
	break;
      }

    case xercesc::DOMNode::COMMENT_NODE:
      {
	target << "<!--" << nodeValue << "-->";
	break;
      }

    default:
      cerr << "Unrecognized node type = "
	   << (long)toWrite->getNodeType() << endl;
    }
  return target;
}

  
bool BOSS::hasAttribute(const xercesc::DOMNode* n, string as) 
{
  return BOSS::hasAttribute((xercesc::DOMElement*) n, as);
}

bool BOSS::hasAttribute(const xercesc::DOMElement * e, string as) {
  XMLCh * ax = xercesc::XMLString::transcode(as.c_str());
  bool a = false;
  if (e->hasAttribute(ax)) a = true;
  xercesc::XMLString::release(&ax);
  return a;
}
  

string BOSS::getAttrByName(const xercesc::DOMNode* n, string as) 
{
  return BOSS::getAttrByName((xercesc::DOMElement*) n, as);
}

string BOSS::getAttrByName(const xercesc::DOMElement * e, string as) {
  XMLCh * ax = xercesc::XMLString::transcode(as.c_str());
  //cout << "ax: " << ax << "\t";
  if (!e->hasAttribute(ax)) {
    xercesc::XMLString::release(&ax);
    return "";
  }
  const XMLCh * vx = e->getAttribute(ax);
  //for(unsigned i = 0; i < xercesc::XMLString::stringLen(vx); i++) cout << sizeof(vx[i]) << "-" << (int)vx[i] << " ";
  //cout << "\tvx: " << vx << "\t";
  char * v = xercesc::XMLString::transcode(vx);
  //for(unsigned i = 0; i < xercesc::XMLString::stringLen(v); i++) cout << i << (char)v[i];
  //cout << "\tv: " << v << "\t";
  string vs = (string)v;
  //cout << "vs: " << vs << endl;
  xercesc::XMLString::release(&ax);
  //XMLString::release(&vx);
  xercesc::XMLString::release(&v);
  return vs;
}

// This function recursively traverses the DOM tree starting at node
// "start_node" and creates a list of element nodes. Each node name must
// satisfy that the node name is equal to "node_name".
void BOSS::createLists(const xercesc::DOMNode * start_node, const char* target_name, BOSS::DOMNodeVector & list)
{
  xercesc::DOMNodeList* nl = start_node->getChildNodes();
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* node = nl->item(i);
    if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      char* node_name = xercesc::XMLString::transcode(node->getNodeName());
      if(strcmp(node_name, target_name) == 0) list.push_back(node);
      xercesc::XMLString::release(&node_name);
      createLists(node, target_name, list);
    }
  }	
}

void BOSS::setAttribute(xercesc::DOMNode* n, const char* name, const char* value)
{
  BOSS::setAttribute((xercesc::DOMElement*) n, name, value);
}

void BOSS::setAttribute(xercesc::DOMElement* e, const char* name, const char* value)
{
  XMLCh* XMLName = xercesc::XMLString::transcode(name);
  XMLCh* XMLValue = xercesc::XMLString::transcode(value);
  e->setAttribute(XMLName, XMLValue);
  xercesc::XMLString::release(&XMLName);
  xercesc::XMLString::release(&XMLValue);
}

xercesc::DOMNodeList* BOSS::getElementsByTagName (const xercesc::DOMElement* e, const char* name)
{
  xercesc::DOMNodeList* list = NULL;
  XMLCh* XMLName = xercesc::XMLString::transcode(name);
  try {
    list = e->getElementsByTagName(XMLName);
  }
  catch (const xercesc::XMLException& toCatch) {
    cerr << "Error " <<  toCatch.getMessage() << "!" << endl;
  }
  xercesc::XMLString::release(&XMLName);
  return list;
}

xercesc::DOMNodeList* BOSS::getElementsByTagName (const xercesc::DOMDocument* doc, const char* name)
{
  xercesc::DOMNodeList* list = NULL;
  XMLCh* XMLName = xercesc::XMLString::transcode(name);
  try {
    list = doc->getElementsByTagName(XMLName);
  }
  catch (const xercesc::XMLException& toCatch) {
    cerr << "Error " <<  toCatch.getMessage() << "!" << endl;
  }
  xercesc::XMLString::release(&XMLName);
  return list;  
  //  return getElementsByTagName(doc->getDocumentElement(), name);
}

xercesc::DOMElement* BOSS::createElement(xercesc::DOMDocument* doc, const char* name)
{
  xercesc::DOMElement* element = NULL;
  XMLCh* XMLName = xercesc::XMLString::transcode(name);
  element = doc->createElement(XMLName);
  xercesc::XMLString::release(&XMLName);
  return element;
}

bool BOSS::compareNodeName(const xercesc::DOMNode* node, const string & name)
{
  char* nodeName = xercesc::XMLString::transcode(node->getNodeName());
  bool result = name.compare(nodeName);
  xercesc::XMLString::release(&nodeName);
  return (result == 0);
}

string BOSS::getNodeName(const xercesc::DOMNode* node)
{
  char * name = xercesc::XMLString::transcode(node->getNodeName());
  string result(name);
  xercesc::XMLString::release(&name);
  return result;
}

string BOSS::getNodeValue(const xercesc::DOMNode* node)
{
  char * name = xercesc::XMLString::transcode(node->getNodeValue());
  string result(name);
  xercesc::XMLString::release(&name);
  return result;
}

void BOSS::initXML()
{
  // Set locale
  cerr << "Local codepage detected as: " << setlocale(LC_ALL,"") << endl;
  setlocale(LC_NUMERIC,"C"); 
  
  // Initialize the XML system. Code is taken from the original distribution
  cerr << "Initializing the XML system" << endl;
  try {
    cerr << "Initializing the XML platform ... ";
    xercesc::XMLPlatformUtils::Initialize();
    cerr << "OK" << endl;
  }
  catch(const xercesc::XMLException& toCatch) {
    cerr << "Error during Xerces-c Initialization.\n" << "  Exception message:" << toCatch.getMessage() << endl;
    exit(EXIT_FAILURE);
  }
  //BOSS::initTranscoder();
}


xercesc::XMLTranscoder* BOSS::initTranscoder(string name)
{
  xercesc::XMLTransService::Codes failReason;
  xercesc::XMLTranscoder* result = xercesc::XMLPlatformUtils::fgTransService->makeNewTranscoderFor(name.c_str(), failReason, 16*1024);
  if (failReason)
  {
    cerr << "Failed to initialise " << name << " transcoder: " << failReason << endl;
    exit(EXIT_FAILURE);
  }
  return result;
}
  

xercesc::DOMDocument* BOSS::parseXMLFile(const char* xml_file)
{
  xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
  //parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    // optional.
  //parser->setDoNamespaces(true);    // optional
  //parser->setDoSchema(true);    // optional
  //parser->useScanner(xercesc::XMLUni::fgDGXMLScanner);
  //cout << "scanner DG" << endl;
  XMLCh* xml = xercesc::XMLString::transcode(xml_file);

  try {
    parser->parse(xml);
  }
  catch (const xercesc::XMLException& toCatch) {
    char* message = xercesc::XMLString::transcode(toCatch.getMessage());
    cout << "Exception message is: \n"
	 << message << "\n";
    xercesc::XMLString::release(&message);
    exit(EXIT_FAILURE);
  }
  catch (const xercesc::DOMException& toCatch) {
    char* message = xercesc::XMLString::transcode(toCatch.msg);
    cout << "Exception message is: \n"
	 << message << "\n";
    xercesc::XMLString::release(&message);
    exit(EXIT_FAILURE);
  }
  catch (const xercesc::SAXException& toCatch) {
    char* message = xercesc::XMLString::transcode(toCatch.getMessage());
    cout << "Exception message is: \n"
	 << message << "\n";
    xercesc::XMLString::release(&message);
    exit(EXIT_FAILURE);
  }

  xercesc::XMLString::release(&xml);
  return parser->getDocument();
}

xercesc::DOMDocument* BOSS::parseXMLFromMemory(const string input)
{
	  xercesc::DOMDocument *doc = NULL;

#if XERCES_VERSION_MAJOR == 3
  //Create Parser

  xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
  const xercesc::InputSource* memoryInput = (xercesc::InputSource*) new MMFInputSource(input.c_str());

  try {
	  parser->parse(*memoryInput);
	  doc = parser->getDocument();
  }
  catch (const xercesc::XMLException& toCatch) {
    cerr << "Error trying to parse: " <<  input << "!" << endl;
    exit(EXIT_FAILURE);
  }

#else if XERCES_VERSION_MAJOR == 2
  //Create Parser
   static const XMLCh impltype[] = { xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull };
   xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(impltype);
   xercesc::DOMBuilder* parser=(static_cast<xercesc::DOMImplementationLS*>(impl))->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS,0);
   xercesc::DOMInputSource* memoryInput = (xercesc::DOMInputSource*) new  MMFInputSource(input.c_str());
   try {
     doc = parser->parse(*memoryInput);
   }
   catch (const xercesc::XMLException& toCatch) {
     cerr << "Error trying to parse: " <<  input << "!" << endl;
     exit(EXIT_FAILURE);
   }
#endif

  return doc;
}

void* BOSS::getUserData(const xercesc::DOMNode* node)
{
  XMLCh* key = xercesc::XMLString::transcode("UserData");
  void* data = node->getUserData(key);
  xercesc::XMLString::release(&key);
  return data;
}

void BOSS::setUserData(const xercesc::DOMNode* node, void* data)
{
  XMLCh* key = xercesc::XMLString::transcode("UserData");
  ((xercesc::DOMNode*)node)->setUserData(key, data, NULL);
  xercesc::XMLString::release(&key);
}

/*
BOSSString & BOSSString::operator = (const XMLCh* source)
{
  int eaten = 0;
  UTF8Transcoder->transcodeTo(source, xercesc::XMLString::stringLen(source), data, 100, eaten, xercesc::XMLTranscoder::UnRep_Throw);
  return data;
}
*/
