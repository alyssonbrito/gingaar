#include <string>
#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "../../../src/cfg/xmlparser.h"
#include "../../../src/cfg/propertynode.h"

namespace fs = boost::filesystem;

class XmlParserTest : public testing::Test {
public:
	XmlParserTest() {
	}
	virtual ~XmlParserTest() {
	}

	virtual void SetUp() {
		path = getDepot();
		addPath(path);

		//	tree configuration
		tree.setName("config");
		tree.addNode("tool")
			.addNode("ginga")
				.addValue("version","Description",0)
				.addValue("enabled","Description",true)
				.addValue("information","Description",std::string("nothing"));
		tree.addNode("provider")
			.addNode("dvb")
				.addValue("version","Description",0)
				.addValue("information","Description",std::string("nothing"))
				.addValue("enabled","Description",false);

		tree("provider.dvb").addNode("network")
			.addValue("ip","Description",std::string("0.0.0.0"));

		//	tree 2 configuration
		tree2.addNode("gui")
			.addValue("use","desc",std::string("dummy"))
			.addNode("canvas")
				.addValue("use","desc",std::string("dummy"));
		tree2("gui").addNode("window")
			.addValue("use","desc",std::string("dummy"));

		//	tree 3 configuration
		tree3.addNode("gui")
			.addValue("use","desc",std::string("dummy"))
			.addNode("canvas")
				.addValue("use","desc",std::string("dummy"));
		tree3("gui").addNode("window")
			.addValue("use","desc",std::string("dummy"));
		tree3("gui").addNode("services")
			.addValue("use","desc",std::string("dummy"));

		//	tree 4 configuration
		tree4.addNode("gui")
			.addValue("use","desc",std::string("dummy"))
			.addNode("canvas")
				.addValue("use","desc",std::string("dummy"));
		tree4("gui").addNode("services")
			.addValue("use","desc",std::string("dummy"))
			.addNode("serviceOne")
				.addValue("use","desc",std::string("dummy"));
		tree4("gui.services").addNode("serviceTwo")
				.addValue("use","desc",std::string("dummy"));
		tree4("gui").addNode("window")
			.addValue("use","desc",std::string("dummy"));

		//	tree 5 configuration
		tree5.addNode("gui")
			.addValue("use","desc",std::string("dummy"))
			.addNode("canvas")
				.addValue("use","desc",std::string("dummy"));
		tree5("gui").addNode("services")
			.addValue("use","desc",std::string("dummy"))
			.addNode("serviceOne")
				.addValue("use","desc",std::string("dummy"))
				.addNode("service-part")
					.addValue("use","desc",std::string("dummy"));
		tree5("gui.services").addNode("serviceTwo")
				.addValue("use","desc",std::string("dummy"));
		tree5("gui").addNode("window")
			.addValue("use","desc",std::string("dummy"));
	}

	void addPath(fs::path &aPath) {
		aPath /= "lib";
		aPath /= "dtv-util";
		aPath /= "test";
		aPath /= "src";
		aPath /= "cfg";
		aPath /= "resources";
	}
	std::string getDepot() {
		const char *root = getenv( "DEPOT" );
		return root ? root : "";
	}

	void setValues(util::cfg::PropertyNode *tree) {
		(*tree)("provider.dvb.network").setStr("ip","192.168.1.1");
		(*tree)("tool.ginga").setStr("version", "50");
		(*tree)("tool.ginga").setStr("enabled", "false");
	}

	void setValuesForTree2(util::cfg::PropertyNode *tree) {
		(*tree)("gui").setStr("use",std::string("dummyPretend"));
		(*tree)("gui.canvas").setStr("use",std::string("dummyPretend"));
		(*tree)("gui.window").setStr("use",std::string("dummyPretend"));
	}

	void setValuesForTree3(util::cfg::PropertyNode *tree){
		setValuesForTree2(tree);
		(*tree)("gui.services").setStr("use",std::string("dummyPretend"));
	}
	void setValuesForTree4(util::cfg::PropertyNode *tree){
		setValuesForTree3(tree);
		(*tree)("gui.services.serviceOne").setStr("use",std::string("dummyPretend"));
		(*tree)("gui.services.serviceTwo").setStr("use",std::string("dummyPretend"));


	}
	void setValuesForTree5(util::cfg::PropertyNode *tree){
		setValuesForTree4(tree);
		(*tree)("gui.services.serviceOne.service-part").setStr("use",std::string("dummyPretend"));
	}

	void addValuesForTree5() {
		tree5("gui").addValue("other","Description",std::string("dummy"));
		tree5("gui.canvas").addValue("other","Description",std::string("dummy"));
		tree5("gui.services").addValue("other","Description",std::string("dummy"));
		tree5("gui.services.serviceOne").addValue("other","Description",std::string("dummy"));
		tree5("gui.services.serviceOne.service-part").addValue("other","Description",std::string("dummy"));
		tree5("gui.window").addValue("other","Description",std::string("dummy"));
	}

	virtual void TearDown() {

	}
	util::cfg::PropertyNode tree;
	util::cfg::PropertyNode tree2;
	util::cfg::PropertyNode tree3;
	util::cfg::PropertyNode tree4;
	util::cfg::PropertyNode tree5;
	fs::path path;
};

static inline bool comparate(const std::string expected,std::stringbuf *buffer) {
	return buffer->str().compare(expected) == 0;
}

TEST_F (XmlParserTest, default_tree) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<config>\n\
  <tool>\n\
    <ginga>\n\
      <version>0</version>\n\
      <enabled>true</enabled>\n\
      <information>nothing</information>\n\
    </ginga>\n\
  </tool>\n\
  <provider>\n\
    <dvb>\n\
      <version>0</version>\n\
      <information>nothing</information>\n\
      <enabled>false</enabled>\n\
      <network>\n\
        <ip>0.0.0.0</ip>\n\
      </network>\n\
    </dvb>\n\
  </provider>\n\
</config>\n");

	std::ostream aStream(buffer);

	ASSERT_TRUE(parser.defaults(aStream,&tree));
	ASSERT_TRUE(comparate(expected,buffer));
	delete buffer;
}

TEST_F (XmlParserTest, default_tree2) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummy</use>\n\
    <canvas>\n\
      <use>dummy</use>\n\
    </canvas>\n\
    <window>\n\
      <use>dummy</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);

	ASSERT_TRUE( parser.defaults(aStream,&tree2) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F (XmlParserTest, default_tree3) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummy</use>\n\
    <canvas>\n\
      <use>dummy</use>\n\
    </canvas>\n\
    <window>\n\
      <use>dummy</use>\n\
    </window>\n\
    <services>\n\
      <use>dummy</use>\n\
    </services>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);

	ASSERT_TRUE( parser.defaults(aStream,&tree3) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F ( XmlParserTest, default_tree4 ) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummy</use>\n\
    <canvas>\n\
      <use>dummy</use>\n\
    </canvas>\n\
    <services>\n\
      <use>dummy</use>\n\
      <serviceOne>\n\
        <use>dummy</use>\n\
      </serviceOne>\n\
      <serviceTwo>\n\
        <use>dummy</use>\n\
      </serviceTwo>\n\
    </services>\n\
    <window>\n\
      <use>dummy</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);

	ASSERT_TRUE( parser.defaults(aStream,&tree4) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F( XmlParserTest, default_tree5 ) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummy</use>\n\
    <canvas>\n\
      <use>dummy</use>\n\
    </canvas>\n\
    <services>\n\
      <use>dummy</use>\n\
      <serviceOne>\n\
        <use>dummy</use>\n\
        <service-part>\n\
          <use>dummy</use>\n\
        </service-part>\n\
      </serviceOne>\n\
      <serviceTwo>\n\
        <use>dummy</use>\n\
      </serviceTwo>\n\
    </services>\n\
    <window>\n\
      <use>dummy</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);

	ASSERT_TRUE( parser.defaults(aStream,&tree5) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F( XmlParserTest, pretend_tree ) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<config>\n\
  <tool>\n\
    <ginga>\n\
      <version>50</version>\n\
      <enabled>false</enabled>\n\
    </ginga>\n\
  </tool>\n\
  <provider>\n\
    <dvb>\n\
      <network>\n\
        <ip>192.168.1.1</ip>\n\
      </network>\n\
    </dvb>\n\
  </provider>\n\
</config>\n");

	std::ostream aStream(buffer);
	setValues(&tree);
	ASSERT_TRUE( parser.pretend(aStream,&tree) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F( XmlParserTest, pretend_tree2 ) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummyPretend</use>\n\
    <canvas>\n\
      <use>dummyPretend</use>\n\
    </canvas>\n\
    <window>\n\
      <use>dummyPretend</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);
	setValuesForTree2(&tree2);
	ASSERT_TRUE( parser.pretend(aStream,&tree2) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F (XmlParserTest, pretend_tree3) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummyPretend</use>\n\
    <canvas>\n\
      <use>dummyPretend</use>\n\
    </canvas>\n\
    <window>\n\
      <use>dummyPretend</use>\n\
    </window>\n\
    <services>\n\
      <use>dummyPretend</use>\n\
    </services>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);
	setValuesForTree3(&tree3);
	ASSERT_TRUE( parser.pretend(aStream,&tree3) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F (XmlParserTest, pretend_tree4) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummyPretend</use>\n\
    <canvas>\n\
      <use>dummyPretend</use>\n\
    </canvas>\n\
    <services>\n\
      <use>dummyPretend</use>\n\
      <serviceOne>\n\
        <use>dummyPretend</use>\n\
      </serviceOne>\n\
      <serviceTwo>\n\
        <use>dummyPretend</use>\n\
      </serviceTwo>\n\
    </services>\n\
    <window>\n\
      <use>dummyPretend</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);
	setValuesForTree4(&tree4);
	ASSERT_TRUE( parser.pretend(aStream,&tree4) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F (XmlParserTest, pretend_tree5){
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummyPretend</use>\n\
    <canvas>\n\
      <use>dummyPretend</use>\n\
    </canvas>\n\
    <services>\n\
      <use>dummyPretend</use>\n\
      <serviceOne>\n\
        <use>dummyPretend</use>\n\
        <service-part>\n\
          <use>dummyPretend</use>\n\
        </service-part>\n\
      </serviceOne>\n\
      <serviceTwo>\n\
        <use>dummyPretend</use>\n\
      </serviceTwo>\n\
    </services>\n\
    <window>\n\
      <use>dummyPretend</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);
	setValuesForTree5(&tree5);
	ASSERT_TRUE( parser.pretend(aStream,&tree5) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F (XmlParserTest, pretend_tree5_withDefaults) {
	util::cfg::XmlParser parser;
	std::stringbuf *buffer = new std::stringbuf;
	std::string expected(
		"<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>\n\
<root>\n\
  <gui>\n\
    <use>dummyPretend</use>\n\
    <canvas>\n\
      <use>dummyPretend</use>\n\
    </canvas>\n\
    <services>\n\
      <use>dummyPretend</use>\n\
      <serviceOne>\n\
        <use>dummyPretend</use>\n\
        <service-part>\n\
          <use>dummyPretend</use>\n\
        </service-part>\n\
      </serviceOne>\n\
      <serviceTwo>\n\
        <use>dummyPretend</use>\n\
      </serviceTwo>\n\
    </services>\n\
    <window>\n\
      <use>dummyPretend</use>\n\
    </window>\n\
  </gui>\n\
</root>\n");

	std::ostream aStream(buffer);
	setValuesForTree5(&tree5);
	addValuesForTree5();
	ASSERT_TRUE( parser.pretend(aStream,&tree5) );
	ASSERT_TRUE( comparate(expected,buffer) );
	delete buffer;
}

TEST_F(XmlParserTest, parserIn_badPath ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "Error.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree), std::runtime_error );
}

TEST_F( XmlParserTest, parserIn_bad_node ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "data.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error );
}

TEST_F( XmlParserTest, parserIn_bad_tree_name ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "nameTree.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error);
}

TEST_F( XmlParserTest, parserIn_value_missing ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "notValue.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error );
}

TEST_F( XmlParserTest, parserIn_value_missing_two ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "notValue2.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error );
}

TEST_F( XmlParserTest, parserIn_node_missing ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "notNode.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error );
}

TEST_F(XmlParserTest, parserIn_node_missing_two ) {
	util::cfg::XmlParser parser;
	util::cfg::PropertyNode tree("config");

	path /= "notNode2.xml";
	ASSERT_THROW( parser.parserIn(path.string(),&tree),  std::runtime_error );
}

TEST_F( XmlParserTest, parserIn ) {
	fs::path root2(getDepot());
	util::cfg::XmlParser parser;

	addPath(root2);
	path /= "data.xml";
	root2 /= "data2.xml";

	parser.parserIn(path.string(),&tree);

	ASSERT_TRUE( tree.get<std::string> ("tool.ginga.information") == "bla" );
	ASSERT_TRUE( tree.get<bool> ("tool.ginga.enabled") );
	ASSERT_TRUE( tree.get<int> ("tool.ginga.version") == 1);
	ASSERT_TRUE( tree.get<std::string> ("provider.dvb.information") == "blalala" );
	ASSERT_TRUE( tree.get<int> ("provider.dvb.version") == 2 );
	ASSERT_TRUE( !tree.get<bool> ("provider.dvb.enabled") );

	parser.parserIn(root2.string(),&tree("tool"));
	ASSERT_TRUE( tree.get<std::string> ("tool.ginga.information") == "fine" );

	ASSERT_TRUE( tree.get<int> ("tool.ginga.version") == 3 );
}
