# These are strings that are passed from C to Tcl but the msgcat
# procedure call '[_ $string]' is on the Tcl side, so that the
# automatic parsing done by xgettext doesn't pick them up as strings
# to be localized.  Therefore we just list them here to make xgettext
# happy.
#
# If these are changed in the src/*.c files, then need to be
# manually updated here.

puts [_ "File"]
puts [_ "Edit"]
puts [_ "Put"]
puts [_ "Find"]
puts [_ "Media"]
puts [_ "Window"]
puts [_ "Help"]

puts [_ "Discard changes to '%s'?"]
puts [_ "Close this window?"]
puts [_ "really quit?"]

puts [_ "Undo clear"]
puts [_ "Undo connect"]
puts [_ "Undo cut"]
puts [_ "Undo disconnect"]
puts [_ "Undo duplicate"]
puts [_ "Undo motion"]
puts [_ "Undo paste"]
puts [_ "Undo typing"]

puts [_ "Redo clear"]
puts [_ "Redo connect"]
puts [_ "Redo cut"]
puts [_ "Redo disconnect"]
puts [_ "Redo duplicate"]
puts [_ "Redo motion"]
puts [_ "Redo paste"]
puts [_ "Redo typing"]

# these should be found in pd-gui.tcl, but they're not... arg
puts [_ "Associated Files"]
puts [_ "Pd Files"]
puts [_ "Max Patch Files"]
puts [_ "Max Text Files"]
puts [_ "Max Binary Files"]
puts [_ "Max Help Files"]
