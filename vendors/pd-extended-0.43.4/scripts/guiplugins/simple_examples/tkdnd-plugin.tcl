package require tkdnd

namespace eval ::pddnd {
}

#------------------------------------------------------------------------------#
# open all files dropped on the Pd Window
::tkdnd::drop_target register .pdwindow DND_Files
bind .pdwindow <<Drop:DND_Files>> {::pddnd::open_dropped_files %D}

proc ::pddnd::open_dropped_files {files} {
    foreach file $files {open_file $file}
    return "link"
}

#------------------------------------------------------------------------------#
# create an object using the dropped filename

bind PatchWindow <<Loaded>> {+::pddnd::setup_dndbind %W}

proc ::pddnd::setup_dndbind {mytoplevel} {
    ::pdwindow::error "setup_dndbind $mytoplevel"
    ::tkdnd::drop_target register $mytoplevel DND_Files
    bind $mytoplevel <<Drop:DND_Files>> {::pddnd::pdtk_canvas_makeobjs %W %D %X %Y}
}

proc ::pddnd::pdtk_canvas_makeobjs {mytoplevel files x y} {
    set c 0
    for {set n 0} {$n < [llength $files]} {incr n} {
        if {[regexp {.*/(.+).pd$} [lindex $files $n] file obj] == 1} {
            ::pdwindow::error " do it $file $obj $x $y $c"
            pdsend "$mytoplevel obj $x [expr $y + ($c * 30)] $obj"
            incr c
        }
    }
    return "link"
}
