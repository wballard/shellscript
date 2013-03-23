Turn a bare symbol into a process invocation.

    module.exports = (command) ->
        console.log 'will execute', command
        () ->
