#!/usr/bin/python2.5
# this is meant to run on apt.puredata.info, which only has python2.5

import string
import glob
import os,sys,errno
import re
import StringIO


sourcetarballs = []
for root, dirs, files in os.walk('/home/apt/www/auto-build/'):
        dirs.sort()
        if 'latest' in dirs:
                dirs.remove('latest')
        for name in files:
                relativeroot = re.sub('/home/apt/www/auto-build', '..', root)
                m = re.search('^Pd-[01]\.[0-9][0-9].*[^5]$', name)
                if m:
                        commonbuildname = re.sub('-20[01][0-9][01][0-9][0-9][0-9]', '', name)
                        commonbuildpath = os.path.join('/home/apt/www/auto-build/latest', commonbuildname)
                        buildtarball = os.path.join(relativeroot, m.string)
                        try:
                                os.symlink(buildtarball, commonbuildpath)
                        except OSError, e:
                                if e.errno == errno.EEXIST:
                                        #print "removing ", commonbuildpath
                                        os.remove(commonbuildpath)
                                        os.symlink(buildtarball, commonbuildpath)
                m2 = re.search('^Pd-extended_[01]\.[0-9][0-9].*-source.tar.bz2$', name)
                if m2:
                        sourcetarballs.append(os.path.join(root, m2.string))

# delete old source links, we only keep one
for f in glob.glob('/home/apt/www/auto-build/latest/Pd-extended*source*'):
        if os.path.islink(f):
                os.remove(f)

if len(sourcetarballs) > 0:
        sourcetarball = sourcetarballs[-1]
        linkname = re.sub('/20[01][0-9]-[01][0-9]-[0-9][0-9]/', '/latest/', sourcetarball)
        sourcetarball = re.sub('/home/apt/www/auto-build', '..', sourcetarball)
        debiantarball = re.sub('-source', '-source.debian', sourcetarball)
        debianlinkname = re.sub('-source', '-source.debian', linkname)
        try:
                os.remove(linkname)
                os.remove(debianlinkname)
        except Exception, e:
                pass
        try:
                #print 'os.symlink(' + sourcetarball + ', ' + linkname + ')'
                os.symlink(sourcetarball, linkname)
                #print 'os.symlink(' + debiantarball + ', ' + debianlinkname + ')'
                os.symlink(debiantarball, debianlinkname)
                foundsourcetarball = True
        except Exception, e:
                print 'Failed to symlink ',
                print e
