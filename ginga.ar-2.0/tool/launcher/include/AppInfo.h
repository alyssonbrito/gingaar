#ifndef __APP_INFO_H__
#define __APP_INFO_H__

#include <iostream>
#include <string>
#include <sstream>

#include <pem-tv-util/pem_moa_common.h>


/**
*
*
*
*/
class XletAppRect {
public:
	XletAppRect();
	XletAppRect(SINT32 x, SINT32 y, UINT32 w, UINT32 h);
	XletAppRect& operator= (const XletAppRect & rect);
	bool operator== (const XletAppRect& rhs);

	std::string toString() const;

	friend std::ostream& operator<< (std::ostream& os, const XletAppRect& rect);

	SINT32 x; /* Bound of X */
	SINT32 y; /* Bound of Y */
	UINT32 w; /* Bound of Width */
	UINT32 h; /* Bound of Height */
};


/**
*
*
*
*/
class XletAppInfo {
public:
	XletAppInfo();
	XletAppInfo(std::string classname, std::string classpath);

	std::string getClassName(void) const;
	void setClassName(const std::string &classname);

	std::string getClassPath(void) const;
	void setClassPath(const std::string & classpath);

	XletAppRect getRect() const;
	void setRect(const XletAppRect & rect);

	XletAppInfo& operator= (const XletAppInfo & info);
	bool operator== (const XletAppInfo& rhs);

	std::string toString() const;

	friend std::ostream& operator<< (std::ostream& os, const XletAppInfo& rect);

private:
	//Information about Xlet that is embedded in NCL document
	std::string classname; /* Initial Classname, '\0' terminated */
	std::string classpath; /* Class Path, '\0' terminated */
	XletAppRect rect;
};


class AppInfo {
public:
	// Information about Application
	UINT16 getOnId(void);
	void setOnId(UINT16 onid);

	UINT16 getTsID(void);
	void setTsID(UINT16 tsid);

	UINT16 getSID(void);
	void setSID(UINT16 sid);

	UINT32 getOrgID(void);
	void setOrgID(UINT32 org_id);

	UINT16 getAppID(void);
	void setAppID(UINT16 app_id);

	UINT8 getCtag(void);
	void setCtag(UINT8 ctag);

	UINT32 getAttachUniqueKey(void);
	void setAttachUniqueKey(UINT32 attachUniqueKey);

	int getStartControl(void);
	void setStartControl(int iControlStart);

	std::string getPath(void) const;
	void setPath(const std::string & path);

	XletAppInfo getXletInfo(void) const;
	void setXletInfo(const XletAppInfo & xletInfo);


private:
	// Information about Application
	UINT16 onid;
	UINT16 tsid;
	UINT16 sid;
	UINT32 org_id;
	UINT16 app_id;
	UINT8 ctag;
	UINT32 attachUniqueKey;
	int iControlStart;
	std::string path; // Absolute Path
	XletAppInfo xletInfo;
};

#endif /* __APP_INFO_H__ */
