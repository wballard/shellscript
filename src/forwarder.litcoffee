Tthis is a forwarding proxy, deals with the trickiness of forwarding
methods that already exist, including the built in Object methods
which you don't actually want to proxy.

    module.exports = (target, onGet) ->

Default traps, these hand along to `Object`.

        traps =
            getOwnPropertyDescriptor: Object.getOwnPropertyDescriptor.bind(null, target)
            getOwnPropertyNames: Object.getOwnPropertyNames.bind(null, target)
            keys: Object.keys.bind(null, target)
            defineProperty: Object.defineProperty.bind(null, target)
            get: (r,k) -> return target[k]
            set: (r,k,v) ->
                target[k] = v
                true
            has: (k) -> return k in target
            hasOwn: (k) -> {}.hasOwnProperty.call(target, k)
            delete: (k) ->
                delete target[k];
                true
            enumerate: `function(){ var i=0,k=[]; for (k[i++] in target); return k }`

Why? well, you need to be able to get at methods on 'target' that are
plenty well already defined, but we want to get at the prototype for
methods that aren't defined

        Proxy.create(Proxy.create(
            get: (r, trap) ->
                (a, b) ->
                    if trap is 'get' and !target[b]
                        return onGet b
                    else
                        return target[b]
                    if trap in traps
                        return traps[trap].apply(target, arguments)
        ), Object.getPrototypeOf(target))
