#include "gmailnotify.h"

#include <QDir>
#include <QApplication>
#include <QTextDocument>
#include <QDesktopServices>

//Namespaces
#define NS_GMAILNOTIFY                   "google:mail:notify"

//Notification Types
#define NNT_GMAIL_NOTIFY                 "GMailNotify"

//Options Widget Orders
#define OWO_GMAIL_NOTIFY                 275

//Menu Icons
#define MNI_GMAILNOTIFY_GMAIL            "gmailnotifyGmail"

#define SHC_GMAILNOTIFY                  "/iq[@type='set']/new-mail[@xmlns='" NS_GMAILNOTIFY "']"

#define NOTIFY_TIMEOUT                   30000

GmailNotify::GmailNotify()
{
	FDiscovery = NULL;
	FXmppStreams= NULL;
	FStanzaProcessor = NULL;
	FNotifications = NULL;

#ifdef RESOURCES_DIR
	FileStorage::setResourcesDirs(FileStorage::resourcesDirs() << (QDir::isAbsolutePath(RESOURCES_DIR) ? RESOURCES_DIR : qApp->applicationDirPath()+"/"+RESOURCES_DIR));
#endif
}

GmailNotify::~GmailNotify()
{

}

void GmailNotify::pluginInfo(IPluginInfo *APluginInfo)
{
	APluginInfo->name = tr("GMail Notifications");
	APluginInfo->description = tr("Notify of new e-mails in Google Mail");
	APluginInfo->version = "1.0";
	APluginInfo->author = "Potapov S.A. aka Lion";
	APluginInfo->homePage = "http://code.google.com/p/vacuum-plugins";
	APluginInfo->dependences.append(STANZAPROCESSOR_UUID);
}

bool GmailNotify::initConnections(IPluginManager *APluginManager, int &AInitOrder)
{
	Q_UNUSED(AInitOrder);

	IPlugin *plugin = APluginManager->pluginInterface("IStanzaProcessor").value(0);
	if (plugin)
	{
		FStanzaProcessor = qobject_cast<IStanzaProcessor *>(plugin->instance());
	}

	plugin = APluginManager->pluginInterface("IServiceDiscovery").value(0);
	if (plugin)
	{
		FDiscovery = qobject_cast<IServiceDiscovery *>(plugin->instance());
	}

	plugin = APluginManager->pluginInterface("IXmppStreams").value(0);
	if (plugin)
	{
		FXmppStreams = qobject_cast<IXmppStreams *>(plugin->instance());
		if (FXmppStreams)
		{
			connect(FXmppStreams->instance(),SIGNAL(opened(IXmppStream *)),SLOT(onXmppStreamOpened(IXmppStream *)));
			connect(FXmppStreams->instance(),SIGNAL(closed(IXmppStream *)),SLOT(onXmppStreamClosed(IXmppStream *)));
		}
	}

	plugin = APluginManager->pluginInterface("INotifications").value(0);
	if (plugin)
	{
		FNotifications = qobject_cast<INotifications *>(plugin->instance());
		if (FNotifications)
		{
			connect(FNotifications->instance(),SIGNAL(notificationActivated(int)),SLOT(onNotificationActivated(int)));
			connect(FNotifications->instance(),SIGNAL(notificationRemoved(int)),SLOT(onNotificationRemoved(int)));
		}
	}

	return FStanzaProcessor!=NULL;
}

bool GmailNotify::initObjects()
{
	if (FDiscovery)
	{
		registerDiscoFeatures();
	}
	if (FNotifications)
	{
		uchar kindMask = INotification::PopupWindow|INotification::TrayIcon|INotification::PlaySound|INotification::AutoActivate;
		uchar kindDefs = INotification::PopupWindow|INotification::TrayIcon|INotification::PlaySound;
		FNotifications->registerNotificationType(NNT_GMAIL_NOTIFY,OWO_GMAIL_NOTIFY,tr("GMail Notifications"),kindMask,kindDefs);
	}
	return true;
}

bool GmailNotify::stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept)
{
	if (FSHIGmailNotify.value(AStreamJid) == AHandleId)
	{
		AAccept = true;
		Stanza reply("iq");
		reply.setType("result").setId(AStanza.id());
		FStanzaProcessor->sendStanzaOut(AStreamJid,reply);
		checkNewMail(AStreamJid);
	}
	return false;
}

void GmailNotify::stanzaRequestResult(const Jid &AStreamJid, const Stanza &AStanza)
{
	if (FMailRequests.contains(AStanza.id()))
	{
		FMailRequests.remove(AStanza.id());
		if (AStanza.type() == "result")
		{
			insertStanzaHandler(AStreamJid);
			notifyGmailReply(parseGmailReply(AStanza));
		}
	}
}

void GmailNotify::stanzaRequestTimeout(const Jid &AStreamJid, const QString &AStanzaId)
{
	if (FMailRequests.contains(AStanzaId))
	{
		FMailRequests.remove(AStanzaId);
		checkNewMail(AStreamJid);
	}
}

bool GmailNotify::isSupported(const Jid &AStreamJid) const
{
	return FSHIGmailNotify.contains(AStreamJid);
}

bool GmailNotify::checkNewMail(const Jid &AStreamJid)
{
	if (!FMailRequests.values().contains(AStreamJid.bare()))
	{
		Stanza query("iq");
		query.setType("get").setId(FStanzaProcessor->newId());
		QDomElement queryElem = query.addElement("query",NS_GMAILNOTIFY);
		if (FStanzaProcessor->sendStanzaRequest(this,AStreamJid,query,NOTIFY_TIMEOUT))
		{
			FMailRequests.insert(query.id(),AStreamJid.bare());
			return true;
		}
		return false;
	}
	return true;
}

GmailReply GmailNotify::parseGmailReply(const Stanza &AStanza) const
{
	GmailReply reply;
	reply.streamJid = AStanza.to();
	reply.resultTime = 0;
	reply.totalMatched = 0;
	reply.totalEstimate = 0;

	QDomElement replyElem = AStanza.firstElement("mailbox",NS_GMAILNOTIFY);
	if (!replyElem.isNull())
	{
		reply.resultTime = replyElem.attribute("result-time").toLongLong();
		reply.totalMatched = replyElem.attribute("total-matched").toInt();
		reply.totalEstimate = replyElem.attribute("total-estimate").toInt();
		reply.mailUrl = replyElem.attribute("url");

		QDomElement theadElem = replyElem.firstChildElement("mail-thread-info");
		while (!theadElem.isNull())
		{
			GmailThread gthread;
			gthread.threadId = theadElem.attribute("tid");
			gthread.participation = theadElem.attribute("participation").toInt();
			gthread.messages = theadElem.attribute("messages").toInt();
			gthread.timestamp = theadElem.attribute("date").toLongLong();
			gthread.threadUrl = theadElem.attribute("url");
			gthread.labels = theadElem.firstChildElement("labels").text();
			gthread.subject = theadElem.firstChildElement("subject").text();
			gthread.snippet = theadElem.firstChildElement("snippet").text();

			QDomElement senderElem = theadElem.firstChildElement("senders").firstChildElement("sender");
			while (!senderElem.isNull())
			{
				GmailSender sender;
				sender.name = senderElem.attribute("name");
				sender.address = senderElem.attribute("address");
				sender.originator = senderElem.attribute("originator").toInt() == 1;
				sender.unread = senderElem.attribute("unread").toInt() == 1;

				gthread.senders.append(sender);
				senderElem = senderElem.nextSiblingElement("sender");
			}

			reply.theads.append(gthread);
			theadElem = theadElem.nextSiblingElement("mail-thread-info");
		}
	}

	return reply;
}

void GmailNotify::notifyGmailReply(const GmailReply &AReply)
{
	if (FNotifications && AReply.theads.count()>0)
	{
		INotification notify;
		notify.kinds = FNotifications->notificationKinds(NNT_GMAIL_NOTIFY);
		notify.type = NNT_GMAIL_NOTIFY;
		if (notify.kinds > 0)
		{
			notify.data.insert(NDR_ICON,IconStorage::staticStorage(RSR_STORAGE_MENUICONS)->getIcon(MNI_GMAILNOTIFY_GMAIL));
			notify.data.insert(NDR_TOOLTIP,tr("New e-mail for %1").arg(AReply.streamJid.bare()));
			if (AReply.theads.count() > 1)
			{
				notify.data.insert(NDR_POPUP_CAPTION,tr("New e-mail"));
				notify.data.insert(NDR_POPUP_TITLE,AReply.streamJid.bare());
				notify.data.insert(NDR_POPUP_HTML,Qt::escape(tr("You have %n unread message(s)","",AReply.totalMatched)));
			}
			else if (AReply.theads.count() == 1)
			{
				GmailThread gthread = AReply.theads.value(0);
				GmailSender sender = gthread.senders.value(0);
				notify.data.insert(NDR_POPUP_CAPTION,tr("New e-mail for %1").arg(AReply.streamJid.bare()));
				notify.data.insert(NDR_POPUP_TITLE,!sender.name.isEmpty() ? QString("%1 <%2>").arg(sender.name).arg(sender.address) : sender.address);
				notify.data.insert(NDR_POPUP_HTML,Qt::escape(gthread.subject + " - " + gthread.snippet));
			}
			int notifyId = FNotifications->appendNotification(notify);
			FNotifies.insert(notifyId,AReply);
		}
	}
}

void GmailNotify::registerDiscoFeatures()
{
	IDiscoFeature dfeature;
	dfeature.var = NS_GMAILNOTIFY;
	dfeature.active = false;
	dfeature.name = tr("GMail Notifications");
	dfeature.description = tr("Supports the notifications of new e-mails in Google Mail");
	FDiscovery->insertDiscoFeature(dfeature);
}

void GmailNotify::insertStanzaHandler(const Jid &AStreamJid)
{
	if (FStanzaProcessor && !FSHIGmailNotify.contains(AStreamJid))
	{
		IStanzaHandle shandle;
		shandle.handler = this;
		shandle.streamJid = AStreamJid;
		shandle.direction = IStanzaHandle::DirectionIn;
		shandle.order = SHO_DEFAULT;
		shandle.conditions.append(SHC_GMAILNOTIFY);
		FSHIGmailNotify.insert(AStreamJid,FStanzaProcessor->insertStanzaHandle(shandle));
	}
}

void GmailNotify::removeStanzaHandler(const Jid &AStreamJid)
{
	if (FStanzaProcessor)
	{
		FStanzaProcessor->removeStanzaHandle(FSHIGmailNotify.take(AStreamJid));
	}
}

void GmailNotify::onXmppStreamOpened(IXmppStream *AXmppStream)
{
	checkNewMail(AXmppStream->streamJid());
}

void GmailNotify::onXmppStreamClosed(IXmppStream *AXmppStream)
{
	removeStanzaHandler(AXmppStream->streamJid());
}

void GmailNotify::onNotificationActivated( int ANotifyId )
{
	if (FNotifies.contains(ANotifyId))
	{
		GmailReply reply = FNotifies.take(ANotifyId);
		QDesktopServices::openUrl(reply.theads.count()>1 ? reply.mailUrl : reply.theads.value(0).threadUrl);
		FNotifications->removeNotification(ANotifyId);
	}
}

void GmailNotify::onNotificationRemoved(int ANotifyId)
{
	FNotifies.remove(ANotifyId);
}

Q_EXPORT_PLUGIN2(plg_gmailnotify, GmailNotify)
