This is a proxy that intercepts node.js global, so that any bare word you
type in a script gets a chance to do something other than generate a not
defined error.

    smartproxy = (target) ->

The tricky part of making a proxy is you need to not proxy the basic methods.

        traps:
            getOwnPropertyDescriptor: Object.getOwnPropertyDescriptor.bind(null, target)
            getOwnPropertyNames: Object.getOwnPropertyNames.bind(null, target)
            keys: Object.keys.bind(null, target)
            defineProperty: Object.defineProperty.bind(null, target)
            get: (r,k) -> target[k]
            set: (r,k,v) ->
                target[k] = v
                true
            has: (k) -> k in target
            hasOwn: (k) -> {}.hasOwnProperty.call(target, k)
            delete: (k) ->
                delete target[k]
                true
            enumerate: `function(){ var i=0,k=[]; for (k[i++] in target); return k }`

        Proxy.create Proxy.create
            get: (r, trap) ->
                console.log r, trap
        , Object.getPrototypeOf(target)

And now, hook this proxy over global...

    global.__proto__ = smartproxy global
