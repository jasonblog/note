proc rediscli_tls_config {testsdir} {
    set tlsdir [file join $testsdir tls]
    set cert [file join $tlsdir redis.crt]
    set key [file join $tlsdir redis.key]
    set cacert [file join $tlsdir ca.crt]

    if {$::tls} {
        return [list --tls --cert $cert --key $key --cacert $cacert]
    } else {
        return {}
    }
}

proc rediscli {port {opts {}}} {
    set cmd [list src/redis-cli -p $port]
    lappend cmd {*}[rediscli_tls_config "tests"]
    lappend cmd {*}$opts
    return $cmd
}
