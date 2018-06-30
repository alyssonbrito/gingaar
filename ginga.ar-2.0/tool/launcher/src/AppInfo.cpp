#include "AppInfo.h"

/**
 *
 *
 *
 */
XletAppRect::XletAppRect() : x(0), y(0), w(0), h(0) {
}

XletAppRect::XletAppRect(SINT32 x, SINT32 y, UINT32 w, UINT32 h) : x(x), y(y), w(w), h(h) {
}

XletAppRect& XletAppRect::operator= (const XletAppRect &rect) {
	this->x = rect.x;
	this->y = rect.y;
	this->w = rect.w;
	this->h = rect.h;
	return *this;
}

bool XletAppRect::operator== (const XletAppRect& rhs) {
	return ( this->x == rhs.x && this->y == rhs.y && this->w == rhs.w && this->h == rhs.h );
}

std::string XletAppRect::toString() const {
	std::stringstream ss;
	ss << "Rect: [ position(";
	ss << this->x << ", " << this->y;
	ss << "), size(";
	ss << this->w << " x " << this->h;
	ss << ") ]";
	return ss.str();
}

std::ostream& operator<< (std::ostream& os, const XletAppRect& rect) {
	os << rect.toString();
	return os;
}

/**
 *
 *
 *
 */
XletAppInfo::XletAppInfo() {
}

XletAppInfo::XletAppInfo(std::string classname, std::string classpath) : classname(classname),
																		 classpath(classpath) {
}

std::string XletAppInfo::getClassName(void) const {
	return this->classname;
}

void XletAppInfo::setClassName(const std::string &classname) {
	this->classname = classname;
}

std::string XletAppInfo::getClassPath(void) const {
	return this->classpath;
}

void XletAppInfo::setClassPath(const std::string &classpath) {
	this->classpath = classpath;
}

XletAppRect XletAppInfo::getRect() const {
	return this->rect;
}

void XletAppInfo::setRect(const XletAppRect & rect) {
	this->rect = rect;
}

XletAppInfo& XletAppInfo::operator= (const XletAppInfo &info) {
	this->classname = info.classname;
	this->classpath = info.classpath;
	this->rect      = info.rect;
	return *this;
}

bool XletAppInfo::operator== (const XletAppInfo& rhs) {
	return ( this->classname == rhs.classname && this->classpath == rhs.classpath && this->rect == rhs.rect );
}

std::string XletAppInfo::toString() const {
	std::stringstream ss;
	ss << "XletAppInfo: { Classname: ['";
	ss << this->classname << "'], Classpath: ['";
	ss << this->classpath;
	ss << "'] }, ";
	ss << this->rect;
	return ss.str();
}

std::ostream& operator<< (std::ostream& os, const XletAppInfo& rect) {
	os << rect.toString();
	return os;
}


/**
 *
 *
 *
 */
UINT16 AppInfo::getOnId(void) {
	return onid;
}

void AppInfo::setOnId(UINT16 onid) {
	this->onid = onid;
}

UINT16 AppInfo::getTsID(void) {
	return tsid;
}

void AppInfo::setTsID(UINT16 tsid) {
	this->tsid = tsid;
}

UINT16 AppInfo::getSID(void) {
	return sid;
}

void AppInfo::setSID(UINT16 sid) {
	this->sid = sid;
}

UINT32 AppInfo::getOrgID(void) {
	return org_id;
}

void AppInfo::setOrgID(UINT32 org_id) {
	this->org_id = org_id;
}

UINT16 AppInfo::getAppID(void) {
	return app_id;
}

void AppInfo::setAppID(UINT16 app_id) {
	this->app_id = app_id;
}

UINT8 AppInfo::getCtag(void) {
	return ctag;
}

void AppInfo::setCtag(UINT8 ctag) {
	this->ctag = ctag;
}

UINT32 AppInfo::getAttachUniqueKey(void) {
	return attachUniqueKey;
}

void AppInfo::setAttachUniqueKey(UINT32 attachUniqueKey) {
	this->attachUniqueKey = attachUniqueKey;
}

int AppInfo::getStartControl(void) {
	return iControlStart;
}

void AppInfo::setStartControl(int iControlStart) {
	this->iControlStart = iControlStart;
}

std::string AppInfo::getPath(void) const {
	return this->path;
}

void AppInfo::setPath(const std::string & path) {
	this->path = path;

	if ( !this->path.empty() ) {
		if ( this->path.at ( this->path.length () - 1 ) != '/' ) {
			this->path += '/';
		}
	}
}

XletAppInfo AppInfo::getXletInfo(void) const {
	return this->xletInfo;
}

void AppInfo::setXletInfo(const XletAppInfo & xletInfo) {
	this->xletInfo = xletInfo;
}


