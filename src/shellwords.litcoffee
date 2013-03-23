Turn a bare symbol into a process invocation by calling it as a shell program.

    shelljs = require 'shelljs'
    shelljs.config.silent = true

    module.exports = (command) ->


In some sense it is a shame that shell uses $ as a subshell, and that jquery
uses $ too. But oh well, here is the magical definition of $ that makes a
subshell.

        inSubshell = false

        if command is '$'
            () ->
                inSubshell++
                inSubshell--
                arguments[0]

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
                    results.output.join ''
                else
                    results.output

