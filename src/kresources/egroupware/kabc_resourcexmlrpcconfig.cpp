/*
    This file is part of kdepim.
    Copyright (c) 2002 - 2004 Tobias Koenig <tokoe@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QLabel>
#include <QLayout>
//Added by qt3to4:
#include <QGridLayout>

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <klineedit.h>
#include <kurlrequester.h>

#include "kabc_egroupwareprefs.h"
#include "kabc_resourcexmlrpc.h"
#include "kabc_resourcexmlrpcconfig.h"

using namespace KABC;

ResourceXMLRPCConfig::ResourceXMLRPCConfig( QWidget* parent )
  : KRES::ConfigWidget( parent )
{
  QGridLayout *mainLayout = new QGridLayout( this );
  mainLayout->setSpacing( KDialog::spacingHint() );
  mainLayout->setMargin( 0 );

  QLabel *label = new QLabel( i18n( "URL:" ), this );
  mURL = new KUrlRequester( this );

  mainLayout->addWidget( label, 0, 0 );
  mainLayout->addWidget( mURL, 0, 1 );

  label = new QLabel( i18n( "Domain:" ), this );
  mDomain = new KLineEdit( this );

  mainLayout->addWidget( label, 1, 0 );
  mainLayout->addWidget( mDomain, 1, 1 );

  label = new QLabel( i18n( "User:" ), this );
  mUser = new KLineEdit( this );

  mainLayout->addWidget( label, 2, 0 );
  mainLayout->addWidget( mUser, 2, 1 );

  label = new QLabel( i18n( "Password:" ), this );
  mPassword = new KLineEdit( this );
  mPassword->setEchoMode( QLineEdit::Password );

  mainLayout->addWidget( label, 3, 0 );
  mainLayout->addWidget( mPassword, 3, 1 );
}

void ResourceXMLRPCConfig::loadSettings( KRES::Resource *res )
{
  ResourceXMLRPC *resource = dynamic_cast<ResourceXMLRPC*>( res );

  if ( !resource ) {
    kDebug(5700) <<"ResourceXMLRPCConfig::loadSettings(): cast failed";
    return;
  }

  mURL->setUrl( resource->prefs()->url() );
  mDomain->setText( resource->prefs()->domain() );
  mUser->setText( resource->prefs()->user() );
  mPassword->setText( resource->prefs()->password() );
}

void ResourceXMLRPCConfig::saveSettings( KRES::Resource *res )
{
  ResourceXMLRPC *resource = dynamic_cast<ResourceXMLRPC*>( res );

  if ( !resource ) {
    kDebug(5700) <<"ResourceXMLRPCConfig::saveSettings(): cast failed";
    return;
  }

  resource->prefs()->setUrl( mURL->url().url() );
  resource->prefs()->setDomain( mDomain->text() );
  resource->prefs()->setUser( mUser->text() );
  resource->prefs()->setPassword( mPassword->text() );
}

#include "kabc_resourcexmlrpcconfig.moc"
