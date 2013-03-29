# Overview #
Use shell commands directly from nodejs, nice and synchronous, so you
can build easy command line scripts in node much like in Ruby.

# Sample #
So much like Ruby in fact, that here are the samples in coffeescript
first.
```coffeescript
require('shellscript').globalize()
shell "ls ."
```

Or if you us just plain Javascript
```javascript
require('shellscript').globalize()
shell("ls .");
```

And of course, you can subshell and capture output
```coffeescript
require('shellscript').globalize()
stuff = $("ls .")
```

# Functions #

## shell(command) ##
This just runs a command using your current `$SHELL` or `/bin/sh` if not
set. It pipes stdout and stderr into the parent, and blocks until
complete. This is the workhorse _make a shell script_ function.
