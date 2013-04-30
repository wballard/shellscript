/*
High level, synchronous interface to let you build just plain shell scripts.
*/


(function() {
  var $, globalize, shell, sheller, subshell,
    __slice = [].slice;

  sheller = require('../build/Release/sheller');

  /*
  Register all our functions as globals, this makes it a lot like other languages
  you may know and love.
  */


  globalize = function() {
    global.shell = shell;
    return global.$ = subshell;
  };

  /*
  The classic, run any shell command line, using your current shell. This is
  essentially `$SHELL -c <command>`. Streams stdout and stdin are connected
  with your parent process, running out the command line. This blocks until
  the shelled command is done, then returns the exit code.
  */


  shell = function(command, exitOnError) {
    var ret;

    ret = sheller.shell(command);
    if (exitOnError && ret.exitCode) {
      console.error("" + command);
      process.exit(ret.exitCode);
    }
    return ret;
  };

  /*
  $, the subshell command, returns stdout as a string. This is useful for string
  interpolation.
  */


  subshell = $ = function() {
    var args, command, ret;

    command = arguments[0], args = 2 <= arguments.length ? __slice.call(arguments, 1) : [];
    if (args.length) {
      ret = sheller.shell(command, true, args);
    } else {
      ret = sheller.shell(command, true);
    }
    if (ret.exitCode) {
      return '';
    } else {
      return ret.stdout;
    }
  };

  /*
  All defined, now export.
  */


  module.exports = {
    shell: shell,
    $: subshell,
    globalize: globalize
  };

}).call(this);
