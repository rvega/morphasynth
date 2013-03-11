#!/bin/bash
# this script is the first attempt to have an automated updater and builder

# echo each command as its executed
set -x

# the source dir where this script is
## this could be done more easily with ${0%/*}
SCRIPT_DIR=$(echo $0 | sed 's|\(.*\)/.*$|\1|')
. $SCRIPT_DIR/auto-build-common

# the name of this script
## this could be done more easily with ${0##*/}
SCRIPT=$(echo $0| sed 's|.*/\(.*\)|\1|g')

BUILD_DIR=.
case $SYSTEM in 
    linux)
        BUILD_DIR=linux_make
        echo "Configuring to use $BUILD_DIR on GNU/Linux"
        ;;
    darwin)
        BUILD_DIR=darwin_app
        echo "Configuring to use $BUILD_DIR on Darwin/Mac OS X"
        ;;
    mingw*)
        BUILD_DIR=win32_inno
        echo "Configuring to use $BUILD_DIR on MinGW/Windows"
        ;;
    cygwin*)
        BUILD_DIR=win32_inno
        echo "Configuring to use $BUILD_DIR on Cygwin/Windows"
        ;;
    *)
        echo "ERROR: Platform $SYSTEM not supported!"
        exit
        ;;
esac


# convert into absolute path
cd "${SCRIPT_DIR}/../.."
auto_build_root_dir=`pwd`
echo "build root: $auto_build_root_dir" 

# clean up and update
# run 'make distclean' first so the autotools Makefiles still exist
echo "--- make distclean -------------------------------------"
make -C ${auto_build_root_dir}/packages/$BUILD_DIR distclean

echo "--- use SCM to clean and update ------------------------"
svn=`find_svn`
# first pass at the root without externals
$svn up --ignore-externals $auto_build_root_dir
# then each folder in the root
for dir in $auto_build_root_dir/*; do
    clean_and_update_folder $dir
    remove_dead_svn_externals $dir
done
remove_dead_svn_externals $auto_build_root_dir

# kludge to restore folders when they've been converted from svn:externals to
# directly checked in
lib_targets=`sed -n 's|^ *LIB_TARGETS +*= *||p' $auto_build_root_dir/externals/Makefile`
cd $auto_build_root_dir/externals
$svn update $lib_targets

echo "--- setting up build -----------------------------------"
cd "${auto_build_root_dir}/packages/$BUILD_DIR"
make -C "${auto_build_root_dir}/packages" set_version
make test_locations
echo "--- environment ----------------------------------------"
set
echo "--- mounts ----------------------------------------"
mount
print_ip_address
echo "--- uname ----------------------------------------"
uname -a

if [ "$1" = "source-tarball-only" ]; then
    echo "--- making source tarball ------------------------------"
    version=`get_pd_version "$auto_build_root_dir"`
    tarballname=Pd-extended_$version-source.tar.bz2
    make_source_tarball "${auto_build_root_dir}" $tarballname
    echo "The source tarball is here:"
    echo "  /tmp/$tarballname"
    echo "Finished cleaning and making source tarball, exiting."
    exit
fi

echo "--- compiling binary package ---------------------------"
make install && make package


echo "--- uploading build results ----------------------------"
upload_build ()
{
    platform_folder=$1
    build_folder=$2
    archive_format=$3
    
    archive=$(ls -1 ${auto_build_root_dir}/packages/${platform_folder}/${build_folder}/Pd*.${archive_format} | tail -1)
    
    echo "upload specs $1 $2 $3"
    echo "Uploading $archive"
    upload_filename=$(ls -1 ${archive} | sed "s|.*/\(.*\)\.${archive_format}|\1-${HOSTNAME}.${archive_format}|")
    if [ -e ${archive} ]; then
        rsync -a --chmod=a+r ${archive} -e ssh ${RSYNC_SERVER}:/home/apt/www/auto-build/${DATE}/${upload_filename}  && \
            md5sum ${archive} | sed "s|^\([0-9a-f][0-9a-f]*  \).*|\1${archive}|" > ${archive}.md5 && \
            rsync -a --chmod=a+r ${archive}.md5 -e ssh ${RSYNC_SERVER}:/home/apt/www/auto-build/${DATE}/${upload_filename}.md5  && \
            echo "successfully uploaded: ${upload_filename}" && \
            echo SUCCESS
    fi
}


case $SYSTEM in 
    linux)
        if [ -x /usr/bin/dpkg-deb ]; then
            upload_build linux_make . deb
        else
            upload_build linux_make build tar.bz2
        fi
        ;;
    darwin)
        upload_build darwin_app . dmg
        ;;
    mingw*)
        upload_build win32_inno Output exe
        upload_build win32_inno zip zip
        ;;
    cygwin*)
        upload_build win32_inno Output exe
        ;;
esac

