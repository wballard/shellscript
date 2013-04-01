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

And pipelines...
```coffeescript
require('shellscript').globalize()
shell "ls . | grep lib"
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

## globalize() ##
Puts shellscript's functions on global, so you can call them without
an intermediate variable or namespace, useful to make this feel a lot
more like other languages with built in synchronous shelling.

## shell(command, exitOnError) ##
This just runs a command using your current `$SHELL` or `/bin/sh` if not
set. It pipes stdout and stderr into the parent, and blocks until
complete. This is the workhorse _make a shell script_ function.

If `exitOnError` is true, this will exit the process for any non 0 exit
code. Handy if you just want to exit and avoid error checking the return
object.

### Returns ###
An object with properties:

* pid: pid of the shelled process, now exited
* exitCode: exit code of the shelled process
* signalCode: signal on exit of the shelled process
* stdout: standard out, captured as a string
* stderr: standard error, captured as a string

## $(command) ##
Much like `shell` above, but instead of returning process information,
returns the string that is stdout, or empty for a non-zero exit code.

## $(command, args...) ##
In this version, if you pass args, either as a CoffeeScript splat or
JavaScript array, the command will be invoked directly without a
subshell. This is a tiny bit faster, and sometimes what you need if you
don't want to run a shell pipeline.
