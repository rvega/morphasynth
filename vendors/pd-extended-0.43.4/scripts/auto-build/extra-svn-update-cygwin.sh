#!/bin/sh
#
# MinGW MSYS does not have its own svn. When using cygwin's svn in
# MSYS, there are sometimes weird issues caused by their different
# types of paths.  This script runs the svn update under cygwin to
# make sure everything runs OK.  The main build script runs this stuff
# under MSYS.

# the source dir where this script is
## this could be done more easily with ${0%/*}
SCRIPT_DIR=$(echo $0 | sed 's|\(.*\)/.*$|\1|')
. $SCRIPT_DIR/auto-build-common

LOGFILE=/tmp/extra-svn-update-cygwin.sh.txt

# convert into absolute path
cd "${SCRIPT_DIR}/../.."
auto_build_root_dir=`pwd`
echo "build root: $auto_build_root_dir" > $LOGFILE 2>&1

# MSYS handles unicode names very poorly, so it might not delete files
# with unicode names.  Cygwin does a better job, so run make clean there
echo "--- remove old build with cygwin in case of unicode ----" >> $LOGFILE 2>&1
make -C $auto_build_root_dir/packages/win32_inno package_clean

echo "--- use SCM to clean and update ------------------------" >> $LOGFILE 2>&1
# first pass at the root without externals
svn up --ignore-externals $auto_build_root_dir >> $LOGFILE 2>&1
# then each folder in the root, but not the root itself, in case pd/
# is coming from a git repo rather than svn. clean_and_update_folder
# will remove pd/ if its not in svn.
for dir in $auto_build_root_dir/*; do
    clean_and_update_folder $dir >> $LOGFILE 2>&1
    remove_dead_svn_externals $dir >> $LOGFILE 2>&1
done
