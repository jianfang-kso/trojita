/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QTest>

#include "test_Imap_Message.h"
#include "Imap/Encoders.h"

Q_DECLARE_METATYPE(Imap::Message::MailAddress)
Q_DECLARE_METATYPE(QVariantList)

    /*
void ImapMessageTest::testEnvelope()
{
    QFETCH( Imap::Message::Envelope, expected );
    QFETCH( QVariantList, data );
    Imap::Message::Envelope envelope;
    try {
        envelope = Imap::Message::Envelope::fromList( data, QByteArray(), 0 );
        QCOMPARE( expected, Imap::Message::Envelope::fromList( data, QByteArray(), 0 ) );
    } catch ( Imap::Exception& e ) {
        QFAIL( e.what() );
    }
}

void ImapMessageTest::testEnvelope_data()
{
    using namespace Imap::Message;

    QTest::addColumn<Envelope>("expected");
    QTest::addColumn<QVariantList>("data");

    QList<MailAddress> from, sender, to, cc, bcc, replyTo;

    from << MailAddress( "Jan Kundrat", QByteArray(), "flaska.net", "jkt" ) <<
        MailAddress( "Another Developer", QByteArray(), "example.org", "another.guy" );
    sender << MailAddress( "Jan Kundrat", QByteArray(), "flaska.net", "jkt" );
    to << MailAddress( QByteArray(), QByteArray(), "lists.flaska.net", "trojita-announce" );
    bcc << MailAddress( QByteArray(), QByteArray(), "example.org", "trash" ) <<
        MailAddress( QByteArray(), QByteArray(), "example.org", "spam" );

    QTest::newRow("foo") <<
        Envelope( QDateTime( QDate( 2008, 4, 27 ), QTime( 13, 41, 37 ), Qt::UTC ),
                "A random test subject", from, sender, replyTo, to, cc, bcc,
                QByteArray(), QByteArray() ) <<
        QVariantList() <<
            QByteArray("Sun, 27 Apr 2008 15:41:37 +0200 (CEST)") << 
            QByteArray("A random test subject") <<
            ( // from
             QVariantList() <<
             ( QVariantList() << QByteArray("Jan Kundrat") << QByteArray() << QByteArray("flaska.net") << QByteArray("jkt") ) <<
             ( QVariantList() << QByteArray("Another Developer") ) ) <<
            ( QVariantList() << ( QVariantList() << QByteArray("Jan Kundrat") << QByteArray() << QByteArray("flaska.net") << QByteArray("jkt") ) ) <<
            ( QVariantList() << ( QVariantList() << QByteArray("Jan Kundrat") << QByteArray() << QByteArray("flaska.net") << QByteArray("jkt") ) ) <<
            ( QVariantList() << ( QVariantList() << QByteArray("Jan Kundrat") << QByteArray() << QByteArray("flaska.net") << QByteArray("jkt") ) ) <<
            ( QVariantList() << ( QVariantList() << QByteArray("Jan Kundrat") << QByteArray() << QByteArray("flaska.net") << QByteArray("jkt") ) );
}*/

void ImapMessageTest::testMailAddresEq()
{
    using namespace Imap::Message;

    QCOMPARE( MailAddress(), MailAddress() );
    QCOMPARE( MailAddress( "name", "adl", "mailbox", "host" ),
              MailAddress( QVariantList() <<
                  QByteArray("name") << QByteArray("adl") << QByteArray("mailbox") << QByteArray("host"),
                  QByteArray(), 0 ) );
}

void ImapMessageTest::testMailAddresNe()
{
    QFETCH( Imap::Message::MailAddress, expected );
    QFETCH( QVariantList, data );
    QVERIFY2( expected != Imap::Message::MailAddress( data, QByteArray(), 0 ), "addresses equal" );
}

void ImapMessageTest::testMailAddresNe_data()
{
    using namespace Imap::Message;

    QTest::addColumn<MailAddress>("expected");
    QTest::addColumn<QVariantList>("data");

    QTest::newRow( "difference-name" ) <<
        MailAddress( QStringLiteral("name"), QStringLiteral("adl"), QStringLiteral("mailbox"), QStringLiteral("host") ) <<
        ( QVariantList() << QByteArray("nAme") << QByteArray("adl") << QByteArray("mailbox") << QByteArray("host") );

    QTest::newRow( "difference-adl" ) <<
        MailAddress( QStringLiteral("name"), QStringLiteral("adl"), QStringLiteral("mailbox"), QStringLiteral("host") ) <<
        ( QVariantList() << QByteArray("name") << QByteArray("aDl") << QByteArray("mailbox") << QByteArray("host") );

    QTest::newRow( "difference-mailbox" ) <<
        MailAddress( QStringLiteral("name"), QStringLiteral("adl"), QStringLiteral("mailbox"), QStringLiteral("host") ) <<
        ( QVariantList() << QByteArray("name") << QByteArray("adl") << QByteArray("maIlbox") << QByteArray("host") );

    QTest::newRow( "difference-host" ) <<
        MailAddress( QStringLiteral("name"), QStringLiteral("adl"), QStringLiteral("mailbox"), QStringLiteral("host") ) <<
        ( QVariantList() << QByteArray("name") << QByteArray("adl") << QByteArray("mailbox") << QByteArray("h0st") );

}

void ImapMessageTest::testMailAddressFormat()
{
    QFETCH( Imap::Message::MailAddress, addr );
    QFETCH( QString, pretty );
    QFETCH( QByteArray, addrspec );
    QFETCH( bool, should2047 );

    QCOMPARE( addr.prettyName(Imap::Message::MailAddress::FORMAT_READABLE), pretty );
    QCOMPARE( addr.asSMTPMailbox(), addrspec );
    
    QByteArray full = addr.asMailHeader();
    QByteArray bracketed;
    bracketed.append(" <").append(addrspec).append(">");
    QVERIFY( full.endsWith(bracketed) );
    full.remove(full.size() - bracketed.size(), bracketed.size());
    
    if (should2047) {
        QVERIFY( full.startsWith("=?") );
        QVERIFY( full.endsWith("?=") );
        QCOMPARE( addr.name, Imap::decodeRFC2047String(full) );
    } else {
        QVERIFY( !full.contains("=?") );
        QVERIFY( !full.contains("?=") );
    }
}

void ImapMessageTest::testMailAddressFormat_data()
{
    using namespace Imap::Message;

    QTest::addColumn<MailAddress>("addr");
    QTest::addColumn<QString>("pretty");
    QTest::addColumn<QByteArray>("addrspec");
    QTest::addColumn<bool>("should2047");

    QTest::newRow("simple") <<
        MailAddress( QStringLiteral("name"), QStringLiteral("adl"), QStringLiteral("mailbox"), QStringLiteral("host") ) <<
        QStringLiteral("name <mailbox@host>") <<
        QByteArray("mailbox@host") << false;

    QTest::newRow("domain-literal") <<
        MailAddress( QStringLiteral("words name"), QStringLiteral("adl"), QStringLiteral("us.er"), QStringLiteral("[127.0.0.1]") ) <<
        QStringLiteral("words name <us.er@[127.0.0.1]>") <<
        QByteArray("us.er@[127.0.0.1]") << false;

    QTest::newRow("idn") <<
        MailAddress( QStringLiteral("words j. name"), QStringLiteral("adl"), QStringLiteral("us.er"),
                     QString::fromUtf8("trojit\xC3\xA1.example.com") ) <<
        QString::fromUtf8("words j. name <us.er@trojit\xC3\xA1.example.com>") <<
        QByteArray("us.er@xn--trojit-uta.example.com") << false;

    /* overspecific test: we want to test here that the combining mark
       is normalized before being converted to punycode for the
       IDN. We don't actually care whether it's normalized for
       PRETTYNAME, but we have to give a value for the test. */
    QTest::newRow("idn+normalize") <<
        MailAddress( QStringLiteral("words j. name"), QStringLiteral("adl"), QStringLiteral("us.er"),
                     QString::fromUtf8("trojita\xCC\x81.example.com") ) <<
        QString::fromUtf8("words j. name <us.er@trojita\xCC\x81.example.com>") <<
        QByteArray("us.er@xn--trojit-uta.example.com") << false;

    QTest::newRow("odd-mailbox") <<
        MailAddress( QStringLiteral("words (q) name"), QStringLiteral("adl"), QStringLiteral("us er"), QStringLiteral("example.com") ) <<
        QStringLiteral("words (q) name <us er@example.com>") <<
        QByteArray("\"us er\"@example.com") << false;

    QTest::newRow("intl-realname") <<
        MailAddress( QStringLiteral("words \xE2\x98\xBA name"),
                     QStringLiteral("adl"), QStringLiteral("*"), QStringLiteral("example.com") ) <<
        QStringLiteral("words \xE2\x98\xBA name <*@example.com>") <<
        QByteArray("*@example.com") << true;

    QTest::newRow("intl-with-composed-mailbox") <<
        MailAddress(QStringLiteral("Jan Kundrát"), QLatin1String(""), QStringLiteral("jan.kundrat"), QStringLiteral("demo.isode.com")) <<
        QStringLiteral("Jan Kundrát <jan.kundrat@demo.isode.com>") <<
        QByteArray("jan.kundrat@demo.isode.com") << true;
}

void ImapMessageTest::testMailAddressParsing()
{
    QFETCH(QString, textInput);
    QFETCH(Imap::Message::MailAddress, expected);

    Imap::Message::MailAddress actual;
    QVERIFY(Imap::Message::MailAddress::fromPrettyString(actual, textInput));
    QCOMPARE(actual, expected);

    Imap::Message::MailAddress afterUrlTransformation;
    QVERIFY(Imap::Message::MailAddress::fromUrl(afterUrlTransformation, actual.asUrl(), QStringLiteral("mailto")));
    QCOMPARE(afterUrlTransformation, expected);
}

void ImapMessageTest::testMailAddressParsing_data()
{
    using namespace Imap::Message;

    QTest::addColumn<QString>("textInput");
    QTest::addColumn<MailAddress>("expected");

    QTest::newRow("trojita-ml") <<
        QStringLiteral("trojita@lists.flaska.net") <<
        MailAddress(QString(), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    QTest::newRow("trojita-ml-with-folding-white-spaces") <<
        QStringLiteral("   trojita@lists.flaska.net   ") <<
        MailAddress(QString(), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    QTest::newRow("trojita-ml-with-short-ascii-name") <<
        QStringLiteral("Trojita <trojita@lists.flaska.net>") <<
        MailAddress(QStringLiteral("Trojita"), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    /*QTest::newRow("trojita-ml-with-short-ascii-name-quoted") <<
        QString::fromUtf8("\"Trojita\" <trojita@lists.flaska.net>") <<
        MailAddress(QString::fromUtf8("Trojita"), QString(), "trojita", "lists.flaska.net");*/

    QTest::newRow("trojita-ml-with-ascii-name") <<
        QStringLiteral("Trojita ML <trojita@lists.flaska.net>") <<
        MailAddress(QStringLiteral("Trojita ML"), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    /*QTest::newRow("trojita-ml-with-ascii-name-quoted") <<
        QString::fromUtf8("\"Trojita ML\" <trojita@lists.flaska.net>") <<
        MailAddress(QString::fromUtf8("Trojita ML"), QString(), "trojita", "lists.flaska.net");*/

    QTest::newRow("trojita-ml-with-short-unicode-name") <<
        QStringLiteral("Trojitá <trojita@lists.flaska.net>") <<
        MailAddress(QStringLiteral("Trojitá"), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    /*QTest::newRow("trojita-ml-with-short-unicode-name-quoted") <<
        QString::fromUtf8("\"Trojitá\" <trojita@lists.flaska.net>") <<
        MailAddress(QString::fromUtf8("Trojitá"), QString(), "trojita", "lists.flaska.net");*/

    QTest::newRow("trojita-ml-with-unicode-name") <<
        QStringLiteral("Trojitá ML <trojita@lists.flaska.net>") <<
        MailAddress(QStringLiteral("Trojitá ML"), QString(), QStringLiteral("trojita"), QStringLiteral("lists.flaska.net"));

    /*QTest::newRow("trojita-ml-with-unicode-name-quoted") <<
        QString::fromUtf8("\"Trojitá ML\" <trojita@lists.flaska.net>") <<
        MailAddress(QString::fromUtf8("Trojitá ML"), QString(), "trojita", "lists.flaska.net");*/

    QTest::newRow("jkt-isode-ascii") <<
        QStringLiteral("Jan Kundrat <jan.kundrat@demo.isode.com>") <<
        MailAddress(QStringLiteral("Jan Kundrat"), QString(), QStringLiteral("jan.kundrat"), QStringLiteral("demo.isode.com"));

    /*QTest::newRow("jkt-isode-ascii-quoted") <<
        QString::fromUtf8("\"Jan Kundrat\" <jan.kundrat@demo.isode.com>") <<
        MailAddress(QString::fromUtf8("Jan Kundrat"), QString(), "jan.kundrat", "demo.isode.com");*/

    QTest::newRow("jkt-isode-unicode") <<
        QStringLiteral("Jan Kundrát <jan.kundrat@demo.isode.com>") <<
        MailAddress(QStringLiteral("Jan Kundrát"), QString(), QStringLiteral("jan.kundrat"), QStringLiteral("demo.isode.com"));

    /*QTest::newRow("jkt-isode-unicode-quoted") <<
        QString::fromUtf8("\"Jan Kundrát\" <jan.kundrat@demo.isode.com>") <<
        MailAddress(QString::fromUtf8("Jan Kundrát"), QString(), "jan.kundrat", "demo.isode.com");*/

    QTest::newRow("long-address-with-fancy-symbols") <<
        QStringLiteral("Some Fünny Äddre¶ <this-is_a.test+some-thin_g.yay@foo-blah.d_o-t.example.org>") <<
        MailAddress(QStringLiteral("Some Fünny Äddre¶"), QString(), QStringLiteral("this-is_a.test+some-thin_g.yay"), QStringLiteral("foo-blah.d_o-t.example.org"));

    QTest::newRow("address-with-at-symbol-in-display-name") <<
        QStringLiteral("john.doe@acme.org via RT <ticketing@example.org>") <<
        MailAddress(QStringLiteral("john.doe@acme.org via RT"), QString(), QStringLiteral("ticketing"), QStringLiteral("example.org"));

    QTest::newRow("address-with-folding-white-spaces") <<
        QStringLiteral("John Doe   <johnd@example.org>   ") <<
        MailAddress(QStringLiteral("John Doe"), QString(), QStringLiteral("johnd"), QStringLiteral("example.org"));

    QTest::newRow("long-address-with-fancy-symbols-no-human-name") <<
        QStringLiteral("this-is_a.test+some-thin_g.yay@foo-blah.d_o-t.example.org") <<
        MailAddress(QString(), QString(), QStringLiteral("this-is_a.test+some-thin_g.yay"), QStringLiteral("foo-blah.d_o-t.example.org"));
}

void ImapMessageTest::testInvalidMailAddressParsing()
{
    QFETCH(QString, textInput);

    Imap::Message::MailAddress actual;
    QVERIFY(!Imap::Message::MailAddress::fromPrettyString(actual, textInput));
}

void ImapMessageTest::testInvalidMailAddressParsing_data()
{
    using namespace Imap::Message;

    QTest::addColumn<QString>("textInput");

    QTest::newRow("mistyped-address-1") << QStringLiteral("john.doe@@example.org");
    QTest::newRow("mistyped-address-2") << QStringLiteral("john.doe@example@org");
    QTest::newRow("mistyped-address-3") << QStringLiteral("John Doe <john.doe@example>.org");
}

void ImapMessageTest::testMessage()
{
}

void ImapMessageTest::testMessage_data()
{
}


QTEST_GUILESS_MAIN( ImapMessageTest )

namespace QTest {

template<> char * toString( const Imap::Message::Envelope& resp )
{
    QByteArray buf;
    QTextStream stream( &buf );
    stream << resp;
    stream.flush();
    return qstrdup( buf.data() );
}

template<> char * toString( const Imap::Message::MailAddress& resp )
{
    QByteArray buf;
    QTextStream stream( &buf );
    stream << resp;
    stream.flush();
    return qstrdup( buf.data() );
}

}
