#!/bin/sh

# Copyright (C) 2015 Stephan Platz <trojita@paalsteek.de>
#
# This file is part of the Trojita Qt IMAP e-mail client,
# http://trojita.flaska.net/
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

set -e

SRCDIR=$1

if [[ ! -f ${SRCDIR}/CMakeLists.txt ]] ; then
    echo "Usage: ${0} path/to/trojita/sources"
    exit 1
fi

KEYDIR="./keys"

# params: receiver, plaintext
# output: quoted encrypted messages
_encrypt() {
	ENC=$(echo "$2" | gpg -e --armor -r $1)
	# quote each line and add newlines
	echo "${ENC}" | sed -e 's/^\(.*\)$/\t"\1\\r\\n"/g'
}

[ -d "$KEYDIR" ] && rm -r "$KEYDIR"
mkdir -m700 "$KEYDIR"

export GNUPGHOME="$KEYDIR"

# generate keys
LD_PRELOAD=./libfake-dev-random.so gpg --batch --quiet --gen-key ${SRCDIR}/tests/Cryptography/batch-keygen

HEAD="// This file is autogenerated by tests/Cryptography/keygen.sh
// Do not edit manually.

#ifndef CRYPTOGRAPHY_DATA_H
#define CRYPTOGRAPHY_DATA_H"
TAIL="#endif // CRYPTOGRAPHY_DATA_H"

# generate messages
# simple valid message encrypted to valid@
VALID=$(_encrypt valid@test.trojita.flaska.net plaintext)

# for an invalid message replace some characters. one of vwxyz should occur in every case
INVALID=$(echo "$VALID" | tr vwxyz lmnop)

# for an expired message we use a key that will be expired after the creation of the messages
EXPIRED=$(_encrypt expired@test.trojita.flaska.net plaintext)

# for a message with missing key we use a key that will be deleted from the keyring after message generation
UNKNOWN=$(_encrypt unknown@test.trojita.flaska.net plaintext)

echo "$HEAD

const QByteArray encValid(
$VALID
);

const QByteArray encInvalid(
$INVALID
);

const QByteArray encExpired(
$EXPIRED
);

const QByteArray encUnknown(
$UNKNOWN
);

$TAIL" > crypto_test_data.h

# expire key
echo -n "key *\nexpire\nseconds=1\nsave\n" | gpg --no-tty --quiet --command-fd 0 --edit-key "expired@test.trojita.flaska.net"

# extract fingerprint of key to be deleted
FINGERPRINT="$(gpg --quiet --no-tty --list-keys --with-colons --fingerprint unknown@test.trojita.flaska.net | grep fpr | cut -d : -f 10)"

# delete key
yes | DISPLAY="" gpg --quiet --batch --no-tty --delete-secret-and-public-key --yes --command-fd 0 --passphrase-fd 0 "${FINGERPRINT}"

#TODO: cleanup: we only want the secring and the keyring in test_keys
if [[ -e "$KEYDIR"/S.gpg-agent ]]; then
	rm "$KEYDIR"/S.gpg-agent
fi