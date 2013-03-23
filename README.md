# Overview #
Use shell commands directly from nodejs, just as if they were functions.

# Sample #
```coffeescript
ls '.'
```

Or if you us just plain Javascript
```javascript
ls('.');
```

And of course, you can subshell and capture output
```coffeescript
stuff = $(ls '.')
console.log stuff
```

And for even more fun, instead of `ls`, you can use any old shell
command you like.

# How it Works #
Using Harmony proxies, and intercepting the global namespace, bare words
like `ls` above are translated from a syntax error to a shell command
function, which is then called with the subsequent arguments via
`child_process`.

To make this as much like a shell as possible, commands are executed
synchronously. You are always welcome to use `child_process` for
asynchronous invocation as needed. This was inspired by
[shelljs](https://github.com/arturadib/shelljs) except that I wanted to
use any shell command, any time. So, maybe a bit more inspired by
[sh](http://amoffat.github.com/sh/).

# Important #
You need to turn on your harmony options:
```
coffee --nodejs --harmony script.coffee
node --harmony script.js
```

You can put this in your shebang
