#ifndef DEF_DEFINITIONS_H
#define DEF_DEFINITIONS_H

//Namespaces
#define NS_GMAILNOTIFY                   "google:mail:notify"
#define NS_GOOGLESETTINGS                "google:setting"

//Notification Types
#define NNT_GMAIL_NOTIFY                 "GMailNotify"

//Notification Type Orders
#define NTO_GMAIL_NOTIFY                 275

//Menu Icons
#define MNI_GMAILNOTIFY_GMAIL            "gmailnotifyGmail"

//Options
#define OPV_GMAILNOTIFY_ROOT             "gmail-notify"
#define OPV_GMAILNOTIFY_ACCOUNT_ITEM     "gmail-notify.account"
#define OPV_GMAILNOTIFY_ACCOUNT_LASTTID  "gmail-notify.account.last-tid"
#define OPV_GMAILNOTIFY_ACCOUNT_LASTTIME "gmail-notify.account.last-time"

//Roster Labels
#define RLID_GMAILNOTIFY                  AdvancedDelegateItem::makeId(AdvancedDelegateItem::MiddleCenter,128,AdvancedDelegateItem::AlignRightOrderMask | 444)

//Roster ToolTip Order
#define RTTO_GMAILNOTIFY                 555

//Roster Click Hooker Orders
#define RCHO_GMAILNOTIFY                 1000

#endif //DEF_DEFINITIONS_H
