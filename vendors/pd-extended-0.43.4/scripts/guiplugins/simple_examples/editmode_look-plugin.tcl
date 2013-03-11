# META NAME edit mode look plugin
# META DESCRIPTION makes edit and interact modes look different from each other
# META AUTHOR Hans-Christoph Steiner <hans@eds.org>
# META VERSION 0.2
# META LICENSE public domain

namespace eval ::editmode_look {
    # array of the original background colors for each window
    array set original_color {}
}

proc ::editmode_look::set_cords_by_editmode {mytoplevel} {
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
        $tkcanvas itemconfigure graph -fill black
        $tkcanvas itemconfigure array -fill black
        $tkcanvas itemconfigure array -activefill blue
        $tkcanvas itemconfigure label -fill black
        $tkcanvas itemconfigure msg -activefill black
        $tkcanvas itemconfigure atom -activefill black
        $tkcanvas itemconfigure cord -fill black
        $tkcanvas itemconfigure {inlet || outlet} -outline black
        $tkcanvas raise {inlet || outlet || cord}
        # store the background color, in case its been changed
        set original_color($mytoplevel) [$tkcanvas cget -background]
        $tkcanvas configure -background "lightblue"
    } else {
        $tkcanvas itemconfigure graph -fill grey
        $tkcanvas itemconfigure array -fill cyan
        $tkcanvas itemconfigure array -activefill blue
        $tkcanvas itemconfigure label -fill "#777777"
        $tkcanvas itemconfigure msg -activefill blue
        $tkcanvas itemconfigure atom -activefill blue
        $tkcanvas itemconfigure cord -fill grey
        $tkcanvas itemconfigure {inlet || outlet} -outline white
        $tkcanvas lower {inlet || outlet || cord}
        $tkcanvas configure -background $original_color($mytoplevel)
    }
}

bind PatchWindow <<EditMode>> {+::editmode_look::set_cords_by_editmode %W}
bind PatchWindow <<Loaded>> {+::editmode_look::set_cords_by_editmode %W}
