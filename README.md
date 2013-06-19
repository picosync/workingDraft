picosync
========

picosync aims to provide an open source library that can communicate with the official [BitTorrent Sync][1]
peer to peer file syncing application.

The basic idea is to analyze BitTorrent Sync app's transmission protocol and write an open source library
that can communicate with the official app.
Later on, this library might be used to create a full open source syncing app compatible to btsync.

# Motives
When I first heard about BitTorrent Sync I was excited about the idea behind it. I read into the sparse
information on the official homepage and [forum][3]. I got curious and wanted to know more details about
how this app works, thought of possibly interesting ways to extend it and so on...

But the official software is closed source and there are ([currently?][4]) no protocol specs available.


Thinking about BitTorrent itself (not the sync app, the p2p network) it seems to me that a big reason for
its success is due to the fact that there are numerous clients that form an entire ecosystem. I want that
for btsync aswell.

Also I think there's great potential in this app. I see the possibility for cloud storage hosters who only see the
encrypted stuff, I see many uses for the peer discovery (i.e. one could write a peer to peer VNC/rdesktop alternative),
and so on.

And finally the only way to know that there are no backdoors is using open source software :)

# License
This project will be released according to the [MIT License][2].

# Wanna help?
I'm doing this as a university project but I'm glad for any help (especially the protocol analysis is not the easiest thing to do...)


[1]: http://labs.bittorrent.com/experiments/sync.html
[2]: http://opensource.org/licenses/mit-license.php
[3]: http://forum.bittorrent.com/forum/56-bittorrent-sync/
[4]: http://forum.bittorrent.com/topic/20642-question-to-developers-specifications/#entry54304
