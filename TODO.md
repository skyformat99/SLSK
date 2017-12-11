### BUGS

* MainMenu::CheckTotalSize(): Total size of operation is not completely accurate (may miss by a margin of a couple MB from what was tested)

### SUGGESTIONS

* Filter list - possible methods:
  * Using Steam's own library categories (libraryfolders.vcf?) - easier, no changing in database, but not sure if it's possible
  * Fixed filters (by size, genre, publisher/developer, year, name, etc.) - would require changing database structure
* Clean up if backup process is manually cancelled (restore might be way more complicated)
* Find a way to optionally compress content (which format?)
* Prep up a CLI version if demand skyrockets
* Make a simple text file with all the paths in the database so people don't see SLSK as a mandatory install if they only want the paths