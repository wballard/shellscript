//Pull in coffeescript so we can import coffee modules. I know this will
//'offend' some that thinking having coffee-script as a dependency is somehow
//over the top if you are just using JavaScript.
//
//Noted.
//
require("coffee-script");
global.__proto__ = require("./src/forwarder")(global.__proto__, require("./src/shellwords"));
