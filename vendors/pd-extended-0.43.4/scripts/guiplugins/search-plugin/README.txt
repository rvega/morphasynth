
This is a plugin that searches all of the .pd, .max, .txt, and .html files that
are in the included docs and in any libraries that are currently in Pd's path.
The syntax of the search can be a keyword search or a Tcl regular expression
(regexp).  Here are some examples:

amplitude              <-- keyword "amplitude"
amplitude modulation   <-- keywords "amplitude" and "modulation"
"amplitude modulation" <-- phrase "amplitude modulation"
melod(y|ies)           <-- regular expression
keywords[^;]*(network).*?;  <-- complex regular expression

A lot of care has been taken so that the average user can get by without
directly using regular expressions at all.  For example, typing "melod(y|ies)"
in the search bar is equivalent to typing "melody melodies" and unchecking
the box for "Match all terms".  Additionally, a complex regular expression
like the keywords example above may be triggered by clicking one of various
links under the heading "Keywords Search" on the homepage of the search window.

You can find out about the Tcl regexp syntax here for advanced searches:
http://tcl.tk/man/tcl8.5/TclCmd/re_syntax.htm

Hans-Christoph Steiner <hans@eds.org>
Jonathan Wilkes
