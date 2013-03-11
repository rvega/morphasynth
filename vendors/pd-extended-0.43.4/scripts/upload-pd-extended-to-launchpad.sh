#!/bin/bash

set -x

DOWNLOAD_SERVER=apt.puredata.info
VERSION=0.43.4
DEBIAN_SOURCE_DATE=extended
URL_DATE=`date +%Y-%m-%d`
ORIG=Pd-extended_${VERSION}~$DEBIAN_SOURCE_DATE-source.tar.bz2
DEBIAN=Pd-extended_${VERSION}~$DEBIAN_SOURCE_DATE-source.debian.tar.bz2
URL_DIR=auto-build/$URL_DATE
BASE_URL=http://${DOWNLOAD_SERVER}/$URL_DIR

BUILD_DIR=/tmp/`basename $0`-$URL_DATE

if [ -e $BUILD_DIR ]; then
    echo "$BUILD_DIR exists."
    rm -rf $BUILD_DIR/pd-extended
else
    mkdir $BUILD_DIR
fi
cd $BUILD_DIR

echo "Downloading source package from $BASE_URL"

# always fetch MD5s to make sure the existing tarballs were correct
rm -f *.md5

# get the tarballs, and use existing versions if already there
#wget -c ${BASE_URL}/${ORIG}
#wget -c ${BASE_URL}/${DEBIAN}
#wget ${BASE_URL}/${ORIG}.md5
#wget ${BASE_URL}/${DEBIAN}.md5
rsync -ax -e ssh apt@puredata.info:~/www/$URL_DIR/$ORIG \
    apt@puredata.info:~/www/$URL_DIR/$DEBIAN \
    apt@puredata.info:~/www/$URL_DIR/$ORIG.md5 \
    apt@puredata.info:~/www/$URL_DIR/$DEBIAN.md5 \
    $BUILD_DIR/

md5sum -c ${ORIG}.md5
md5sum -c ${DEBIAN}.md5

if [ -d pd-extended ]; then
    rm -rf pd-extended
fi
tar xjf ${ORIG}
# rename tarball to Debian orig name
mv $ORIG `echo $ORIG | sed 's,^P,p,' | sed 's,-source,.orig,'`
cd pd-extended
tar xjf ../${DEBIAN}

sed -i \
    's|^\(pd-extended (0.*-[0-9]\).*) [a-zA-Z][a-zA-Z]*;|\1~lucid) lucid;|' \
    debian/changelog
debuild -S
sed -i s,lucid,maverick,g debian/changelog && debuild -S
sed -i s,maverick,natty,g debian/changelog && debuild -S
sed -i s,natty,oneiric,g debian/changelog && debuild -S
sed -i s,oneiric,precise,g debian/changelog && debuild -S
sed -i s,precise,quantal,g debian/changelog && debuild -S
sed -i s,quantal,raring,g debian/changelog && debuild -S

for f in ../pd-extended_*~*.changes; do
    echo $f
    dput ppa:eighthave/pd-extended $f
done
