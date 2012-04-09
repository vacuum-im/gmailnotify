#include "notifygmaildialog.h"

#include <QTextDocument>
#include <QDesktopServices>

NotifyGmailDialog::NotifyGmailDialog(const Jid &AAccountJid, QWidget *AParent) : QDialog(AParent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose,true);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	IconStorage::staticStorage(RSR_STORAGE_MENUICONS)->insertAutoIcon(this,MNI_GMAILNOTIFY_GMAIL,0,0,"windowIcon");

	setWindowTitle(tr("GMail Notify"));
	ui.lblAccount->setText(QString("<b>%1</b>").arg(AAccountJid.uBare()));

	connect(ui.dbbButtons,SIGNAL(accepted()),SLOT(onDialogAccepted()));
	connect(ui.dbbButtons,SIGNAL(rejected()),SLOT(onDialogRejected()));
	connect(ui.lblThreads,SIGNAL(linkActivated(const QString &)),SLOT(onLinkActivated(const QString &)));
}

NotifyGmailDialog::~NotifyGmailDialog()
{

}

void NotifyGmailDialog::setGmailReply(const IGmailReply &AReply)
{
	FReply = AReply;
	ui.lblInfo->setText(tr("You have <b>%n unread letter(s)</b>:","",AReply.totalMatched));
	
	QString list = "<ul>";
	foreach(const IGmailThread &gthread, FReply.theads)
		list += QString("<li><a href='%3'>%1</a> (%2)</li>").arg(Qt::escape(gthread.subject)).arg(gthread.messages).arg(gthread.threadUrl.toString());
	list += "</ul>";
	ui.lblThreads->setText(list);
}

void NotifyGmailDialog::onDialogAccepted()
{
	QDesktopServices::openUrl(FReply.mailUrl);
	accept();
}

void NotifyGmailDialog::onDialogRejected()
{
	reject();
}

void NotifyGmailDialog::onLinkActivated(const QString &ALink)
{
	QDesktopServices::openUrl(ALink);
	accept();
}
