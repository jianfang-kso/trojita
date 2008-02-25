/* Copyright (C) 2007 Jan Kundrát <jkt@gentoo.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef TEST_IMAP_PARSER_PARSE
#define TEST_IMAP_PARSER_PARSE

#include <QtCore/QObject>
#include "Imap/LowLevelParser.h"


/** @short Unit tests for Imap::Responses::AbstractResponse and friends */
class ImapLowLevelParserTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testParseList();
    void testGetString();
    void testGetAString();
    void testGetUInt();
};

#endif
