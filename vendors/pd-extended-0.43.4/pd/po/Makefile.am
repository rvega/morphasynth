AUTOMAKE_OPTIONS = foreign

SUFFIXES = .po .pot .msg

# this is the only way to get gettext 0.17 with old Fink packages, with an
# up-to-date Fink, you need to: "fink install gettext-bin gettext-tools"
if MACOSX
  PATH := /sw/lib/gettext-tools-0.17/bin:${PATH}
endif

TCLFILES = apple_events.tcl AppMain.tcl dialog_array.tcl dialog_audio.tcl dialog_canvas.tcl dialog_data.tcl dialog_find.tcl dialog_font.tcl dialog_gatom.tcl dialog_iemgui.tcl dialog_message.tcl dialog_midi.tcl dialog_path.tcl dialog_startup.tcl helpbrowser.tcl makeapp.tcl opt_parser.tcl pd_bindings.tcl pd_connect.tcl pd_guiprefs.tcl pd-gui.tcl pd_menucommands.tcl pd_menus.tcl pdtk_canvas.tcl pdtk_text.tcl pdwindow.tcl scrollbox.tcl scrollboxwindow.tcl wheredoesthisgo.tcl

FILES = $(addprefix ../tcl/, $(TCLFILES)) iemgui_dynamic_strings.tcl pd_dynamic_strings.tcl

# these are the supported languages, 
ALL_LINGUAS = af az be bg ca cs da de el en_ca en_gb es es_mx eu fr gd gu he hi hu it ja ko lt lv nl pa pt pt_pt ru sk sq sv tr vi zh_cn zh_tw
POFILES = $(ALL_LINGUAS:=.po)
MSGFILES = $(ALL_LINGUAS:=.msg)

TEMPLATE = template.pot

libpdpodir = $(pkglibdir)/po
libpdpo_DATA = $(MSGFILES)
dist_libpdpo_DATA =

EXTRA_DIST = $(POFILES)

# generate .msg files from the .po files
all-local: $(MSGFILES)



# refresh .po files from the template
clean-local:
	-rm -f -- $(MSGFILES)
	-rm -f -- $(POFILES:=~)

po: $(POFILES)

# refresh the template from the source code
template: $(TEMPLATE)

$(TEMPLATE): $(FILES)
	xgettext --join-existing \
		--from-code=UTF-8 --language=Tcl --keyword=_ \
		--sort-output --no-location --output=$(TEMPLATE) \
		--package-name="Pure Data" --package-version=0.43 \
		--copyright-holder='This file is put in the public domain' \
		--foreign-user \
		--msgid-bugs-address=http://bugs.puredata.info \
		$(FILES)

# I guess officially, the .po file should depend on the template.pot, but its
# mostly annoying since it wasnts to update the template.pot and .po files any
# time a .tcl file changes
#$(POFILES): %.po: $(TEMPLATE)
$(POFILES): %.po:
	msgmerge --sort-output --no-fuzzy-matching --no-location --update $< $(TEMPLATE)


%.msg: %.po
	msgfmt --check --tcl --locale=$* -d . $<


etags: TAGS
	etags --append --language=none --regex="/proc[ \t]+\([^ \t]+\)/\1/" *.tcl
