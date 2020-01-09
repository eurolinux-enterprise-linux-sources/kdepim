/* KPilot
**
** Copyright (C) 2001 by Dan Pilone <dan@kpilot.org>
** Copyright (C) 2002-2004 by Adriaan de Groot <groot@kde.org>
** Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
**
** This file defines the pages that make up part of the configuration dialog.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

#include "options.h"

#include <kcharsets.h>
#include <kautostart.h>

#include "syncAction.h"

#include "kpilotConfig.h"
#include "kpilotSettings.h"

#include "config_dialog_probe.h"
#include "config_dialog_dbselection.h"

#include "config_pages.moc"

/* virtual */ QString ConfigPage::maybeSaveText() const
{
	return i18n("<qt>The settings for configuration page <i>%1</i> have been changed. Do you "
		"want to save the changes before continuing?</qt>", this->conduitName());
}

DeviceConfigPage::DeviceConfigPage(QWidget * w, QVariantList &args ) 
	: ConfigPage( w, args )
{
	FUNCTIONSETUP;

	fWidget = new QWidget(w);
	fConfigWidget.setupUi( fWidget );
	
	// Fill the encodings list
	{
		QStringList l = KGlobal::charsets()->descriptiveEncodingNames();
		for ( QStringList::Iterator it = l.begin(); it != l.end(); ++it )
		{
			fConfigWidget.fPilotEncoding->addItem(*it);
		}
	}
	
	connect( fConfigWidget.fDeviceAutodetect, SIGNAL(clicked()), this
		, SLOT(autoDetectDevice()));

#define CM(a,b) connect(fConfigWidget.a,b,this,SLOT(modified()));
	CM(fPilotDevice, SIGNAL(textChanged(const QString &)));
	CM(fPilotSpeed, SIGNAL(activated(int)));
	CM(fPilotEncoding, SIGNAL(textChanged(const QString &)));
	CM(fUserName, SIGNAL(textChanged(const QString &)));
	CM(fWorkaround, SIGNAL(activated(int)));
#undef CM

	fConduitName = i18n("Device");
}

void DeviceConfigPage::load()
{
	FUNCTIONSETUP;
	KPilotSettings::self()->readConfig();

	/* General tab in the setup dialog */
	fConfigWidget.fPilotDevice->setText(KPilotSettings::pilotDevice());
	fConfigWidget.fPilotSpeed->setCurrentIndex(KPilotSettings::pilotSpeed());
	getEncoding();
	fConfigWidget.fUserName->setText(KPilotSettings::userName());

	switch(KPilotSettings::workarounds())
	{
	case KPilotSettings::eWorkaroundNone :
		fConfigWidget.fWorkaround->setCurrentIndex(0);
		break;
	case KPilotSettings::eWorkaroundUSB :
		fConfigWidget.fWorkaround->setCurrentIndex(1);
		break;
	default:
		WARNINGKPILOT << "Unknown workaround number "
			<< (int) KPilotSettings::workarounds();
		KPilotSettings::setWorkarounds(KPilotSettings::eWorkaroundNone);
		fConfigWidget.fWorkaround->setCurrentIndex(0);
	}
	unmodified();
}

/* virtual */ void DeviceConfigPage::commit()
{
	FUNCTIONSETUP;

	// General page
	KPilotSettings::setPilotDevice(fConfigWidget.fPilotDevice->text());
	KPilotSettings::setPilotSpeed(fConfigWidget.fPilotSpeed->currentIndex());
	setEncoding();
	KPilotSettings::setUserName(fConfigWidget.fUserName->text());

	switch(fConfigWidget.fWorkaround->currentIndex())
	{
	case 0 :
		KPilotSettings::setWorkarounds(KPilotSettings::eWorkaroundNone);
		break;
	case 1 :
		KPilotSettings::setWorkarounds(KPilotSettings::eWorkaroundUSB);
		break;
	default :
		WARNINGKPILOT << "Unknown workaround number "
			<< fConfigWidget.fWorkaround->currentIndex();
		KPilotSettings::setWorkarounds(KPilotSettings::eWorkaroundNone);
	}
	KPilotConfig::updateConfigVersion();
	KPilotSettings::self()->writeConfig();
	unmodified();
}

/* slot */ void DeviceConfigPage::changePortType(int i)
{
	FUNCTIONSETUP;

	switch (i)
	{
	case 0:
		fConfigWidget.fPilotSpeed->setEnabled(true);
		break;
	case 1:
	case 2:
		fConfigWidget.fPilotSpeed->setEnabled(false);
		break;
	default:
		WARNINGKPILOT << "Unknown port type" << i;
	}
}

void DeviceConfigPage::getEncoding()
{
	FUNCTIONSETUP;
	QString e = KPilotSettings::encoding();
	if (e.isEmpty())
	{
		fConfigWidget.fPilotEncoding->setEditText(CSL1("ISO8859-15"));
	}
	else
	{
		fConfigWidget.fPilotEncoding->setEditText(e);
	}
}

void DeviceConfigPage::setEncoding()
{
	FUNCTIONSETUP;

	QString enc = fConfigWidget.fPilotEncoding->currentText();
	if (enc.isEmpty())
	{
		WARNINGKPILOT << "Empty encoding. Will ignore it.";
	}
	else
	{
		KPilotSettings::setEncoding(enc);
	}
}


void DeviceConfigPage::autoDetectDevice()
{
	FUNCTIONSETUP;
	ProbeDialog *d = new ProbeDialog( fWidget );
	d->show();
	d->exec();
	if (d->detected())
	{
		fConfigWidget.fUserName->setText( d->userName() );
		fConfigWidget.fPilotDevice->setText( d->device() );
	}
}


SyncConfigPage::SyncConfigPage(QWidget * w, QVariantList &args)
	: ConfigPage( w, args )
{
	FUNCTIONSETUP;

	fConfigWidget = new SyncConfigWidget( w );
	fConfigWidget->resize(fConfigWidget->size());
	fWidget = fConfigWidget;

#define CM(a,b) connect(fConfigWidget->a,b,this,SLOT(modified()));
	CM(fSpecialSync, SIGNAL(activated(int)));
	CM(fFullSyncCheck, SIGNAL(toggled(bool)));
	CM(fScreenlockSecure, SIGNAL(toggled(bool)));
	CM(fConflictResolution, SIGNAL(activated(int)));
#undef CM

	fConduitName = i18n("HotSync");
}

#define MENU_ITEM_COUNT (4)
static SyncAction::SyncMode::Mode syncTypeMap[MENU_ITEM_COUNT] = {
	SyncAction::SyncMode::eHotSync,
	SyncAction::SyncMode::eFullSync,
	SyncAction::SyncMode::eCopyPCToHH,
	SyncAction::SyncMode::eCopyHHToPC
	} ;

void SyncConfigPage::load()
{
	FUNCTIONSETUP;
	KPilotSettings::self()->readConfig();

	/* Sync tab */
	int synctype=KPilotSettings::syncType();
	if (synctype<0) synctype=(int) SyncAction::SyncMode::eHotSync;
	for (unsigned int i=0; i<MENU_ITEM_COUNT; ++i)
	{
		if (syncTypeMap[i] == synctype)
		{
			fConfigWidget->fSpecialSync->setCurrentIndex(i);
			synctype=-1;
			break;
		}
	}
	if (synctype != -1)
	{
		fConfigWidget->fSpecialSync->setCurrentIndex(0); /* HotSync */
	}

	fConfigWidget->fFullSyncCheck->setChecked(KPilotSettings::fullSyncOnPCChange());
	fConfigWidget->fConflictResolution->setCurrentIndex(KPilotSettings::conflictResolution());
	fConfigWidget->fScreenlockSecure->setChecked(KPilotSettings::screenlockSecure());

	unmodified();
}

/* virtual */ void SyncConfigPage::commit()
{
	FUNCTIONSETUP;

	/* Sync tab */
	int synctype = -1;
	unsigned int selectedsync = fConfigWidget->fSpecialSync->currentIndex();
	if (selectedsync < MENU_ITEM_COUNT)
	{
		synctype = syncTypeMap[selectedsync];
	}
	if (synctype < 0)
	{
		synctype = SyncAction::SyncMode::eHotSync;
	}

	KPilotSettings::setSyncType(synctype);
	KPilotSettings::setFullSyncOnPCChange(fConfigWidget->fFullSyncCheck->isChecked());
	KPilotSettings::setConflictResolution(fConfigWidget->fConflictResolution->currentIndex());
	KPilotSettings::setScreenlockSecure(fConfigWidget->fScreenlockSecure->isChecked());

	KPilotConfig::updateConfigVersion();
	KPilotSettings::self()->writeConfig();
	unmodified();
}


BackupConfigPage::BackupConfigPage(QWidget * w, QVariantList &args )
	: ConfigPage( w, args )
{
	FUNCTIONSETUP;

	fWidget = new QWidget(w);
	fConfigWidget.setupUi(fWidget);

	connect(fConfigWidget.fBackupOnlyChooser, SIGNAL( clicked() ),
		SLOT( slotSelectNoBackupDBs() ) );
	connect(fConfigWidget.fSkipDBChooser, SIGNAL(clicked()),
		SLOT(slotSelectNoRestoreDBs()));

#define CM(a,b) connect(fConfigWidget.a,b,this,SLOT(modified()));
	CM(fBackupOnly, SIGNAL(textChanged(const QString &)));
	CM(fSkipDB, SIGNAL(textChanged(const QString &)));
	CM(fBackupFrequency, SIGNAL(activated(int)));
#undef CM

	fConduitName = i18n("Backup");
}

void BackupConfigPage::load()
{
	FUNCTIONSETUP;
	KPilotSettings::self()->readConfig();

	/* Backup tab */
	fConfigWidget.fBackupOnly->setText(KPilotSettings::skipBackupDB().join(CSL1(",")));
	fConfigWidget.fSkipDB->setText(KPilotSettings::skipRestoreDB().join(CSL1(",")));
	fConfigWidget.fRunConduitsWithBackup->setChecked(
		KPilotSettings::runConduitsWithBackup());

	int backupfreq = KPilotSettings::backupFrequency();

	fConfigWidget.fBackupFrequency->setCurrentIndex(backupfreq);

	unmodified();
}

/* virtual */ void BackupConfigPage::commit()
{
	FUNCTIONSETUP;

	/* Backup tab */
	KPilotSettings::setSkipBackupDB(
		fConfigWidget.fBackupOnly->text().split( ',' ) );
	KPilotSettings::setSkipRestoreDB(
		fConfigWidget.fSkipDB->text().split( ',' ) );
	KPilotSettings::setRunConduitsWithBackup(
		fConfigWidget.fRunConduitsWithBackup->isChecked());
	KPilotSettings::setBackupFrequency(
		fConfigWidget.fBackupFrequency->currentIndex());

	KPilotConfig::updateConfigVersion();
	KPilotSettings::self()->writeConfig();
	unmodified();
}

void BackupConfigPage::slotSelectNoBackupDBs()
{
	FUNCTIONSETUP;

	QStringList selectedDBs(fConfigWidget.fBackupOnly->text().split(','));

	QStringList deviceDBs = KPilotSettings::deviceDBs();
	QStringList addedDBs = KPilotSettings::addedDBs();
	KPilotDBSelectionDialog *dlg = new KPilotDBSelectionDialog(selectedDBs
		, deviceDBs, addedDBs, 0, "NoBackupDBs");
	if (dlg && (dlg->exec() == QDialog::Accepted) )
	{
		fConfigWidget.fBackupOnly->setText(
			dlg->getSelectedDBs().join(CSL1(",")));
		KPilotSettings::setAddedDBs( dlg->getAddedDBs() );
	}
	KPILOT_DELETE(dlg);
}

void BackupConfigPage::slotSelectNoRestoreDBs()
{
	FUNCTIONSETUP;

	QStringList selectedDBs(fConfigWidget.fSkipDB->text().split(','));

	QStringList deviceDBs = KPilotSettings::deviceDBs();
	QStringList addedDBs = KPilotSettings::addedDBs();
	KPilotDBSelectionDialog *dlg = new KPilotDBSelectionDialog(selectedDBs
		, deviceDBs, addedDBs, 0, "NoRestoreDBs");
	
	if (dlg && (dlg->exec() == QDialog::Accepted) )
	{
		fConfigWidget.fSkipDB->setText(
			dlg->getSelectedDBs().join(CSL1(",")));
		KPilotSettings::setAddedDBs( dlg->getAddedDBs() );
	}
	KPILOT_DELETE(dlg);
}









StartExitConfigPage::StartExitConfigPage(QWidget * w, QVariantList &args )
	: ConfigPage( w, args )
{
	FUNCTIONSETUP;

	fWidget = new QWidget(w);
	fConfigWidget.setupUi(fWidget);

#define CM(a,b) connect(fConfigWidget.a,b,this,SLOT(modified()));
	CM(fStartDaemonAtLogin, SIGNAL(toggled(bool)));
	CM(fKillDaemonOnExit, SIGNAL(toggled(bool)));
	CM(fDockDaemon, SIGNAL(toggled(bool)));
	CM(fQuitAfterSync, SIGNAL(toggled(bool)));
#undef CM

	fConduitName = i18n("Startup and Exit");
}

void StartExitConfigPage::load()
{
	FUNCTIONSETUP;
	KPilotSettings::self()->readConfig();

	fConfigWidget.fStartDaemonAtLogin->setChecked(KPilotSettings::startDaemonAtLogin());
	fConfigWidget.fDockDaemon->setChecked(KPilotSettings::dockDaemon());
	fConfigWidget.fKillDaemonOnExit->setChecked(KPilotSettings::killDaemonAtExit());
	fConfigWidget.fQuitAfterSync->setChecked(KPilotSettings::quitAfterSync());
	unmodified();
}


/* virtual */ void StartExitConfigPage::commit()
{
	FUNCTIONSETUP;

	KPilotSettings::setStartDaemonAtLogin(fConfigWidget.fStartDaemonAtLogin->isChecked());
	KAutostart autostart( CSL1("KPilotDaemon") );
	autostart.setAutostarts( KPilotSettings::startDaemonAtLogin() );
	autostart.setStartPhase( KAutostart::Applications );

	KPilotSettings::setDockDaemon(fConfigWidget.fDockDaemon->isChecked());
	KPilotSettings::setKillDaemonAtExit(fConfigWidget.fKillDaemonOnExit->isChecked());
	KPilotSettings::setQuitAfterSync(fConfigWidget.fQuitAfterSync->isChecked());
	KPilotConfig::updateConfigVersion();
	KPilotSettings::self()->writeConfig();
	unmodified();
}

