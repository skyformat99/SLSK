### TASKS

* Move paths in database to a text file so it's easier to maintain (and also not obligatory to actually download SLSK just for taking a quick look)
* Generate the database *during* installation instead of bundling it in the repo (using the text file above)
* Fix INSTALL.sh according to issue #2
* $STEAMAPPS path is hardcoded, it shouldn't (issue #3)

### BUGS

* MainMenu::CheckTotalSize(): Total size of operation is not completely accurate (may miss by a margin of a couple MB from what was tested)

### SUGGESTIONS

* Filter list - possible methods:
  * Using Steam's own library categories (libraryfolders.vdf) - easier, no changing in database, but not sure if it's possible
  * Fixed filters (by size, genre, publisher/developer, year, name, etc.) - would require changing database structure
* Clean up if backup process is manually cancelled (restore might be way more complicated)
* Find a way to optionally compress content (which format?)
* Prep up a CLI version if demand skyrockets
