/* converter.cpp
**
** Copyright (C) 2003 by Reinhold Kainhofer <reinhold@kainhofer.com>
**
** This is the main program of the KDE PalmDOC converter.
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

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

#include "kpalmdoc_dlg.h"



int main(int argc, char *argv[])
{

	KAboutData about("converter", 0, ki18n("KPalmDOC"), "-0.0.1",
		ki18n("KPalmDOC - KDE Converter for PalmDOC texts.\n\n"),
		KAboutData::License_GPL, ki18n("(c) 2003, Reinhold Kainhofer"));
	about.addAuthor(ki18n("Reinhold Kainhofer"), ki18n("Main Developer"),
		"reinhold@kainhofer.com", "http://reinhold.kainhofer.com/Linux/");
	about.addCredit(ki18n("Adriaan de Groot"), ki18n("Maintainer of KPilot"),
		"groot@kde.org", "http://www.kpilot.org/");

	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addStdCmdLineOptions();

	KApplication app;
	ConverterDlg *dlg=new ConverterDlg(0L, i18n("PalmDOC Converter"));
	dlg->show();
	return app.exec();
}

