#!/bin/sh
# rsync the SVN source files from SourceForge to make a local copy

LOGFILE=/tmp/rsync-source-repos-from-sourceforge.sh.txt

VERBOSE_FLAGS=-v
#VERBOSE_FLAGS="-v --progress"

echo "" > $LOGFILE

test -d pure-data-backups || mkdir pure-data-backups
cd pure-data-backups

test -d svnroot || mkdir svnroot >> $LOGFILE 2>&1
cd svnroot

echo "rsync pure-data svn" >> $LOGFILE 2>&1
echo "----------------------------------------------------------------------" >> $LOGFILE 2>&1
/usr/bin/rsync -ax $VERBOSE_FLAGS --delete  pure-data.svn.sourceforge.net::svn/pure-data/ svnroot/pure-data/ >> $LOGFILE 2>&1

cd ..
test -d gitroot || mkdir gitroot >> $LOGFILE 2>&1
cd gitroot

echo "rsync pure-data git" >> $LOGFILE 2>&1
echo "----------------------------------------------------------------------" >> $LOGFILE 2>&1
/usr/bin/rsync -av $VERBOSE_FLAGS --delete  pure-data.git.sourceforge.net::gitroot/pure-data/* .   >> $LOGFILE 2>&1

echo "rsync pd-gem git" >> $LOGFILE 2>&1
echo "----------------------------------------------------------------------" >> $LOGFILE 2>&1
/usr/bin/rsync -av $VERBOSE_FLAGS --delete  pd-gem.git.sourceforge.net::gitroot/pd-gem/* .   >> $LOGFILE 2>&1


