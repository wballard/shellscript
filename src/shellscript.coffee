###
High level, synchronous interface to let you build just plain shell scripts.
###
#
sheller = require '../build/Release/sheller'
module.exports = {}

###
Register all our functions as globals, this makes it a lot like other languages
you may know and love.
###
#
module.exports.globalize = ->
        global.shell = module.exports.shell
        global.$ = module.exports.$
        global.run = module.exports.run

###
The classic, run any shell command line, using your current shell. This is
essentially `$SHELL -c <command>`. Streams stdout and stdin are connected
with your parent process, running out the command line. This blocks until
the shelled command is done, then returns the exit code.
###

module.exports.shell = (command, exitOnError) ->
        ret = sheller.shell command
        if exitOnError and ret.exitCode
            console.error "#{command}"
            process.exit ret.exitCode
        ret

###
$, the subshell command, returns stdout as a string. This is useful for string
interpolation.
###

module.exports.$ = (command, args...) ->
        if args.length
            ret = sheller.shell command, true, args
        else
            ret = sheller.shell command, true
        if ret.exitCode
            ''
        else
            ret.stdout

