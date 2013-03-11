#!/bin/sh

# show each thing being executed
set -x

# Any subsequent commands which fail will cause the shell script to exit
set -e

upload_generated_file()
{
    file=$1
    subdir=$2
    rsync_file $file $subdir/`basename $file`
    md5sum ${file} | sed "s|^\([0-9a-f][0-9a-f]*  \)/.*/|\1|" > ${file}.md5
    rsync_file $file.md5 $subdir/`basename $file`.md5
}

# the source dir where this script is
## this could be done more easily with ${0%/*}
SCRIPT_DIR=$(echo $0 | sed 's|\(.*\)/.*$|\1|')
. $SCRIPT_DIR/auto-build-common

# convert into absolute path
cd "${SCRIPT_DIR}/../.."
auto_build_root_dir=`pwd`
echo "build root: $auto_build_root_dir" 

$SCRIPT_DIR/pd-extended-auto-builder.sh source-tarball-only

# the version is set by the above script, so keep this after
version=`get_pd_version "$auto_build_root_dir"`

source=`ls -1 /tmp/Pd-extended*$version*-source.tar.bz2 | tail -1`
upload_generated_file $source $DATE
debian=`ls -1 /tmp/Pd-extended*$version*-source.debian.tar.bz2 | tail -1`
upload_generated_file $debian $DATE
if [ "$running_on_server" = "yes" ]; then
    rm -f -- $source $source.md5
    rm -f -- $debian $debian.md5
fi
echo SUCCESS
