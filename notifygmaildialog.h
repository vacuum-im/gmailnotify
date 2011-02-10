#ifndef NOTIFYGMAILDIALOG_H
#define NOTIFYGMAILDIALOG_H

#include <igmailnotify.h>
#include <definitions.h>

#include <QDialog>
#include <definitions/resources.h>
#include <utils/iconstorage.h>
#include "ui_notifygmaildialog.h"

class NotifyGmailDialog : 
	public QDialog
{
	Q_OBJECT;
public:
	NotifyGmailDialog(const Jid &AAccountJid, QWidget *AParent = NULL);
	~NotifyGmailDialog();
public:
	void setGmailReply(const IGmailReply &AReply);
protected slots:
	void onDialogAccepted();
	void onDialogRejected();
	void onLinkActivated(const QString &ALink);
private:
	Ui::NotifyGmailDialog ui;
private:
	IGmailReply FReply;
};

#endif // NOTIFYGMAILDIALOG_H
