# This plugin is only to print a warning about the beta status of
# 64-bit builds on various platforms.  It is included on 64-bit builds
# by packages/Makefile

set msg "\nWARNING: 64-bit builds are still beta, some libraries are known to have serious issues ("
if {$tcl_platform(os) eq "Darwin"} {
    append msg "hid, Gem, gem2pdp, tclpd, "
}
append msg "cyclone, maxlib, moonlib, moocow, pdp, bsaylor, etc.)"
::pdwindow::error "$msg\n"
::pdwindow::error "For more info, see the README.txt in '64-bit-warning-plugin' in the Help Browser\n\n"
