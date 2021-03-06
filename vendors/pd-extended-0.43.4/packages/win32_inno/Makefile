#
# This is a grand unifying Makefile for compiling Pd-extended under MinGW
#
all: install

export CC = gcc
CWD := $(shell pwd)

DESTDIR = $(CWD)/build
cvs_root_dir := $(shell cd $(CWD)/../.. && pwd)
BUILDLAYOUT_DIR = $(cvs_root_dir)/packages


include $(BUILDLAYOUT_DIR)/Makefile.buildlayout

# to control whether the debug symbols are kept
#STRIP = strip --strip-unneeded -R .note -R .comment
STRIP = echo

DEBUG_CFLAGS = -g -fno-inline-functions -fno-omit-frame-pointer -DDEBUG_SOUNDFILE -Wstrict-aliasing=2

OPT_CFLAGS = -O3 -funroll-loops
# compile for pentium4 so we can use SSE2
OPT_CFLAGS += -march=pentium4 -msse2 -mfpmath=sse
OPT_CFLAGS += -ftree-vectorize -ftree-vectorizer-verbose=1

# these are sent to all of the various Makefiles so that they all copy their
# output to the same directory tree
DEST_PATHS = BUILDLAYOUT_DIR=$(BUILDLAYOUT_DIR) \
				cvs_root_dir=$(cvs_root_dir) \
				DESTDIR=$(DESTDIR) \
				prefix=$(prefix) \
				OPT_CFLAGS="$(OPT_CFLAGS)" \
				UNAME=$(UNAME) \
				STRIP="$(STRIP)"

PD_INNO_SETUP = pd-inno.iss

package: installer zip

installer:  $(PD_INNO_SETUP)
## this pops up the GUI app
#	start $(PD_INNO_SETUP)
## this compiles the package on the command line
	"$(PROGRAMFILES)/Inno Setup 5/ISCC.exe" "$(PD_INNO_SETUP)"

zipfolder = $(packages_src)/win32_inno/zip
zipsource = $(packages_src)/win32_inno/Output/Pd-$(PD_VERSION).exe
zippackage = Pd-$(PD_VERSION)
zippackagefolder = $(zipfolder)/$(zippackage)
zip: $(zipsource)
# this fake installs the above into a folder to be zipped up
	install -d "$(zipfolder)"
	cp "$(zipsource)" "$(zipfolder)"
# test the inno exe
	innounp -v -t -b -y zip/$(zippackage).exe -d"$(zipfolder)"
# extract the inno exe
	innounp -v -x -b -y zip/$(zippackage).exe -d"$(zipfolder)"
	cd "$(zipfolder)" && \
		mv \{app\} $(zippackagefolder) && \
		mv \{fonts\} $(zippackagefolder)/fonts && \
		install -p \{sys\}/*.dll $(zippackagefolder)/bin/ && \
		install -p \{sys\}/*.exe $(zippackagefolder)/bin/ && \
		zip -9r --no-wild "$(zippackage).zip" "$(zippackage)"
	@echo " "
	@echo "win32_inno install succeeded!"

$(pd_src)/asio/ASIOSDK2: $(HOME)/asiosdk2.2.zip
	unzip $(HOME)/asiosdk2.2.zip -d $(pd_src)/asio/

build_pd: $(pd_src)/asio/ASIOSDK2
	$(MAKE) -C $(pd_src)/src -f makefile.mingw MORECFLAGS="-O2 -g -fno-tree-vectorize -fno-strict-aliasing"

# use autoconf setup to install the translation files
pd_po_msg_install:
	cd $(pd_src) && ./autogen.sh && ./configure
	make -C $(pd_src)/po $(DEST_PATHS) libpdpodir=$(libpddir)/po install

PD_NAME=Pd
pd_install: build_pd pd_po_msg_install
# the autoconf/MinGW setup doesn't compile the extras yet
#	$(MAKE) -C $(pd_src)/src $(DEST_PATHS) bin
#	-$(MAKE) -C $(pd_src)/src $(DEST_PATHS) install
	$(MAKE) -C $(pd_src)/src -f makefile.mingw $(DEST_PATHS) install
# make the Pd headers available in the standard locations
	install -d $(DESTDIR)/include/pd
	install -p $(DESTDIR)/include/pdextended/*.h $(DESTDIR)/include/pd
# install notes.txt into the help browser
	install -d $(DESTDIR)$(manualsdir)/$(PD_NAME)
	install -p $(pd_src)/src/notes.txt $(DESTDIR)$(manualsdir)/$(PD_NAME)

install: pd_install prebuilt_install lib_install exe_install
	@echo " "
	@echo "win32_inno install succeeded!"


prebuilt_install:
	$(MAKE) -C $(packages_src) $(DEST_PATHS) extended_install
	$(MAKE) -C $(packages_src) $(DEST_PATHS) doc_format
	install -p pd-settings.reg $(DESTDIR)$(prefix)/bin
	install -p pd-extended.bat $(DESTDIR)$(prefix)


#==============================================================================#
#
## setup Pd binaries
#
#==============================================================================#

bin_src = /usr/local/bin
system32 = /c/WINDOWS/system32
lib_install:
# ultimately, the DLLs should be installed in %SystemRoot%\system32 by
# InnoSetup since they are 'officially' named and versioned DLLs, and can be
# shared between apps
	install -d $(DESTDIR)$(prefix)/
	install -p \
		$(bin_src)/libbz2-2.dll \
		$(bin_src)/libcurl-4.dll \
		$(bin_src)/libdca-0.dll \
		$(bin_src)/libdl.dll \
		$(bin_src)/libfftw3-3.dll \
		$(bin_src)/libfftw3f-3.dll \
		$(bin_src)/libfreetype-6.dll \
		$(bin_src)/libgavl-1.dll \
		$(bin_src)/libgcc_s_dw2-1.dll \
		$(bin_src)/libgnurx-0.dll \
		$(bin_src)/libjpeg.dll \
		$(bin_src)/libmp3lame-0.dll \
		$(bin_src)/libogg-0.dll \
		$(bin_src)/libopenjpeg-2.dll \
		$(bin_src)/libpng-3.dll \
		$(bin_src)/libpng12-0.dll \
		$(bin_src)/libsndfile-1.dll \
		$(bin_src)/libstdc++-6.dll \
		$(bin_src)/libtheora-0.dll \
		$(bin_src)/libtheoradec-1.dll \
		$(bin_src)/libtheoraenc-1.dll \
		$(bin_src)/libtiff-3.dll \
		$(bin_src)/libtiffxx-3.dll \
		$(bin_src)/libvorbis-0.dll \
		$(bin_src)/libvorbisenc-2.dll \
		$(bin_src)/libvorbisfile-3.dll \
		$(bin_src)/libx264-116.dll \
		$(bin_src)/libz-1.dll \
		$(bin_src)/lua51.dll \
		$(bin_src)/pthreadGC2.dll \
		$(bin_src)/xvidcore.dll \
		$(DESTDIR)$(prefix)/
# very old pthread lib from pd-vanilla
	install -d $(DESTDIR)$(bindir)
	install -p pthreadVC.dll $(DESTDIR)$(bindir)
# these stay with pd.exe
	install -p \
		$(bin_src)/tcl85.dll \
		$(bin_src)/tclpip85.dll \
		$(bin_src)/tk85.dll \
		$(DESTDIR)$(bindir)
# if these are installed, include them in the build root
	-install -p $(system32)/msvcp71.dll   $(DESTDIR)$(bindir)
	-install -p $(system32)/msvcr71.dll   $(DESTDIR)$(bindir)
# these go into the Pd package's lib dir
	install -d $(DESTDIR)$(libdir)/
	rsync -axv --cvs-exclude \
		/usr/local/lib/tcl8 \
		/usr/local/lib/tcl8.5 \
		/usr/local/lib/tk8.5 \
		/usr/local/lib/dde1.4 \
		/usr/local/lib/reg1.2 \
		/usr/local/lib/tkdnd2.6 \
		/usr/local/lib/tcllib1.13 \
		$(DESTDIR)$(libdir)/


# tcllib install
# tcllib is an old standard lib for Tcl written in Tcl, it includes things we
# need, like base64, so include it in the package on all platforms
tcllib_install:
	install -d $(DESTDIR)$(libdir)
	cp -a /usr/local/lib/tcllib1.13 $(DESTDIR)$(libdir)/tcllib1.13

exe_install:
	install -d $(DESTDIR)$(bindir)
	install -p $(bin_src)/wish85.exe  $(DESTDIR)$(bindir)
	install -p $(bin_src)/tclsh85.exe  $(DESTDIR)$(bindir)


$(PD_INNO_SETUP): $(PD_INNO_SETUP).in
	@echo "Making Inno Setup file: $(PD_INNO_SETUP)"
	cat "$(PD_INNO_SETUP).in" | \
		sed 's/PACKAGE_NAME/$(PACKAGE_NAME)/g' | \
		sed 's/PD_VERSION/$(PD_VERSION)/g' | \
		sed 's/PD-EXTENDED_VERSION/$(PD-EXTENDED_VERSION)/g' > \
		$(PD_INNO_SETUP)
#	start $(PD_INNO_SETUP)


#==============================================================================#
#
## CLEAN TARGETS
#
#==============================================================================#

distclean: package_clean
	$(MAKE) -C $(packages_src) $(DEST_PATHS) distclean


package_clean: 	
	-rm $(PD_INNO_SETUP)
	-rm -rf $(DESTDIR)
	-rm -rf Output
	-rm -rf $(zipfolder)


clean: package_clean
	$(MAKE) -C $(packages_src) $(DEST_PATHS) clean

#==============================================================================#
#
## TEST TARGETS
#
#==============================================================================#

test_package:
	echo "No tests here yet"


test_locations:
	@echo --------------------------------------------------
	@echo "PD_VERSION: $(PD_VERSION)"
	@echo "PD-EXTENDED_VERSION: $(PD-EXTENDED_VERSION)"
	@echo "CWD $(CWD)"
	@echo "DESTDIR $(DESTDIR)"
	@echo "PREFIX $(prefix)"
	@echo "BINDIR  $(bindir)"
	@echo "LIBDIR  $(libdir)"
	@echo "OBJECTSDIR  $(objectsdir)"
	@echo "PDDOCDIR  $(pddocdir)"
	@echo "LIBPDDIR  $(libpddir)"
	@echo "LIBPDBINDIR  $(libpdbindir)"
	@echo "HELPDIR  $(helpdir)"
	@echo "MANUALSDIR  $(manualsdir)"
	@echo "EXAMPLESDIR  $(examplesdir)"
	@echo --------------------------------------------------
	autoconf --version
	@echo --------------------------------------------------
	make --version
	@echo --------------------------------------------------
	gcc --version


.PHONY: all build_pd clean distclean exe_install install installer lib_install \
	package package_clean pd_install pd_po_msg_install prebuilt_install \
	tcllib_install test_locations test_package
