# this script makes it so that the cords are hidden when not in edit mode

namespace eval ::editmode_look {
    # array of the original background colors for each window
    array set original_color {}
}

proc ::editmode_look::set_cords_by_editmode {mytoplevel eventname} {
    variable original_color
    if {$mytoplevel eq ".pdwindow"} {return}
    set tkcanvas [tkcanvas_name $mytoplevel]
    # if the mytoplevel sent to us doesn't currently have a window, silently quit
    if { ! [winfo exists $mytoplevel] } {return}
    # if the array doesn't have this instance, get the current color
    if {[array get original_color $mytoplevel] eq ""} {
        set original_color($mytoplevel) [$tkcanvas cget -background]
    }
    if {$::editmode($mytoplevel) == 1} {
        # store the background color, in case its been changed
        set original_color($mytoplevel) [$tkcanvas cget -background]
        $tkcanvas configure -background "lightblue"
    } else {
        $tkcanvas configure -background $original_color($mytoplevel)
    }
}

bind PatchWindow <<EditMode>> {+::editmode_look::set_cords_by_editmode %W editmode}
bind PatchWindow <<Loaded>> {+::editmode_look::set_cords_by_editmode %W loaded}
