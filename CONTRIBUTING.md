# Contributing to SLSK

First of all, thank you very much for considering contributing to SLSK, your help makes all the difference :)

# Table of Contents

[A few words of comfort](#a-few-words-of-comfort)

[So how do I contribute?](#so-how-do-i-contribute)

* [Documentation](#documentation)

* [Improving code / UI](#improving-code--ui)

* [Improving the database](#improving-the-database)

* [Guidelines for adding entries](#guidelines-for-adding-entries)

* [Changelog and Versioning](#changelog-and-versioning)

[Closing words](#closing-words)

# A few words of comfort

*Make yourself at home.* Even though this might be considered a small hobby project, you are invited to take part of it if you so desire.

*Any help is welcome.* Whether it be a shoutout for a bug you just found or a mass addition of entries in the database, I appreciate your
time and effort, and again, thank you very much for your help.

And of course, the usual: *be friendly and polite*, but don't feel afraid to share your opinions if you need to, and, most importantly,
*don't be ashamed to ask for help*. Who knows, maybe your neighbor Github user has actually found that pesky save path you spent days
searching for ;)

# So how do I contribute?

First of all, make sure you have installed the [dependencies required](README.md#getting-started) for this project.

In short, there are some ways you can help contributing to this project, which will be explained below. Contributing is not limited to these only,
but they are essentially the main ones.

## Documentation

I'll be honest here: in the beginning, *I made SLSK with little to no planning. At all.* That also meant little to no documentation, aside from
commenting the code. I admit I'm to blame, at least partially. Whilist coding it, I thought "meh, it's such a small project, maybe it'll stay
simple like this, just add a thing here and there...". Heh, and then it became a monster. So this, along with the fact I was in the middle of
learning C++ and software engineering in college, while learning Qt at home and building SLSK, *all from scratch and at the same time*,
kinda left me without a solid ground and made me keep taking shots in the dark.

That said. This is literally the first program I've ever built and finished, so I'm paying the toll. I'll be glad to help with (and accept help to)
properly document this program if it is really needed even for a small project like  this one, according to the community's needs. All I need
is a little guidance because I'm a little clueless on *what* exactly to document. I'll be sure to give my best during my free time between college
and other things I have to take care of. To give you a head start, it's all commented in the code. If you have any doubts, don't be afraid to ask.

## Improving code / UI

Ah, now we're getting to the fun part. Both the code and the UI were made using the Qt Creator IDE. Using it is not obligatory, but highly
recommended so we can keep a standard.

* About the code:
  * Please try to keep the code *well commented*. It doesn't need to be a car manual, but let us know what your code does so documenting
gets easier (especially when you manage to reduce parts of the code)
  * Also, try to keep the code *well formatted*. It's OK if it gets kinda big, as long as it's functional and stable enough so it doesn't crash the
program, we can always reduce it later
  * Still regarding the point above, reduce code as you like but *don't reduce it too much* so it gets unreadable - I know we as programmers
are always itching to do things like [this](https://en.wikipedia.org/wiki/Minification_(programming\)#Example), but remember that
readability counts a lot, so reduce with moderation
* About the UI:
  * If you feel the UI needs a tweak here and there, let us know first so we can discuss about it (bring your mockups / screenshots as well!)
  * You may tweak the UI in whichever way you want, but please try to make it clean and minimalistic. Remember, *less is more*
  * Also, try to make it easy for the eyes - not wanting to start a flame war between light/dark themes here, but please consider those poor
folks who are trying to backup their stuff in their pitch black rooms at 1:00 AM (not me, but you get the drill) :(

If you don't feel like coding or tweaking the UI, it's all fine - you can add your ideas to the [TODO](TODO.md) list, which has a section for optional
suggestions.

## Improving the database

An SQLite database is included with the program. This is the most important part of the whole project, without it we just have a pretty but
useless shell made in Qt. You can use CLI tools to tweak the database if you feel more comfortable that way, but using a GUI editor like
`sqlitebrowser` is also highly recommended.

Each registered Linux Steam game in the database (from now on referred as an "entry") has the following columns:

* **AppID (PRIMARY KEY)** - The game's AppID on Steam (http://store.steampowered.com/app/`AppID-goes-here`)
* **SteamName** - The game's name as it appears on the Steam store
* **GameFolder** - The game folder's name (exactly how it is named inside `steamapps/common`)
* **SavePath1** - The full path of the folder where the game stores its save files
* **SaveFolder1** - The name of the folder where the game stores its save files (last folder in the path - after the last "/")
* **ConfigPath1** - The full path of the folder where the game stores its configurations
* **ConfigFolder1** - The name of the folder where the game stores its configurations (last folder in the path - after the last "/")
* **SavePath2** - A second (optional) path for the save folder, if the game has more than one save location
* **SaveFolder2** - A second (optional) save folder
* **ConfigPath2** - A second (optional) path for the config folder, if the game has more than one config location
* **ConfigFolder2** - A second (optional) config folder
* **SavePath3** - A third (optional) path for the save folder
* **SaveFolder3** - A third (optional) save folder
* **ConfigPath3** - A third (optional) path for the config folder
* **ConfigFolder3** - A third (optional) config folder

Adding an entry to the database requires a few guidelines which will help both the program on filtering and working in general, and
whoever wants to take a quick look at the database to know where there is missing information.

### Guidelines for adding entries

#### 1. **Field labels**

All fields except for AppID, SteamName and GameFolder **must** make use of the following labels when applicable:

* **$STEAMUSER** - shortcut to `~/.local/share/Steam/userdata/7656*` *(including the wildcard)*
* **$STEAMAPPS** - shortcut to `~/.local/share/Steam/steamapps/common/<GameFolder>`
* **[N/A]** - game naturally doesn't have saves/configs
* **[CLOUD-ONLY]** - saves/configs only exist in the cloud or in the game's server, out of reach
* **[UNKNOWN]** - saves/configs are in an unknown location

So, giving a few examples on when to use each one of these labels:

* Some games like Saints Row The Third, Bioshock Infinite and PAYDAY 2 store their saves inside a folder with the user's STEAMID,
which is inside the Steam folder (like `~/.local/share/Steam/userdata/7656*/<AppID>`), so the SavePath/ConfigPath is written
like **$STEAMUSER/`<AppID>`**
* Half-Life 2 has its saves stored in `~/.local/share/Steam/steamapps/common/Half-Life 2/hl2/save`, so it's SavePath in the database
is written like **$STEAMAPPS/hl2/save**
* CS:GO has no save files at all due to its multiplayer-only nature, but has config files in `$STEAMAPPS/csgo/cfg`, so its ConfigPath
stays like that, but its SavePath is labeled **[N/A]** (this also applies to the second and third SavePath/ConfigPath fields, when there's
no second or third path for saves/configs)
* Killing Floor has its save games hosted exclusively in the Steam Cloud and thus unreachable from our computers, so its SavePath
is labeled **[CLOUD-ONLY]**, while its config files are in `~/.killingfloor`
* The idea is that all games store their saves and configs somewhere, no matter where. *But*, sometimes it's really hard to find those
paths, and that's where the **[UNKNOWN]** label comes in. This will be explained further below

#### 2. **Path structures**

Game saves and configs are usually structured in one of the following ways (not definitive, but these patterns were sure detected
when searching locally for paths):

* **Case 1** - saves/configs consist of *a single file* (e.g. `game/save.dat` - Crypt of the NecroDancer, Knights of Pen and Paper, etc)
* **Case 2** - saves/configs consist of *multiple files with the same extension* (e.g. `game/*.cfg` - System Shock 2)
* **Case 3** - saves/configs consist of *a single folder with lots of different files* (e.g. `game/save` - most common case for the majority
of games)
* **Case 4** - saves/configs consist of *multiple folders*, like one folder per save slot (e.g. `game/save_*` - again, System Shock 2)

That said, depending on how the SavePath/ConfigPath ends, the SaveFolder/ConfigFolder field(s) will change.
If the SavePath/ConfigPath ends in:

* *A single file or multiple files (Cases 1 and 2)* - put the **parent folder** of the path (in both cases, `game`)
* *A single folder or multiple folders (Cases 3 and 4)* - put the **last folder** of the path (`save` and `save_*`, respectively)

**NOTE:** if the config files happen to be *in the same folder as the save files*, or vice-versa, copy-paste the same path and folder for both.
Yes, this means duplicates may occur, but that's intentional. Also, *please* use `~` to indicate the user's home folder, people recognize it
easily and it's shorter than `$HOME`, plus the GUI tool needs it to do some work.

#### 3. [UNKNOWN] paths

**NOTE: this is exclusively for paths labeled as [UNKNOWN], it doesn't apply to games with paths labeled as [N/A] or [CLOUD-ONLY].**

If you want to add a game to the database but can't find its save/config paths, or you're not sure where the save/config files are located,
don't worry - we might find those paths sooner or later, but we need to know that you can't find them. That makes it easier to keep looking
for information about that game's paths (whether they actually exist or not). We keep a file called MISSINGLIST for games that meet
this criteria, kind of like a "wanted list" or "bounty list" if you like catchy names. Here's how to proceed if you end up in this situation:

1. Add the game normally to the database using the **[UNKNOWN]** label in the respective missing fields
2. Include the game in the CHANGELOG normally, but mark it as "(incomplete)", like this:
  * (incomplete) GameName
3. Write the game's name in the MISSINGLIST, specifying which paths are unknown - only save path, only config path, or both - like this:
  * GameName (save and config)
4. Once the paths are found, or if it's proven that there are no paths - **[N/A]** - or that the missing information is **[CLOUD-ONLY]**:
  * update the database accordingly
  * add the game in the CHANGELOG but without the "(incomplete)" before it (more details below in Changelog and Versioning)
  * remove the game's name from the MISSINGLIST

## Changelog and Versioning

Even though the main focus of the project is the database, there's also the GUI tool which may need improvements as time passes.
When contributing, versioning applies only to the *core program* (be it code or UI). Database updates do not count for versioning, but
they still appear on the Changelog. SLSK uses the standard [Semantic Versioning](https://semver.org/) - MAJOR.MINOR.PATCH, no big
surprises here. In short, there may be Core updates and Database updates. The former applies to the core program, but *can* also include
database updates (as seen below), while the latter applies *exclusively* to database changes.

Database entries can be commited directly to master branch, but changes to core program *must* go to a separate dev branch. Fix bugs and
add features as you may desire, adding tags in commit titles regarding whether you're fixing/adding things in UI, code or docs (respectively,
**[ui]**, **[code]** and **[docs]**), but test the dev branch to make sure it's stable enough before submitting a pull request, as this will be
considered a version increment.

### Example of changelog entry for database additions only:

#### YYYY-MM-DD - Database update
* X games added to the database:
  * this game
  * that other game
  * (incomplete) awesome game
  * ...

### Example of changelog entry for core program versioning:

#### YYYY-MM-DD - Core update [vX.Y.Z]
* Added this neat feature
* Fixed those pesky bugs
* X games added to the database:
  * not-so-awesome game
  * ...
* X games completed in the database
  * awesome game
  * ...

# Closing words

This is it for now. It's a really simple project so there's not much specific detail, except for the database entries. If you have any doubts, don't be
afraid and ask away so I can improve this file ;)