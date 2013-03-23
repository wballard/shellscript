//this is a forwarding proxy, deals with the trickiness of forwarding
//methods that already exist, including the built in Object methods
//which you don't actually want to proxy
module.exports = function (target, onGet){
    var traps = {
        getOwnPropertyDescriptor: Object.getOwnPropertyDescriptor.bind(null, target),
        getOwnPropertyNames: Object.getOwnPropertyNames.bind(null, target),
        keys: Object.keys.bind(null, target),
        defineProperty: Object.defineProperty.bind(null, target),
        get: function(r,k){ return target[k] },
        set: function(r,k,v){ target[k] = v; return true },
        has: function(k){ return k in target },
        hasOwn: function(k){ return {}.hasOwnProperty.call(target, k) },
        delete: function(k){ delete target[k]; return true },
        enumerate: function(){ var i=0,k=[]; for (k[i++] in target); return k }
    };

    return Proxy.create(Proxy.create({
        get: function(r, trap){
            return function(a, b){
                if (trap === 'get' && !target[b]) {
                    return onGet(b);
                } else {
                    return target[b];
                }
                if (trap in traps)
                    return traps[trap].apply(target, arguments);
            }
        }
    }), Object.getPrototypeOf(target));
}
