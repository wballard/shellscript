# Overview #
Use shell commands directly from nodejs, just as if they were functions.

# Sample #
```coffeescript
output = ls '*'
console.log output, $?, $$, $!
```

# How it Works #
Using Harmony proxies, an intercepting the global namespace, bare words
like `ls` above are translated from a syntax error to a shell command
function, which is then called with the subsequent arguments via
`child_process`.

To make this as much like a shell as possible, commands are executed
synchronously. You are always welcome to use `child_process` for
asynchronous invocation as needed. 

# Important #
You need to turn on your harmony options:
```
coffee --nodejs --harmony script.coffee
node --harmony script.js
```

You can put this in your shebang
