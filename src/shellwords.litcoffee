Turn a bare symbol into a process invocation by calling it as a shell program.

    shelljs = require 'shelljs'
    shelljs.config.silent = true
    script_output = []

    module.exports = (command) ->

Keep track of all the script output, we'll use this to sweep up subshells.

        flush = ->
            for out in script_output
                process.stdout.write out
            script_output = []

Any output that we're left with on exit needs to be printed.

        process.on 'exit', flush

In some sense it is a shame that shell uses $ as a subshell, and that jquery
uses $ too. But oh well, here is the magical definition of $ that makes a
subshell.

        if command is '$'
            () ->

In a subshell, we turn all the arguments into text output, and consume them
off the output buffer.

                ret = []
                for _, out of arguments
                    ret.push out
                    script_output.pop()
                flush()
                ret.join ''

Using shelljs, it had the basics of the functionality, this library just adds
on the bare shell words capability.

        else
            () ->
                bits = [command]
                for _, arg of arguments
                    bits.push arg
                results = shelljs.exec bits.join ' '

Capture the shell variables, can't do a bare $?, but this is close.

                global['$?'] = results.code
                if Array.isArray results.output
                    ret = results.output.join ''
                else
                    ret = results.output

Buffer the output, this will either get consumed by a subshell, or be left
to print on exit.

                script_output.push ret
                ret

