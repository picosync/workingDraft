#!/usr/bin/python
# This is just a little script that helps dig through loads of tcpdump output.
# It filters away the IP+UDP headers and annotates each package with CSS classes
# based on specific bytes in the package.
# As a result, it outputs a .html file that contains some js code to show/hide
# specific packet types. Just try it out to see how it works:
#
# Usage: ./parsedump [inputFile] [limit]
#
#   Without parameters, the script will read from stdin.
#   With one parameter, the par is the input filename
#   If you specify a second parameter, it's the package count limit
#
#   To create dumps parsable by this script, run tcpdump like this:
#     tcpdump -i <intf> -X -U -s0 udp and host <ip1> and host <ip2>|tee -a /path/to/file.dump
#
#   You can use other pcap filters if you like to, as this one won't get the
#   broadcast stuff (which doesn't concern me right now anyway
#   
#
# This script currently uses the start of my host name to annotate the sender.
# ("rech" and "rasp" (you guessed right, one of them is a raspberry pi ;))).
# If you want to change that behaviour, just edit that file for now. I might
# add checking an environment var for that purpose later...
#
# The current version was written back when I didn't know that btsync uses µTP
# as an underlying protocol. But it helped me finding that out :)
#
# This script is going to be updated soon to only show the data within the µTP
# packets

import sys

# reads a packet from f and returns the binary contents (with the IP+UDP headers stripped)
def parsePackets(f):
	for (direction, pkg) in _parsePackets(f):
		yield direction, pkg[28:]

def _parsePackets(f):
	pkg = ''
	for line in f:
#		if len(line) == 0:
#			break # this shouldn't happen as each line should end with a \n
		if line[0] == '\t':
			# hex line
			hexLine = line.split('  ')[1].replace(' ', '')
			pkg += hexLine

		elif len(line) > 1:
			# header, new pkg
			if pkg != '':
				yield direction, pkg.decode('hex')

			host = line.split(' IP ')[1][:4]
			direction = '?'
			if host == 'rech':
				direction = 'o'
			elif host == 'rasp':
				direction = 'i'

			pkg = ''

	if pkg != '':
		yield direction, pkg.decode('hex')
	return


		

def printPkgs(f, limit=None):
	for (direction, pkg) in parsePackets(f):
		if limit != None:
			limit -= 1
			if limit <= 0: break

		cls = []
		if direction == 'o': cls.append('outPkg')
		elif direction == 'i': cls.append('inPkg')
		else: cls.append('unknownPkg')

		hexPkg = pkg.encode('hex')
		n=4
		prettyPkg = ' '.join([hexPkg[i:i+n] for i in range(0, len(hexPkg), n)])


		# annotate some special pkg types
		if hexPkg[:4] == '0100':
			cls.append('dataPkg')
		elif hexPkg[:4] == '2100':
			cls.append('ackPkg')
		elif hexPkg[:4] == '4100':
			cls.append('ctrInitPkg')
		elif hexPkg[:12] == '4253594e4300':
			cls.append('announcePkg')
		else:
			cls.append('otherPkg')

		if hexPkg[40:52] == '4253594e4300':
			cls.append('aesPkg')
		

		prettyPkg = prettyPkg.replace('4253 594e 4300', '<span class="bsyncString">4253 594e 4300</span>')

		print '<div class="{cls}" data-len="{pkglen}">{pkglen:4}:  {pkg}</div>'.format(cls=' '.join(cls), pkg=prettyPkg, pkglen=len(pkg))

def printHtml(f, limit=None):
	print """<html>
<head>
<title>Btsync packet capture</title>
<script src="http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js"></script>
<script>
	function toggleVisibility(self) {
		var $self = $(self);
		var cls = self.dataset['cls'];
		var sel = $('.'+cls);
		$self.toggleClass('hidden')
		var hidden = $self.hasClass('hidden')
		
		if (hidden) sel.hide();
		else sel.show();

		// set the btn text
		var txtState = hidden ? 'Show' : 'Hide';
		$self.text(txtState+' '+self.dataset['text']+' Packets');
	}
</script>
<style>

* {
	box-sizing: border-box;
	margin: 0;
	padding: 0;
}

html, body {
	height: 100%;
	width: 100%;
}

#btnBar {
	position: absolute;
	top: 0;
	left: 0;
	height: 36px;
	width: 100%;
	background-color: white;
	border-bottom: 1px solid #ccc;
	padding: 2px;
}

#pkgContainer {
	height: 100%;
	width: 100%;
	overflow: auto;
	padding-top: 38px;
}
.inPkg, .outPkg {
	font-family: monospace;
	white-space: pre;
}
.outPkg {
	font-weight: bold;
	color: red;
}
.announcePkg {
	color: #ccc;
}
.unknownPkg {
	color: #aaa;
}
.bsyncString {
	_background-color: yellow;
	border-bottom: 1px solid black;
}

.btn {
	cursor: pointer;
	border: 2px outset #ccc;
	background-color: #ccc;
	display: inline-block;
	padding: 3px;
}

#btnBar .hidden {
	background-color: #eee;
}

#btnBar {
	margin-bottom: 1em;
}
</style>
</head>
<body>
<div id="btnBar">
	<div class="btn" onclick="toggleVisibility(this)" data-cls="announcePkg" data-text="BSYNC Announce">Hide BSYNC Announce Packets</div>
	<div class="btn" onclick="toggleVisibility(this)" data-cls="ackPkg" data-text="Ack">Hide Ack Packets</div>
	<div class="btn" onclick="toggleVisibility(this)" data-cls="ctrInitPkg" data-text="Init">Hide Init Packets</div>
	<div class="btn" onclick="toggleVisibility(this)" data-cls="aesPkg" data-text="AES">Hide AES Packets</div>
	<div class="btn" onclick="toggleVisibility(this)" data-cls="dataPkg" data-text="Data">Hide Data Packets</div>
	<div class="btn" onclick="toggleVisibility(this)" data-cls="otherPkg" data-text="Other">Hide Other Packets</div>
</div>
<div id="pkgContainer">
"""

	printPkgs(f, limit)

	print "</div>\n</body>\n</html>"

limit = None
if len(sys.argv) == 1:
	f = sys.stdin
elif len(sys.argv) <= 3:
	f = open(sys.argv[1], 'r')
	if len(sys.argv) == 3:
		limit = int(sys.argv[2])
else:
	sys.stderr.write("Usage: {prog} filename [pkg-limit]\n".format(sys.argv[0]))
	sys.exit(1)

try:
	printHtml(f, limit)
except IOError:
	None
