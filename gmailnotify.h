#ifndef GMAILNOTIFY_H
#define GMAILNOTIFY_H

#include "igmailnotify.h"
#include "definitions.h"

#include <QUrl>
#include <QPointer>
#include <definitions/resources.h>
#include <definitions/rosterindexkinds.h>
#include <definitions/rosterindexroles.h>
#include <definitions/notificationdataroles.h>
#include <interfaces/ipluginmanager.h>
#include <interfaces/ixmppstreammanager.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/iservicediscovery.h>
#include <interfaces/inotifications.h>
#include <interfaces/irostersview.h>
#include <utils/stanza.h>
#include <utils/options.h>
#include <utils/iconstorage.h>
#include <utils/widgetmanager.h>
#include <utils/advanceditemdelegate.h>
#include "notifygmaildialog.h"

class GmailNotify : 
	public QObject,
	public IPlugin,
	public IGmailNotify,
	public IStanzaHandler,
	public IStanzaRequestOwner,
	public IRostersClickHooker
{
	Q_OBJECT;
	Q_INTERFACES(IPlugin IGmailNotify IStanzaHandler IStanzaRequestOwner IRostersClickHooker);
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
	//IRostersClickHooker
	virtual bool rosterIndexSingleClicked(int AOrder, IRosterIndex *AIndex, const QMouseEvent *AEvent);
	virtual bool rosterIndexDoubleClicked(int AOrder, IRosterIndex *AIndex, const QMouseEvent *AEvent);
	//IGmailNotify
	virtual bool isSupported(const Jid &AStreamJid) const;
	virtual IGmailReply gmailReply(const Jid &AAccountJid) const;
	virtual QDialog *showNotifyDialog(const Jid &AAccountJid);
signals:
	void gmailReplyChanged(const Jid &AAccountJid, const IGmailReply &AReply);
protected:
	bool checkNewMail(const Jid &AStreamJid, bool AFull);
	void setGmailReply(const Jid &AStreamJid, const IGmailReply &AReply);
	IGmailReply parseGmailReply(const Stanza &AStanza) const;
	QList<int> findAccountNotifies(const Jid &AAccountJid) const;
	int findThreadNotify(const Jid &AAccountJid, const QString &AThreadId) const;
	void processGmailReply(const Jid &AStreamJid, const IGmailReply &AReply, bool AFull);
	void notifyGmailThreads(const Jid &AStreamJid, const QList<IGmailThread> &AThreads, bool ATotal);
protected:
	void registerDiscoFeatures();
	void insertStanzaHandler(const Jid &AStreamJid);
	void removeStanzaHandler(const Jid &AStreamJid);
protected slots:
	void onXmppStreamOpened(IXmppStream *AXmppStream);
	void onXmppStreamClosed(IXmppStream *AXmppStream);
	void onDiscoveryInfoReceived(const IDiscoInfo &AInfo);
	void onNotificationActivated(int ANotifyId);
	void onNotificationRemoved(int ANotifyId);
	void onRostersViewIndexToolTips(IRosterIndex *AIndex, quint32 ALabelId, QMap<int,QString> &AToolTips);
private:
	IServiceDiscovery *FDiscovery;
	IStanzaProcessor *FStanzaProcessor;
	INotifications *FNotifications;
	IRostersViewPlugin *FRostersViewPlugin;
	IXmppStreamManager *FXmppStreamManager;
private:
	QMap<Jid,int> FSHIGmailNotify;
	QMap<QString,bool> FMailRequests;
private:
	quint32 FGmailLabelId;
	QMap<int,Jid> FNotifies;
	QMap<Jid,IGmailReply> FAccountReply;
	QMap<Jid,QPointer<NotifyGmailDialog> > FAccountDialog;
};

#endif // GMAILNOTIFY_H
