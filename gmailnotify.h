#ifndef GMAILNOTIFY_H
#define GMAILNOTIFY_H

#include <QUrl>
#include <definitions/resources.h>
#include <definitions/notificationdataroles.h>
#include <interfaces/ipluginmanager.h>
#include <interfaces/ixmppstreams.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/iservicediscovery.h>
#include <interfaces/inotifications.h>
#include <utils/iconstorage.h>
#include <utils/stanza.h>

#define GMAILNOTIFY_UUID "{ff4ab60b-60d1-45f3-942a-68dc80624408}"

struct GmailSender
{
	QString name;
	QString address;
	bool originator;
	bool unread;
};

struct GmailThread
{
	QString threadId;
	int participation;
	int messages;
	qlonglong timestamp;
	QUrl threadUrl;
	QString labels;
	QString subject;
	QString snippet;
	QList<GmailSender> senders;
};

struct GmailReply
{
	Jid streamJid;
	qlonglong resultTime;
	int totalMatched;
	int totalEstimate;
	QUrl mailUrl;
	QList<GmailThread> theads;
};

class GmailNotify : 
	public QObject,
	public IPlugin,
	public IStanzaHandler,
	public IStanzaRequestOwner
{
	Q_OBJECT;
	Q_INTERFACES(IPlugin IStanzaHandler IStanzaRequestOwner);
public:
	GmailNotify();
	~GmailNotify();
	//IPlugin
	virtual QObject *instance() { return this; }
	virtual QUuid pluginUuid() const { return GMAILNOTIFY_UUID; }
	virtual void pluginInfo(IPluginInfo *APluginInfo);
	virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
	virtual bool initObjects();
	virtual bool initSettings() { return true; }
	virtual bool startPlugin() { return true; }
	//IStanzaHandler
	virtual bool stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept);
	//IStanzaRequestOwner
	virtual void stanzaRequestResult(const Jid &AStreamJid, const Stanza &AStanza);
	virtual void stanzaRequestTimeout(const Jid &AStreamJid, const QString &AStanzaId);
	//IGmailNotify
	virtual bool isSupported(const Jid &AStreamJid) const;
protected:
	bool checkNewMail(const Jid &AStreamJid);
	GmailReply parseGmailReply(const Stanza &AStanza) const;
	void notifyGmailReply(const GmailReply &AReply);
protected:
	void registerDiscoFeatures();
	void insertStanzaHandler(const Jid &AStreamJid);
	void removeStanzaHandler(const Jid &AStreamJid);
protected slots:
	void onXmppStreamOpened(IXmppStream *AXmppStream);
	void onXmppStreamClosed(IXmppStream *AXmppStream);
	void onNotificationActivated(int ANotifyId);
	void onNotificationRemoved(int ANotifyId);
private:
	IXmppStreams *FXmppStreams;
	IServiceDiscovery *FDiscovery;
	IStanzaProcessor *FStanzaProcessor;
	INotifications *FNotifications;
private:
	QMap<Jid,int> FSHIGmailNotify;
private:
	QMap<int,GmailReply> FNotifies;
	QMap<QString,Jid> FMailRequests;
};

#endif // GMAILNOTIFY_H
