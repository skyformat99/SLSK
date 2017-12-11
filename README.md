# Steam Linux Swiss Knife (SLSK)

Steam Linux Swiss Knife is an open-source program that aims to automate the backup and restore of Steam games, their saves and
configs, centralizing paths in a local and extremely lightweight database.

[[https://github.com/supremesonicbrazil/SLSK/blob/master/SLSK_img.png]]

# Table of Contents

[Summary](#summary)

* [Main features](#main-features)

[Getting started](#getting-started)

* [Installation](#installation)
* [How to use](#how-to-use)

[FAQ](#faq)

[How to contribute](#how-to-contribute)

[Special thanks](#special-thanks)

[Donations](#donations)

[License / Copyright](#license--copyright)

## Summary

There's little reliable information on the internet about save/config locations on Linux, and when it does show up, it's often "hit-or-miss".
Many players are getting tired of having to manually backup their games, which can be even more frustrating when there's too many of
them sitting at the library. Some of us just want to "set it and forget it". Problem is, while Windows users have GameSave Manager, there
is no alternative to Linux, and GSM is certainly not going to be ported at all due to its structure.

This is why Steam Linux Swiss Knife was made: to provide a way to unify information about save/config paths of Linux Steam games into
one open, lightweight database which everyone is invited to add this info to it as they see fit; and to use this database for automating
backups of Linux Steam games, saves and configurations. These goals will be easier to accomplish due to SLSK's open-source nature: new
releases come every day, and considering it's pretty much inefficient for only one person to buy 2000+ Linux Steam games solely for path
documentation purposes, SLSK provides a solid base for anyone to contribute with this documentation. Plus, although most games follow
(or at least should follow) the XDG Base Directory Specifications for writing saves/configs (e.g. ~/.local/share, ~/.config, etc.), there is a
good amount of games ignoring this, writing saves and configs in other specific (and sometimes unknown and/or inadequate) locations
of the system. Some of them may not even have save files, or configurations may be stored exclusively via cloud. Regardless, it is up to us
to find that out and contribute so others can be helped.

So far, there are **133** games registered in SLSK's database, of which **36** have incomplete path information.

### Main features

* **Store known paths for saves and configurations of Linux Steam games**
* **Automate the backup and restore of Linux Steam games, saves and configs with an optional GUI tool**

## Getting started

Before downloading or cloning SLSK, please make sure you have the following dependencies installed:

* `g++` (for C++11)
* `qt5-default` and `qt5-qmake` (minimum Qt 5.5+)

### Installation

1. Download the repository, or clone it with `git clone https://github.com/supremesonicbrazil/SLSK`
2. Open a terminal inside the repo's root directory (or `cd` to it) 
3. `sudo chmod +x INSTALL.sh && sudo ./INSTALL.sh`
4. Drag the desktop entry to wherever you like and run it

### How to use

1. Set a backup folder by clicking the `...` button (below the Start button)
2. Scan for content by clicking any of the scan buttons, according to your preference:
  * Backup Saves, Configs or Games (unlocked by default)
  * Restore Saves, Configs or Games (locked by default until a backup folder is set and respective folders are detected in it)
3. Mark which games you want to backup or restore in the list
4. Press the Start button to initiate the backup or restore process

## FAQ

* **So it's basically a GameSave Manager for Linux?**

Yep, but it does a bit more. GSM only manages saves, SLSK manages saves, configs *and* games, plus it's open-source.

* **Will you expand this to non-Steam games (GOG, Itch.io, repo games, etc.)?**

For the time being, no. It's definitely not impossible, but this is just not the main focus now.

* **Will you do a version for Windows/Mac?**

No. Windows already has GSM, and I'm not familiar with Mac. I don't want to compete with GSM, I want to fill a niche it hasn't filled for years.

* **Will I be VAC banned if I use this?**

SLSK uses no login methods and no API whatsoever, it's all local file scanning so relax ;)

* **Can I use part of your program in a project of mine (code, database entries, etc.)? Why not build this into `insert-open-source-project-here`?**

This is more of a personal project that I want to share with people. Nothing stops you (or `insert-open-source-project-here`'s devs) from
incorporating parts of this project into whatever you like. Seriously, not only I give you full permission to use parts of this project, I also
strongly encourage you to do so. It's open-source after all. Everything from the code up to the database entries, use whatever your heart
desires to use. If you need help, get in touch so I can give you a hand to the best of my abilities :)

Just keep in mind two things: first, this project is under GNU GPL v3 - besides helping to avoid license incompatibilities, if you happen to
improve on whichever part you used, please contribute back, it'll mean a lot to us.

Second, [*please* **don't** do this](http://i0.kym-cdn.com/photos/images/original/001/079/173/ed2.png). Seriously, it's not cool and
people will hate you if they find out, please keep the original credits, even if it's just a tiny mention somewhere :(

* **But what about `insert-similar-project-name-here`? Y'know this guy's doing this and that, looks pretty much the same...**

*I know.* Even if there's a thousand alternatives popping up every second, the objective for each one is different, even if a bit slightly.
My objective with SLSK is not to create another standard, but rather try to *adapt* to those standards.

Let's be honest - unless saves/configs are being written inside the own game's folder, which IMO would be the best of both worlds, every
dev making Linux games (especially those who came from Windows - not criticizing, just trying to raise attention) should at the very least be
following the [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html), that way
our lives would get easier and the whole paradigm of "Linux is too hard to use" would start to crack down like it has been since the past few
years. Until then, all we can do is keep on educating the following generations of new users, while giving them a little support so they can
walk on their own later, like we do with a toddler, and I believe this was one way I found to help with this re-education. Once people start
coming more towards Linux, among other things they *will* miss their GameSave Manager for sure (I was missing it myself, hence this
project was born), and having a native alternative that acts in a similar way (but probably improved) will weaken their resistance to change,
at least gaming-wise.

* **Why "Swiss Knife"?**

Internal joke. That was the best way I found to describe what I wanted while in the planning phase - a project that has everything I need:
information, automation and ease of use, all in a single package, like a swiss knife - so I decided to put that nickname on it. Fun fact, I
actually borrowed a swiss knife from my dad so I could sketch the icons :)

## How to contribute

See [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

## Special thanks

* [/r/linux_gaming](https://www.reddit.com/r/linux_gaming) (for being such an awesome and supportive community)

## Donations

Wanna buy me a beer? Donations are welcome:

* Bitcoin (BTC): 1MKwZku5DixVCuidtitSTYAJkykpHEhTLk
* Bitcoin Cash (BCH): 1P71dzRC6ch7n4QvVAKaFYwTXNRZW4hzPb
* Decred (DCR): DskAzj4ANSH85PDwgBhBUnYkRcHmafXGTdn
* Ethereum Classic (ETC): 0x7829205e7798A351f70Ff642CB732f58015F1941
* Ethereum (ETH): 0xC1a41a4e9e04052AfC351DA5751283cA725454EF
* Litecoin (LTC): Li6swpBAi3fr6qYamNhWUnk4pJH2Q94YYZ

Using a currency that's not listed here? Please let me know! I'll be glad to add it here if possible :)

## License / Copyright

**Copyright (C) 2017 Supremist (aka supremesonicbrazil)**

Reddit: [/u/TheSupremist](https://www.reddit.com/user/TheSupremist/)

Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the accompanying [COPYING](COPYING) file for more details.