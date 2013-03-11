
In order to work on 64-bit systems, some objects need to be updated to use
newer features of Pd.  These objects will not be able to properly read or
write arrays until they are fixed.  When you use the broken feature, you will
get an error message like this in the Pd window:

    An operation on the array '$0-a2' in the patch 'arbran-help.pd'
    failed since it uses garray_getfloatarray while running 64-bit!

The only method of accessing arrays in C code, garray_getfloatarray() only
works on 32-bit systems and only gives half of the data when running 64-bit.
There is a new API that is 32-bit and 64-bit compatible called
garray_getfloatwords().

Here is a listing of the objects that currently still use the old
garray_getfloatarray():

    bsaylor/pvoc~.c
    creb/modules/statwav~.c
    creb/modules/tabreadmix~.c
    cxc/mean~.c
    ggee/experimental/fofsynth~.c
    ggee/experimental/tabwrite4~.c
    maxlib/arbran.c
    maxlib/score.c
    maxlib/subst.c
    miXed/shared/common/vefl.c
    miXed/cyclone/hammer/funbuff.c
    miXed/cyclone/sickle/cycle.c
    miXed/shared/sickle/arsic.c
    moocow/flite/flite.c
    moocow/pdstring/src/array2rawbytes.c
    moocow/pdstring/src/pdstringUtils.c
    moocow/pdstring/src/rawbytes2array.c
    moonlib/readsfv~.c
    moonlib/tabdump2.c
    moonlib/tabenv.c
    moonlib/tabreadl.c
    moonlib/tabsort.c
    moonlib/tabsort2.c
    mrpeach/flist2tab/flist2tab.c
    mrpeach/tab2flist/tab2flist.c
    mrpeach/tabfind/tabfind.c
    pdp/modules/image_basic/pdp_cheby.c
    pdp/modules/image_io/pdp_qt.c
    pdp/modules/image_special/pdp_array.c
    pdp/modules/image_special/pdp_histo.c
    pdp/modules/matrix_basic/clusterstuff.c

You can see some examples of how to fix this issue here:
 http://pure-data.svn.sourceforge.net/viewvc/pure-data/trunk/externals/bsaylor/partconv~.c?r1=10320&r2=16806
 http://pure-data.svn.sourceforge.net/viewvc/pure-data/trunk/externals/maxlib/arraycopy.c?r1=16070&r2=16069&pathrev=16070


For Mac OS X, there are some APIs that Apple deprecated in their transition to
64-bit.  Here are the affected objects:

 * pdp_qt, pdp_ieee1394: use 64-bit API to replace Quicktime and Carbon calls
 * tclpd: make Pd work with Tk/Cocoa
 * hid: use 64-bit API to replace Carbon calls
 * gemwin, pix_video, pix_film, pix_image: use 64-bit API to replace
   Quicktime and Carbon calls
 * gem2pdp: get Gem and pdp built
