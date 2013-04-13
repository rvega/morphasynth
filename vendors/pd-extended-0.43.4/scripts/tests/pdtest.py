#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import re
import random
import signal
import smtplib
import socket
import subprocess
import sys
import time

class PdTest():

    def __init__(self, pdrootdir):
        self.pdrootdir = pdrootdir
        self.rand = random.SystemRandom()

    def find_pdexe(self):
        # start with the Windows/Mac OS X location
        exe = self.pdrootdir + '/bin/pd'
        if not os.path.isfile(exe):
            # try the GNU/Linux location
            exe = self.pdrootdir+'../../bin/pd'
            if not os.path.isfile(exe):
                print "ERROR: can't find pd executable"
                exit
        return os.path.realpath(exe)

    def make_netreceive_patch(self, port):
        filename = '/tmp/.____pd_netreceive_'+str(self.port)+'____.pd'
        fd = open(filename, 'w')
        fd.write('#N canvas 222 130 454 304 10;')
        fd.write('#X obj 201 13 import vanilla;')
        fd.write('#X obj 111 83 netreceive ' + str(self.port) + ' 0 old;')
        fd.write('#X obj 111 103 loadbang;')
        fd.write('#X obj 111 123 print netreceive_patch;')
    # it would be nice to have this patch tell us when it is closed...
    #    fd.write('#X obj 211 160 tof/destroysend pd;')
    #    fd.write('#X obj 211 160 closebang;')
    #    fd.write('#X obj 211 180 print CLOSE;')
        fd.write('#X connect 2 0 3 0;')
    #    fd.write('#X connect 3 0 4 0;')
        fd.close()
        self.netreceive_patch = filename

    def send_to_socket(self, message):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(5)
        try:
            s.connect(('localhost', self.port))
            s.send(message)
            s.close()
        except socket.timeout:
            print "socket timed out while sending"

    def send_to_pd(self, message):
        self.send_to_socket('; pd ' + message + ';\n')

    def open_patch(self, filename):
        dir, file = os.path.split(filename)
        self.send_to_pd('open ' + file + ' ' + dir)

    def close_patch(self, filename):
        dir, file = os.path.split(filename)
        self.send_to_pd('; pd-' + file + ' menuclose')

    def launch_pd(self):
        pdexe = self.find_pdexe()
        p = subprocess.Popen([pdexe, '-nogui', '-stderr', '-open', self.netreceive_patch],
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, 
                             close_fds=True);
        line = p.stdout.readline()
        while line != 'netreceive_patch: bang\n':
            line = p.stdout.readline()
        return p

    def quit_pd(self, process):
        self.send_to_pd('quit')
        time.sleep(1)
        try:
            os.kill(process.pid, signal.SIGTERM)
        except OSError:
            print 'OSError on SIGTERM'
        time.sleep(1)
        try:
            os.kill(process.pid, signal.SIGKILL)
        except OSError:
            print "OSError on SIGKILL"


    #---------- list of lines to ignore ----------#
    def remove_ignorelines(self, list):
        ignorelines = [
            'expr, expr~, fexpr~ version 0.4 under GNU General Public License \n',
            'this is pddplink 0.1, 3rd alpha build...\n',
            'beware! this is tot 0.1, 19th alpha build...\n',
            'foo: you have opened the [loadbang] help document\n',
            'verbose(3): comport - PD external for unix/windows\n',
            'verbose(4): freeverb~ v1.2.1\n',
            'verbose(6): backend \'aviplay\' unavailable',
            'verbose(5): threaded ImageLoading not supported!\n',
            'verbose(3): Gem: already loaded\n',
            'verbose(6): frei0r_loader: failed!!\n',
            'verbose(3): [OSCroute]: OSCx is deprecated! \n',
            '        Consider switching to mrpeach\'s [routeOSC]\n',
            'rhythm: using "Large and Kolen" adaptation model',
            'slipdec: maximum packet length is 65536\n',
            'verbose(3): pdlua: already loaded\n',
            'warning: nop~ does not delay any more!\n',
            'verbose(3): libdir loader 1.9\n',
            'verbose(3): Gem: already loaded\n',
            'print: bang\n',
            'print: 207\n',
            'print: 2 1\n',
            'obj3\n',
            'obj4 34\n',
            'initial_bang: bang\n',
            '\n'
            ]
        ignorepatterns = [
            'fiddle version 1.1 ',
            'sigmund version 0.0',
            'bonk version ',
            'pique 0.1 for PD version',
            'pd~ version ',
            'verbose\\(6\\): backend',
            'verbose\\(6\\): trying to add',
            'verbose\\(6\\): added backend',
            'verbose\\(6\\): reading \'.*\' with ',
            'Image loading support: ',
            'verbose\\(4\\): loaded image ',
            'verbose\\(3\\): SG channnel Device List',
            '\\[pix_.*\\]: QuickTime support',
            '\\[pix_.*\\]: Darwin support',
            'error: .* is no longer available!',
            'error: use \\[hid\\] or similar for interfacing a ',
            'listening to port number',
            'listening on port',
            'compiled',
            'iemnet - networking with Pd: ',
            'based on mrpeach/net, based on maxlib',
            'GNU General Public License',
            ' IOhannes m ',
            'Institute for Electronic Music - Graz',
            'institute of electronic music and acoustics',
            ' jasch ',
            'ck@mamalala.de',
            'Richard Dudas',
            'IntegraLive',
            'Bryan Jurish',
            'Martin Peach',
            'Hans-Christoph Steiner',
            'hans@eds.org',
            'Winfried Ritsch',
            'morph_2016@yahoo.co.uk',
            'okin@altern.org',
            ' Ed Kelly',
            'ydegoyon@free.fr',
            'Yves Degoyon',
            'Matt Wright',
            'matt wright',
            'olaf.matthes@gmx.de',
            'Olaf.*Matthes',
            '[a-z]+ v0\.[0-9]',
            'IOhannes m zm',
            'part of zexy-',
            'cooled~ : allocated .* bytes',
            'Pd: 0\.4[0-9]\.[0-9]-extended',
            'verbose\\(3\\): libdir_loader: added .* to the canvas-local objectclass path',
            'verbose\\(14\\): Loaded libdir \'.*\' from ',
            'verbose\\(3\\): \\[import\\] loaded library: ',
            'based on sync from jMax',
            'copyright',
            'Copyright'
            ]
        for ignore in ignorelines:
            try:
                list.remove(ignore)
            except ValueError:
                pass
        for line in list:
            for pattern in ignorepatterns:
                m = re.search('.*' + pattern + '.*', line)
                while m:
                    try:
                        list.remove(m.string)
                        m = re.search('.*' + pattern + '.*', line)
                    except ValueError:
                        break
        return list


    def runtest(self, log, root, filename):
        patchoutput = []
        patch = os.path.join(root, filename)
        self.port = int(self.rand.random() * 10000) + int(self.rand.random() * 10000) + 40000
        self.make_netreceive_patch(self.port)
        p = self.launch_pd()
        try:
            self.open_patch(patch)
            time.sleep(5)
            self.close_patch(patch)
            self.quit_pd(p)
        except socket.error:
            patchoutput.append('socket.error')                 
        while True:
            line = p.stdout.readline()
            m = re.search('EOF on socket', line)
            if not m and line:
                patchoutput.append(line)
            else:
                break
        toprint = self.remove_ignorelines(patchoutput)
        if len(toprint) > 0:
            log.write('\n\n__________________________________________________\n')
            log.write('loading: ' + patch + '\n')
            for line in toprint:
                log.write(line)
            log.flush()

class TestLog():

    def __init__(self, logfile):
        self.logfile = logfile
        self.rsynclog = []
        self.separator = '______________________________________________________________________\n\n'

    def upload(self):
        # upload the log file to the autobuild website
        date = time.strftime('20%y-%m-%d', time.localtime(time.time()))
        logfilename = os.path.basename(self.logfile)
        rsyncfile = str('apt@puredata.info:~/www/auto-build/'
                        + date + '/logs/' + logfilename)
        cmd = ['rsync', '-axv', '-e ssh', self.logfile, rsyncfile]
        self.rsynclog.append(' '.join(cmd) + '\n')
        p = subprocess.Popen(cmd,
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        p.wait()
        self.rsynclog.append(''.join(p.stdout.readlines()))
        self.rsynclog.append(''.join(p.stderr.readlines()))
        self.rsynclog.append(self.separator)
        self.rsynclog.append('Complete log:\n')
        self.rsynclog.append('http://autobuild.puredata.info/auto-build/' + date + '/logs/'
                             + logfilename + '\n')
        self.rsynclog.append(self.separator)

    def email(self,
              subject='PdTest results',
              fromaddr='pd@pdlab.idmi.poly.edu',
              toaddr='hans@at.or.at',
              smtpserver='in1.smtp.messagingengine.com'):
        server = smtplib.SMTP(smtpserver)
        header = []
        header.append('From: ' + fromaddr + '\n')
        header.append('To: ' + toaddr + '\n')
        header.append('Subject: ' + subject + '\n')
        logoutput = []
        fd = open(self.logfile, 'r')
        for line in fd:
            logoutput.append(line)
        server.sendmail(fromaddr, toaddr, ''.join(header + self.rsynclog + logoutput))
        server.quit()


#------------------------------------------------------------------------------#
# for testing from the command line:
def main(argv):
    print('nothing here yet')

if __name__ == "__main__":
    main(sys.argv[1:])
