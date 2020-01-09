/* KPilot
**
** Copyright (C) 1998-2001 by Dan Pilone <dan@kpilot.org>
**
** This is a class that does "the work" of adding and deleting
** files in the pending_install directory of KPilot. It is used
** by the fileInstallWidget and by the daemon's drag-and-drop
** file accepter.
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

#include <QtCore/QDir>

//#include <q3strlist.h>

#include <kglobal.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include "fileInstaller.moc"

FileInstaller::FileInstaller() :
	enabled(true)
{
	FUNCTIONSETUP;

	fDirName = KGlobal::dirs()->saveLocation("data",
		CSL1("kpilot/pending_install/"));
	fPendingCopies = 0;

}

/* virtual */ FileInstaller::~FileInstaller()
{
	FUNCTIONSETUP;
}


void FileInstaller::clearPending()
{
	FUNCTIONSETUP;

	unsigned int i;

	QDir installDir(fDirName);

	// Start from 2 to skip . and ..
	//
	for (i = 2; i < installDir.count(); ++i)
	{
		QFile::remove(fDirName + installDir[i]);
	}

	if (i > 2)
	{
		emit filesChanged();
	}
}

void FileInstaller::deleteFile(const QString &file)
{
    QFile::remove(fDirName + file);
    emit filesChanged();
}

void FileInstaller::deleteFiles(const QStringList &files)
{
    if(files.empty())
        return;

    for(QStringList::ConstIterator it = files.begin(); it != files.end(); ++it)
        QFile::remove(fDirName + *it);
    
    emit filesChanged();
}

/* virtual */ bool FileInstaller::runCopy(const QString &s, QWidget *w )
{
	FUNCTIONSETUP;

	if(!(s.endsWith(CSL1(".pdb"), Qt::CaseInsensitive)
		|| s.endsWith(CSL1(".prc"), Qt::CaseInsensitive))) 
	{
		KMessageBox::detailedSorry(w, i18n("Cannot install %1",s),
			i18n("Only PalmOS database files (like *.pdb and *.prc) can be installed by the file installer."));
		return false;
	}

	DEBUGKPILOT << "Copying" << s;

	KUrl src;
	KUrl dest;
	src.setPath(s);
	dest.setPath(fDirName + CSL1("/") + src.fileName());

	// Permissions -1, overwrite, no resume
	KIO::Job *my_job = KIO::file_copy(src,dest, -1, KIO::Overwrite);
	return KIO::NetAccess::synchronousRun(my_job, w);
}


void FileInstaller::addFiles(const QStringList & fileList, QWidget* w)
{
	FUNCTIONSETUP;

	if (!enabled) return;

	unsigned int succ = 0;

	for(QStringList::ConstIterator it = fileList.begin();
	    it != fileList.end(); ++it)
	{
		if (runCopy( *it, w ))
			succ++;
	}

	if (succ)
	{
		emit filesChanged();
	}
}

void FileInstaller::addFile( const QString & file, QWidget* w )
{
	FUNCTIONSETUP;

	if (!enabled) return;

	if (runCopy(file, w))
	{
		emit(filesChanged());
	}
}

/* slot */ void FileInstaller::copyCompleted()
{
	FUNCTIONSETUP;
}

const QStringList FileInstaller::fileNames() const
{
	FUNCTIONSETUP;

	QDir installDir(fDirName);

	return installDir.entryList(QDir::Files |
		QDir::NoSymLinks | QDir::Readable);
}

/* slot */ void FileInstaller::setEnabled(bool b)
{
	FUNCTIONSETUP;
	enabled=b;
}


