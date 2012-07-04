#ifndef IGMAILNOTIFY_H
#define IGMAILNOTIFY_H

#include <QUrl>
#include <QList>
#include <QString>
#include <QDialog>
#include <utils/jid.h>

#define GMAILNOTIFY_UUID "{ff4ab60b-60d1-45f3-942a-68dc80624408}"

struct IGmailSender
{
	QString name;
	QString address;
	bool originator;
	bool unread;
};

struct IGmailThread
{
	QString threadId;
	int participation;
	int messages;
	qlonglong timestamp;
	QUrl threadUrl;
	QString labels;
	QString subject;
	QString snippet;
	QList<IGmailSender> senders;
};

struct IGmailReply
{
	QString resultTime;
	int totalMatched;
	int totalEstimate;
	QUrl mailUrl;
	QList<IGmailThread> theads;
};

class IGmailNotify
{
public:
	virtual QObject *instance() = 0;
	virtual bool isSupported(const Jid &AStreamJid) const =0;
	virtual IGmailReply gmailReply(const Jid &AAccountJid) const =0;
	virtual QDialog *showNotifyDialog(const Jid &AAccountJid) =0;
signals:
	virtual void gmailReplyChanged(const Jid &AAccountJid, const IGmailReply &AReply) =0;
};

Q_DECLARE_INTERFACE(IGmailNotify,"Vacuum.ExternalPlugin.IGmailNotify/1.0")

#endif // IGMAILNOTIFY_H
