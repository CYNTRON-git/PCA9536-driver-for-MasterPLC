#ifndef __ARCHIVE_SUBSCRIPTION_H__
#define __ARCHIVE_SUBSCRIPTION_H__

#include "share/base_subscription.h"

class ArchiveSubscription :
	public BaseSubscription
{
public:
	ArchiveSubscription() :
		/*_publishingInterval(0),
		_nextPacketNumber(0),*/
		_maxNotificationsPerPublish(0),
		_maxSize(0), _subId(0),
		_last_sent_packet(0)
		{
		}
	virtual ~ArchiveSubscription(){}

	int GetLifetimeInterval() const { return _lifetimeInterval; }
	//int GetPublishingInterval() const { return _publishingInterval; }
	int GetSubId() const { return _subId; }

	OpcUa_StatusCode Load(int subId, const Document& request, int nMaxResponseSize);
	OpcUa_StatusCode PublishHistoryData(const Document& request, ResponseWriter& writer);
	OpcUa_StatusCode AddRequest(const Document& request, int nMaxResponseSize);
private:
	uint32_t _last_sent_packet;
	char __buf[30];
	void WriteItems(ArchiveRequest &rq, ResponseWriter& writer);

	void GetNew(ArchiveRequest & rq, int arch_id);

	typedef std::map<int, ArchiveRequest> RequestsBySourceIndexMap;
	typedef std::map<uint32_t, RequestsBySourceIndexMap> RequestsMap;
	RequestsMap requests;
	uint32_t _subId;	//Id подписки, устанавливается сервером
	int _maxSize; //Максимальный размер пакета
	//int _publishingInterval; //Интервал мс, с которым клиент будет запрашивать уведомления. Пока не используется
	int _maxNotificationsPerPublish; //Максимальное количество уведомлений в пакете

	////typedef std::list<EventsItem> CEventsItemList;
	////CEventsItemList _items; //Набор выборок по данной подписке

	//						//dynamic data
	//int _nextPacketNumber; //Следующий номер для нового пакета, сквозная нумерация в рамках подписки
	////typedef std::list<CEventsPacket> CEventsPacketList;
	////CEventsPacketList _packets; //Набор пакетов, сформированных по данной подписке
};

#endif //__ARCHIVE_SUBSCRIPTION_H__
