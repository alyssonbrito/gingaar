#pragma once

#include "../event.h"

namespace player {
namespace event {
namespace si {


class SiStream {
public:
	SiStream();
	virtual ~SiStream() {};

	void setPid( int pid ) { _pid = pid; }
	void setComponentTag( int componenttag ) { _componentTag = componenttag; }
	void setType( int type ) { _type = type; }
	void setRegionSpecType( int regionSpecType ) { _regionSpecType = regionSpecType; }
	void setRegionSpec( const std::string &regionSpec ) { _regionSpec = regionSpec; }

	int pid() const { return _pid; }
	int componentTag() const { return _componentTag; }
	int type() const { return _type; }
	int regionSpecType() const { return _regionSpecType; }
	std::string regionSpec() const { return _regionSpec; }

private:
	int _pid;
	int _componentTag;
	int _type;
	int _regionSpecType;
	std::string _regionSpec;
};

class EventTypeService {
public:
	EventTypeService();
	virtual ~EventTypeService() {};

	//	Initialization
	bool initializeWithDefaults() { return true; };


	void setId( int id ) { _id = id; }
	void setAvailable( bool available ) { _isAvailable = available; }
	void setPartialReception( bool partialReception ) { _isPartialReception = partialReception; }
	void setParentControlRating( int parentalControlRating ) { _parentalControlRating = parentalControlRating; }
	void setRunningStatus( int runningStatus ) { _runningStatus = runningStatus; }
	void setServiceType( int serviceType ) { _serviceType = serviceType; }

	void setProviderName( const std::string &providerName ) { _providerName = providerName; }
	void setServiceName( const std::string &serviceName ) { _serviceName = serviceName; }
	void addStream( SiStream &stream ) { _stream.push_back(&stream); }


	int id() const { return _id; }
	bool isAvailable() const { return _isAvailable; }
	bool isPartialReception() const { return _isPartialReception ;}
	int parentalControlRating() const { return _parentalControlRating; }
	int runningStatus() const { return _runningStatus; }
	int serviceType() const { return _serviceType; }
	std::string providerName() const { return _providerName; }
	std::string serviceName() const { return _serviceName; }
	std::vector<SiStream*> streams() const { return _stream; }


private:
	int _id;
	bool _isAvailable;
	bool _isPartialReception;
	int _parentalControlRating;
	int _runningStatus;
	int _serviceType;
	std::string _providerName;
	std::string _serviceName;
	std::vector<SiStream*> _stream;
};

typedef struct
{
	int logicalId;
	int presentationInfo;
	int id;
	int linkageInfo;
	int bouquetId;
	int networkId;
	int tsId;
	int serviceId;
	int eventId;
} EventTypeMosaic;

typedef struct
{
	int id;
	int tsId;
	int networkId;
	int serviceId;
} EventGroup;


typedef struct
{
	int id;
	int totalBitRate;
	std::string description;
	int CAUnitID;
	std::vector<int> tag;
} ComponentGroup;

typedef struct
{
	std::string startTime;
	std::string endTime;
	int runningStatus;
	std::string name;
	int originalNetworkId;
	std::string shortDescription;
	std::string extendedDescription;
	int copyrightId;
	std::string copyrightInfo;
	int parentalRating;
	std::string parentalRatingDescription;
	std::string audioLanguageCode;
	std::string audioLanguageCode2;
	std::string dataContentLanguageCode;
	std::string dataContentText;
	bool hasInteractivity;
	std::string logoURI;

	//	contentDescription
	std::string content_nibble_1;
	std::string content_nibble_2;
	std::string user_nibble_1;
	std::string user_nibble_2;

	//	Linkage
	int lTsId;
	int lNetworkId;
	int lServiceId;
	int lType;
	std::string lData;

	//	Hyperlink
	int hType;
	int hDestinationType;
	int hTsId;
	int hNetworkId;
	int hEventId;
	int hComponentTag;
	int hModuleId;
	int hServiceId;
	int hContentId;
	std::string hUrl;

	//	Series
	int sId;
	int sRepeatLabel;
	int sProgramPattern;
	int sEpisodeNumber;
	int sLastEpisodeNumber;
	std::string sName;

	//	EventGroup
	int eType;
	std::vector<EventGroup> eEventGroups;

	//	ComponentGroup
	int cType;
	std::vector<ComponentGroup> cComponentGroups;
} EventTypeEpg;

typedef struct
{
	int year;
	int month;
	int day;
	int hours;
	int minutes;
	int seconds;
} EventTypeTime;

//	Event class methods
int postEvent( lua_State *st, bool isOut, int eventPos );
void dispatchSi( Module *module, EventImpl *table );
void fillService(lua_State *st);

}
}
}

