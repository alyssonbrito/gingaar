#include "BrowserPresentationEngine.h"

#define DEFAULT_SCREEN_WIDTH	1280
#define DEFAULT_SCREEN_HEIGHT	720

/**
 *
 *
 *
 */
BrowserPresentationEngine::BrowserPresentationEngine() {
	printf("[BrowserPresentationEngine::%s::%d] BEGIN\n", __FUNCTION__, __LINE__);
	windowWidth       = DEFAULT_SCREEN_WIDTH;
	windowHeight      = DEFAULT_SCREEN_HEIGHT;
	canvasWidth       = DEFAULT_SCREEN_WIDTH;
	canvasHeight      = DEFAULT_SCREEN_HEIGHT;
	recommendedWidth  = DEFAULT_SCREEN_WIDTH;
	recommendedHeight = DEFAULT_SCREEN_HEIGHT;
	_isInitialized    = false;
	_isRunning        = false;
	_isWorking        = false;
	_name             = "ginga_ncl";
	_desc             = "DESC";
	_version          = BUILD_TAG_REVISION;
	_build            = "1";
	_home_directory   = boost::filesystem::initial_path();
	printf("[BrowserPresentationEngine::%s::%d] END\n", __FUNCTION__, __LINE__);
}


/**
 *
 *
 *
 */
BrowserPresentationEngine::~BrowserPresentationEngine() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::printArguments(int argc, char *argv[]) {
	LTRACE("Port", "---[ Arguments from Command Line ]---");
	for (int i = 0; i < argc; i++) {
		LTRACE("Port", "# [% 2d/% 2d]: [%s]", i, argc, argv[i]);
	}
}



/**
 *
 *
 *
 */
void BrowserPresentationEngine::printConfiguration() {
	LTRACE("Port", "---[ Launcher Info ]---");
	LTRACE("Port", "Launcher Name          : [%s]", _name.c_str());
	LTRACE("Port", "Launcher Description   : [%s]", _desc.c_str());
	LTRACE("Port", "Launcher Version       : [%s]", _version.c_str());
	LTRACE("Port", "Launcher Build         : [%s]", _build.c_str());

	LTRACE("Port", "Home Directory set     : [%s]", _home_directory.string().c_str());

	LTRACE("Port", "---[ Window ]---");
	LTRACE("Port", "gui.window.title       : [%s]", util::cfg::getValue<std::string>("gui.window.title").c_str());
	LTRACE("Port", "gui.window.size.width  : [%d]", util::cfg::getValue<int>("gui.window.size.width"));
	LTRACE("Port", "gui.window.size.height : [%d]", util::cfg::getValue<int>("gui.window.size.height"));

	LTRACE("Port", "---[ Canvas ]---");
	LTRACE("Port", "gui.canvas.size.width  : [%d]", util::cfg::getValue<int>("gui.canvas.size.width"));
	LTRACE("Port", "gui.canvas.size.height : [%d]", util::cfg::getValue<int>("gui.canvas.size.height"));

#if HTV_NC_VER >= 5 /* webOS and higher version */
	LTRACE("Port", "---[ WebOS Environment ]---");
	LTRACE("Port", "tool.windid            = %s", getCfg<std::string>("winid").c_str());
#endif /* HTV_NC_VER >= 5 */

}



/**
 *
 *
 *
 */
int BrowserPresentationEngine::initialize(int argc, char *argv[]) {
#ifndef NDEBUG
	try {
#endif

#ifdef PR_SET_NAME
		prctl(PR_SET_NAME,"tNCLworker",0,0,0);
#endif

		cmd = new util::cfg::cmd::CommandLine(argc, argv);
		registerProperties(*cmd);				// Create tree config
		util::reg::init();						// Calls the callbacks that initializes the tool's libraries
		registerConfigs();						// Register the configuration files that will be loaded
		loadConfig();							// Load configuration from files
		cmd->parse();

		if (cmd->isSet("debug")) {
			debug();
			util::log::init();					// Initialize log system
		}
		/*else {
			util::cfg::get("tool").addValue("log", "Logging is enabled/disabled", false);
		}*/

		if(getCfg<std::string> ("size") == "720x576") {
			windowWidth  = 720;
			windowHeight = 576;
			canvasWidth  = 720;
			canvasHeight = 576;			
		}	

		setWindowTitle("Ginga-NCL");

		// Initialize Screen Size with default values
		//notifyRecommendedResolution(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
		//applyRecommendedResolution();

		util::cfg::setValue("gui.canvas.size.width",  DEFAULT_SCREEN_WIDTH);
		util::cfg::setValue("gui.canvas.size.height", DEFAULT_SCREEN_HEIGHT);
		util::cfg::setValue("gui.window.size.width",  DEFAULT_SCREEN_WIDTH);
		util::cfg::setValue("gui.window.size.height", DEFAULT_SCREEN_HEIGHT);


		printArguments(argc, argv);
		printConfiguration();

		finalizeGraphic();
		initializeGraphic(windowWidth, windowHeight);

		initializeDevice();

		br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::onKeyPress( boost::bind( &BrowserPresentationEngine::onKeyPressed, this, _1 ) );

		pem = new br::pucrio::telemidia::ginga::ncl::PresentationEngineManager(sys);
		pem->setBaseId(getCfg<std::string>("baseId"));

		//calbacks
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Registering Xlet Player", __FUNCTION__, __LINE__);
		pem->RegisterNCLPlayerEvent(_onNCLPlayerEventCallback);
		pem->registerNCLStartXlet(_onNCLStartXletCallback);
		pem->registerNCLControlXlet(_onNCLControlXletCallback);
		pem->registerXletAppStateCallback(_onXletAppStateCallback);
		pem->registerNCLPlayerSetVisibility(_onNCLSetVisibilityCallback);

		pem->setOnStopDocument(boost::bind(&BrowserPresentationEngine::notifyStopped, this));

		_isInitialized = true;

		if (_beforeInitCallback != NULL) {
				(*_beforeInitCallback)();
			}

		pem->run();

#ifndef NDEBUG
	} catch (const std::exception &e) {
		LERROR("Port", "[BrowserPresentationEngine::%s::%d][Tool] Catch exception: %s", __FUNCTION__, __LINE__, e.what());
	} catch (...) {
		LERROR("Port", "[BrowserPresentationEngine::%s::%d][Tool] Catch unknown exception", __FUNCTION__, __LINE__);
	}
#endif

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return SUCCESS;
}

/**
 *
 *
 *
 */
int BrowserPresentationEngine::finalize(void) {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);

#ifndef NDEBUG
	try {
#endif

		util::log::fin();			//	Finalize log system
		util::reg::fin();			//	Calls the callbacks that finalizes the tool's libraries

		if (cmd != NULL) {
			delete cmd;
			cmd = NULL;
		}

		finalizeGraphic();

#ifndef NDEBUG
	} catch (const std::exception &e) {
		LERROR("Port", "[BrowserPresentationEngine::%s::%d][Tool] Catch exception: %s", __FUNCTION__, __LINE__, e.what());
	} catch (...) {
		LERROR("Port", "[BrowserPresentationEngine::%s::%d][Tool] Catch unknown exception", __FUNCTION__, __LINE__);
	}
#endif

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return SUCCESS;
}

/**
 * Prepare System and Device, and initialize them. It should be done once.
 * For Low Models, if you try to reinitialize device, the process crashes. So,
 * initialize once and keep its references. Do not destroy it!
 */
void BrowserPresentationEngine::initializeDevice() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);

	if (NULL == sys) {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Initalizing System", __FUNCTION__, __LINE__);
		sys = new player::System();
		if (NULL == dev) {
			LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Initializing Player", __FUNCTION__, __LINE__);
			dev = new player::Device(sys);
			assignCallbacksToDevice();
		} else {
			LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Device is already init", __FUNCTION__, __LINE__);
		}
	} else {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] System is already init", __FUNCTION__, __LINE__);
	}

	if (dev->initialize()) {
		sys->addDevice(dev);
	} else {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] can't init device", __FUNCTION__, __LINE__);
	}

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
}


/**
 *
 *
 */
void BrowserPresentationEngine::assignCallbacksToDevice() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);

	assert(dev);

	if (_fetchFilesCallback   != NULL)	{	dev->setOnFetchFile(_fetchFilesCallback);		}
	if (_beforeInitCallback   != NULL)	{	dev->setOnKeyBeforeInit(_beforeInitCallback);		}
	if (_reserveKeysCallback != NULL)	{	dev->setOnReserveKeys(_reserveKeysCallback);		}
	if (_loadMediaCallback    != NULL)	{	dev->setOnLoadMedia(_loadMediaCallback);		}
	if (_playMediaCallback    != NULL)	{	dev->setOnPlayMedia(_playMediaCallback);		}
	if (_setMediaTimeCallback    != NULL)	{	dev->setOnMediaTime(_setMediaTimeCallback);		}
	if (_stopMediaCallback    != NULL)	{	dev->setOnStopMedia(_stopMediaCallback);		}
	if (_closeMediaCallback   != NULL)	{	dev->setOnCloseMedia(_closeMediaCallback);		}
	if (_freezeMediaCallback  != NULL)	{	dev->setOnFreezeMedia(_freezeMediaCallback);		}
	if (_resumeMediaCallback  != NULL)	{	dev->setOnResumeMedia(_resumeMediaCallback);		}
	if (_resizeMediaCallback  != NULL)	{	dev->setOnResizeMedia(_resizeMediaCallback);		}

	if (_mediaPropertyCallback   != NULL)	{	dev->setOnMediaPropertyChanged(_mediaPropertyCallback);		}

	if (_getPropertyCallback  != NULL)	{	sys->setOnGetProperty(_getPropertyCallback);		}
	if (_getNativePropertyCallback  != NULL)	{	sys->setOnGetNativeProperty(_getNativePropertyCallback);		}
	if (_setPropertyCallback  != NULL)	{	sys->setOnSetProperty(_setPropertyCallback);		}
	if (_keySetUpdateCallback != NULL)	{	sys->setOnKeySetUpdate(_keySetUpdateCallback);		}

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::execute() {

	int iResult = 0;
	bool bResult = false;

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);

	notifyBeforeStart();

	iResult = changeToDirectory(_application_path);
	if (iResult == -1)
	{
		setRunning(false);
		setWorking(false);
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Error: Loading Path", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	applyRecommendedResolution();
	printConfiguration();

	bResult = pem->startDocument(_application_path.string());

	if (bResult == false)
	{
		setRunning(false);
		setWorking(false);
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] Error: Starting Document", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return SUCCESS;
}


int BrowserPresentationEngine::destroy() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);
	pem->qStopDocument();
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return SUCCESS;
}

/**
 *
 *
 *
 */
int BrowserPresentationEngine::waitInit() {

	int iWaitInit = 0;
	while (!isInitialized()){
		iWaitInit++;
		if (iWaitInit > 10) {
			LTRACE("Port", "[BrowserPresentationEngine::%s::%d] init timeout", __FUNCTION__, __LINE__);
			return FAILURE;
		}
		sleep(1);
	}
	return SUCCESS;
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::stop() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);

	if (waitInit() == FAILURE) {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: No reference to application", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	if (pem == NULL) {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: No reference to application", __FUNCTION__, __LINE__);
		return FAILURE;
	}
	if (isRunning() == false){
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: No application running", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	if (isWorking() == true){
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: No application running", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	setWorking(true);

	destroy();

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return SUCCESS;
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::loadApplication(const std::string &path) {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN - Application Path = %s", __FUNCTION__, __LINE__, path.c_str());
	int res = -1;

	if (waitInit() == FAILURE) {
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: Engine initialization failed", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	if (isRunning() == true){
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: Application is already running", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	res = notifyLoadFileInvocation(path);

	_application_path = boost::filesystem::path(path);

	res = execute();

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END", __FUNCTION__, __LINE__);
	return res;
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::notifyLoadFileInvocation(const std::string &filename) {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);
	int res = -1;
	if (_loadMainFileCallback != 0) {
		res = _loadMainFileCallback(filename);
	}
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END - Result = [%d]", __FUNCTION__, __LINE__, res);
	return res;
}

/**
 *
 *
 *
 */
int BrowserPresentationEngine::notifyBeforeStart(void) {

	setRunning(true);

	if (_beforeStartCallback != NULL) {
		_beforeStartCallback( &appInfo );
	}
	return SUCCESS;
}

/**
 *
 *
 *
 */
int BrowserPresentationEngine::notifyStopped() {
	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] BEGIN", __FUNCTION__, __LINE__);
	int res = -1;

	if (_stoppedCallback != 0) {
		res = _stoppedCallback ( &appInfo ); //onStoppedApp
	}

	setRunning(false);
	setWorking(false);

	LTRACE("Port", "[BrowserPresentationEngine::%s::%d] END - Result = [%d]", __FUNCTION__, __LINE__, res);
	return res;
}


/**
 *
 *
 *
 */
const std::string &BrowserPresentationEngine::makeDescription(std::string &group, std::string &category) const {
	std::string *desc = new std::string("Level to log for ");
	if (category == "all") {
		(*desc) += "all categories in ";
		(*desc) += group;
		(*desc) += " group";
	} else {
		(*desc) += category;
		(*desc) += " category in  ";
		(*desc) += group;
		(*desc) += " group";
	}
	return (*desc);
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::debug() const {
	std::vector<std::string> arguments;

	boost::split(arguments, util::cfg::getValue<std::string> ("tool.debug"), boost::is_any_of("&"));

	BOOST_FOREACH( std::string arg, arguments ) {
		if (!arg.empty()) {
			bool badFormat = true;
			if (arg.find( "." ) != std::string::npos) {
				size_t from = 0;
				size_t to = arg.find(".");
				std::string group = arg.substr(from, to-from);
				from = to+1;
				to = arg.find(".", from);
				if (to != std::string::npos) {
					std::string category = arg.substr(from, to-from);
					from = to+1;
					to = arg.find(".", from);
					if (to == std::string::npos) {
						std::string level = arg.substr(from, to-from);
						badFormat = false;
						if (!util::cfg::get("log.filter").existsNode(group)) {
							util::cfg::PropertyNode &node = util::cfg::get("log.filter").addNode(group);
							if (category != "all") {
								std::string cat("all");
								node.addValue(cat, makeDescription(group, cat), "");
								node.set<std::string>(cat, util::cfg::getValue<std::string>("log.filter.all.all"));
							}
							node.addValue(category, makeDescription(group, category), "");
						}
						if (!util::cfg::get(std::string("log.filter.")+group).existsValue(category)) {
							util::cfg::get(std::string("log.filter.")+group).addValue(category, makeDescription(group, category), "");
						}
						util::cfg::get(std::string("log.filter.")+group).set<std::string>(category, level);
					}
				}
			}
			if (badFormat) {
				throw std::runtime_error( "bad format of command line debug option, format is: [group].[category].[level]");
			}
		}
	}
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::loadConfig() {
	util::cfg::XmlParser parser;
	BOOST_FOREACH(std::string xml, _cfgXmls) {
		if (boost::filesystem::exists(xml)) {
			LDEBUG("Port", "Load configuration from xml file: %s", xml.c_str());
			boost::filesystem::path path(xml);
			parser.parserIn(path.string(), &util::cfg::get());
		}

	}
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::registerProperties(util::cfg::cmd::CommandLine &cmd) {
	util::cfg::get().addNode("tool").addNode(name());

	// tool.help
	registerProp("tool", "help",    'h', "Display this message", false, cmd, false);
	registerProp("tool", "version", 'v', "Display tool version", version(), cmd, false);
	registerProp("tool", "build",   'b', "Display build information", build(), cmd, false);

	registerProp("tool", "debug",    "Set the categories to log with their log level", std::string(""), cmd, false);
	registerProp("tool", "defaults", "Show in xml format the configuration default values", false, cmd, false);
	registerProp("tool", "pretend",  "Show in xml format the configuration pretend values", false, cmd, false);
	registerProp("tool", "set",      "Sets the specified configuration values", std::string(""), cmd, false);


	// tool.ginga.ncl
	registerToolProp("ncl", "Start ncl file", std::string(""), cmd);

	// tool.ginga.baseId; tool.ginga.size
	registerToolProp("baseId", 'i', "Set the BaseId name",               std::string("-1"),       cmd);
	registerToolProp("size",   's', "Window size (1280x720 or 720x576)", std::string("1280x720"), cmd);

#if HTV_NC_VER >= 5 /* webOS and higher version */
	registerToolProp("winid",  'w', "Window ID (this value should be hexadecimal)", std::string("0xffff000f"), cmd);
#endif /* HTV_NC_VER >= 5 */
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::registerOpt(util::cfg::cmd::CommandLine &cmd, const std::string &owner, const std::string &prop, bool visible, char sname) {
	if (sname) {
		cmd.registerOpt(owner + "." + prop, prop, sname);
	} else {
		cmd.registerOpt(owner + "." + prop, prop);
	}
	if (!visible) {
		cmd.noShowDefault( prop );
	}
}


/**
 *
 *
 *
 */
void BrowserPresentationEngine::registerConfigs() {
	char *env = getenv("HOME");
	std::string home;

	if (env) {
		home = env;
		addConfig(home + "/tool.cfg.xml");
	}

	addConfig("tool.cfg.xml");

	if (env) {
		addConfig(home + "/" + name() + ".cfg.xml");
	}

	addConfig(name() + ".cfg.xml");
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::changeToDirectory(boost::filesystem::path path) {
	LDEBUG("Port", "BrowserPresentationEngine::%s:: Path = %s", __FUNCTION__, path.string().c_str());

	int res = -1;
	boost::filesystem::path temp(path.string());

	if (!temp.is_absolute()) {
		temp = boost::filesystem::current_path() / temp;
	}

	if (exists(temp) && is_regular_file(temp)) {
		boost::filesystem::path workingDir(temp);

		LDEBUG("Port", "Working Directory to [%s]", workingDir.string().c_str());

		const char *workingDir_char = workingDir.remove_filename().string().c_str();
		res = chdir(workingDir_char);

		// TODO
		pem->path = std::string(workingDir_char);
		appInfo.setPath(std::string(workingDir_char));

		if (res != 0) {
			LERROR("Port", "Cannot change directory because of [%s(%d)]", strerror(errno), errno);
		}
	}

	LDEBUG("Port", "BrowserPresentationEngine::%s:: Result = %d", __FUNCTION__, res);
	return res;
}

//	Key press
static boost::condition_variable _keyWakeup;
static boost::mutex _keyMutex;
bool _keyPressed = false;


void BrowserPresentationEngine::onKeyPressed( ::util::key::type /*key*/) {
	_keyMutex.lock();
	_keyPressed = true;
	_keyMutex.unlock();
	_keyWakeup.notify_one();
}


/**
 *
 *
 *
 */
int BrowserPresentationEngine::putKey(unsigned int key, unsigned int keyType) {
	LDEBUG("Port", "[BrowserPresentationEngine::%s::%d] key = %d[0x%x], key type = %d[0x%x]",
				   __FUNCTION__, __LINE__,
				   key, key,
				   keyType, keyType);

	::util::key::type _key = util::key::null;
	bool _kup = 0;

	if (!_isInitialized) {
		LDEBUG("Port", "[BrowserPresentationEngine::%s::%d] FATAL!! NOT INIT!!! dispatch key = %d", __FUNCTION__, __LINE__, _key);
		return FAILURE;
	}

	if (isRunning() == false){
		LTRACE("Port", "[BrowserPresentationEngine::%s::%d] FATAL: Don't inject the key", __FUNCTION__, __LINE__);
		return FAILURE;
	}

	_kup = (keyType == 402 /*RELEASE*/ ? 1 : 0);
	//_kup = isUp = 1 : release
	//_kup = isUp = 0 : press

	switch (key)	{
		case 80: _key = ::util::key::enter;		LDEBUG("Port", "%s key ENTER", _kup ? "^^" : "~~"); 	break;
		case 81: _key = ::util::key::back;		LDEBUG("Port", "%s key BACK",  _kup ? "^^" : "~~");	break;
		case 82: _key = ::util::key::exit;		LDEBUG("Port", "%s key EXIT",  _kup ? "^^" : "~~");	break;
		case 27: _key = ::util::key::exit;		LDEBUG("Port", "%s key EXIT",  _kup ? "^^" : "~~");	break;

		case 48: _key = ::util::key::number_0;		LDEBUG("Port", "%s key 0", _kup ? "^^" : "~~");		break;
		case 49: _key = ::util::key::number_1;		LDEBUG("Port", "%s key 1", _kup ? "^^" : "~~");		break;
		case 50: _key = ::util::key::number_2;		LDEBUG("Port", "%s key 2", _kup ? "^^" : "~~");		break;
		case 51: _key = ::util::key::number_3;		LDEBUG("Port", "%s key 3", _kup ? "^^" : "~~");		break;
		case 52: _key = ::util::key::number_4;		LDEBUG("Port", "%s key 4", _kup ? "^^" : "~~");		break;
		case 53: _key = ::util::key::number_5;		LDEBUG("Port", "%s key 5", _kup ? "^^" : "~~");		break;
		case 54: _key = ::util::key::number_6;		LDEBUG("Port", "%s key 6", _kup ? "^^" : "~~");		break;
		case 55: _key = ::util::key::number_7;		LDEBUG("Port", "%s key 7", _kup ? "^^" : "~~");		break;
		case 56: _key = ::util::key::number_8;		LDEBUG("Port", "%s key 8", _kup ? "^^" : "~~");		break;
		case 57: _key = ::util::key::number_9;		LDEBUG("Port", "%s key 9", _kup ? "^^" : "~~");		break;

		case 33: _key = ::util::key::red;		LDEBUG("Port", "%s key RED",    _kup ? "^^" : "~~");	break;
		case 34: _key = ::util::key::green;		LDEBUG("Port", "%s key GREEN",  _kup ? "^^" : "~~");	break;
		case 35: _key = ::util::key::yellow;		LDEBUG("Port", "%s key YELLOW", _kup ? "^^" : "~~");	break;
		case 36: _key = ::util::key::blue;		LDEBUG("Port", "%s key BLUE",   _kup ? "^^" : "~~");	break;

		case 37: _key = ::util::key::cursor_left;	LDEBUG("Port", "%s key LEFT",  _kup ? "^^" : "~~");	break;
		case 38: _key = ::util::key::cursor_up;		LDEBUG("Port", "%s key UP",    _kup ? "^^" : "~~");	break;
		case 39: _key = ::util::key::cursor_right;	LDEBUG("Port", "%s key RIGHT", _kup ? "^^" : "~~");	break;
		case 40: _key = ::util::key::cursor_down;	LDEBUG("Port", "%s key DOWN",  _kup ? "^^" : "~~");	break;

		default: return FAILURE;
	}



	pem->enqueue( boost::bind( &player::Device::dispatchKey, pem->mainScreen(), _key, _kup ) );



	//	Wait for compare
	boost::unique_lock<boost::mutex> lock( _keyMutex );
	while (!_keyPressed) {
		_keyWakeup.wait( lock );
	}
	_keyPressed = false;


	LDEBUG("Port", "[BrowserPresentationEngine::%s::%d] Key was dispatched!", __FUNCTION__, __LINE__);
	return SUCCESS;
}



int BrowserPresentationEngine::onMediaStop(SINT32 handlerId) {
	dev->onMediaStop(handlerId);
	return 0;
}



int BrowserPresentationEngine::notifyUserPropertyChanged(unsigned int changeType, char *key, char *oldValue, char *newValue) {
	LDEBUG("Port", "BrowserPresentationEngine::%s:: ChangeType[%d] key[%s] oldvalue[%s] newValue[%s]", __FUNCTION__, changeType, key == NULL ? key : "", oldValue != NULL ? oldValue : "", NULL != newValue ? newValue : "");

	//configureDebug(1,7);

	LDEBUG("Port", "BrowserPresentationEngine::%s:: Result = %d", __FUNCTION__, 0);
	return 0;
}

int BrowserPresentationEngine::notifyGetNativeProperty(char *value) {
	LDEBUG("Port", "BrowserPresentationEngine::%s:: Result = %d", __FUNCTION__, 0);
	return 0;
}
int BrowserPresentationEngine::notifyRecommendedResolution(int width, int height) {
	int result = 0;

	LDEBUG("Port", "BrowserPresentationEngine::%s:: New Recommended Resolution notification: (%d x %d)", __FUNCTION__, width, height);

	recommendedWidth  = (width  <= 0 ? DEFAULT_SCREEN_WIDTH  : width);
	recommendedHeight = (height <= 0 ? DEFAULT_SCREEN_HEIGHT : height);

	LDEBUG("Port", "BrowserPresentationEngine::%s:: Result = [%d]", __FUNCTION__, result);
	return result;
}

int BrowserPresentationEngine::notifyMediaStop(SINT32 handlerId) {
	
	dev->stop();
	
	return 0;
}
int BrowserPresentationEngine::notifyRequestKeySet(const std::string & value) {
	int result = 0;

	LDEBUG("Port", "BrowserPresentationEngine::%s:: Key Capture: [%s]", __FUNCTION__, value.c_str());
	util::cfg::setValue("channel.keyCapture", value);
	LDEBUG("Port", "BrowserPresentationEngine::%s:: End", __FUNCTION__);
	return result;
}



void BrowserPresentationEngine::setWindowTitle(std::string title) {
	LDEBUG("Port", "BrowserPresentationEngine::%s:: Set Title: [%s]", __FUNCTION__, title.c_str());
	util::cfg::setValue("gui.window.title", title);
	LDEBUG("Port", "BrowserPresentationEngine::%s:: End", __FUNCTION__);
}



void BrowserPresentationEngine::applyRecommendedResolution() {
	LDEBUG("Port", "BrowserPresentationEngine::%s:: Applying Recommended Resolution", __FUNCTION__);
	util::cfg::setValue("gui.canvas.size.width",  recommendedWidth);
	util::cfg::setValue("gui.canvas.size.height", recommendedHeight);
	util::cfg::setValue("gui.window.size.width",  recommendedWidth);
	util::cfg::setValue("gui.window.size.height", recommendedHeight);

	//util::cfg::setValue("gui.canvas.size.width",  canvasWidth);
	//util::cfg::setValue("gui.canvas.size.height", canvasHeight);
	//util::cfg::setValue("gui.window.size.width",  windowWidth);
	//util::cfg::setValue("gui.window.size.height", windowHeight);
}



int BrowserPresentationEngine::configureDebug(unsigned int /*enable*/, unsigned int /*level*/) {

#if 0
	if (enable > 1){
		return;
	}
	if (level > 7){
		return;
	}

	//enable or disable
	if (!util::cfg::get( "tool" ).hasValue( "log" )) {
		util::cfg::get( "tool" ).addValue( "log", "Logging is enabled/disabled", enable );
	}
	else {
		util::cfg::get( "tool" ).set( "log", enable );
	}

	//filter
	if (!util::cfg::get( "tool" ).hasValue( "filter" )) {
		util::cfg::get( "tool" ).addValue( "filter", "Level to log for all categories in this group", level );
	}
	else {
		util::cfg::get( "tool" ).set( "filter", level );
	}

	//output
	if (!util::cfg::get( "tool" ).hasValue( "output" )) {
		util::cfg::get( "tool" ).addValue( "filter", "Level to log for all categories in this group", level );
	}
	else {
		util::cfg::get( "tool" ).set( "filter", level );
	}
#endif

	return 0;
}


