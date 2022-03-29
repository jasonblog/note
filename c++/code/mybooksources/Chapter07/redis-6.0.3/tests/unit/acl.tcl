start_server {tags {"acl"}} {
    test {Connections start with the default user} {
        r ACL WHOAMI
    } {default}

    test {It is possible to create new users} {
        r ACL setuser newuser
    }

    test {New users start disabled} {
        r ACL setuser newuser >passwd1
        catch {r AUTH newuser passwd1} err
        set err
    } {*WRONGPASS*}

    test {Enabling the user allows the login} {
        r ACL setuser newuser on +acl
        r AUTH newuser passwd1
        r ACL WHOAMI
    } {newuser}

    test {Only the set of correct passwords work} {
        r ACL setuser newuser >passwd2
        catch {r AUTH newuser passwd1} e
        assert {$e eq "OK"}
        catch {r AUTH newuser passwd2} e
        assert {$e eq "OK"}
        catch {r AUTH newuser passwd3} e
        set e
    } {*WRONGPASS*}

    test {It is possible to remove passwords from the set of valid ones} {
        r ACL setuser newuser <passwd1
        catch {r AUTH newuser passwd1} e
        set e
    } {*WRONGPASS*}

    test {Test password hashes can be added} {
        r ACL setuser newuser #34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4e6
        catch {r AUTH newuser passwd4} e
        assert {$e eq "OK"}
    }

    test {Test password hashes validate input} {
        # Validate Length
        catch {r ACL setuser newuser #34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4e} e
        # Validate character outside set
        catch {r ACL setuser newuser #34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4eq} e
        set e
    } {*Error in ACL SETUSER modifier*}

    test {ACL GETUSER returns the password hash instead of the actual password} {
        set passstr [dict get [r ACL getuser newuser] passwords]
        assert_match {*34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4e6*} $passstr
        assert_no_match {*passwd4*} $passstr
    }

    test {Test hashed passwords removal} {
        r ACL setuser newuser !34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4e6
        set passstr [dict get [r ACL getuser newuser] passwords]
        assert_no_match {*34344e4d60c2b6d639b7bd22e18f2b0b91bc34bf0ac5f9952744435093cfb4e6*} $passstr
    }

    test {By default users are not able to access any command} {
        catch {r SET foo bar} e
        set e
    } {*NOPERM*}

    test {By default users are not able to access any key} {
        r ACL setuser newuser +set
        catch {r SET foo bar} e
        set e
    } {*NOPERM*key*}

    test {It's possible to allow the access of a subset of keys} {
        r ACL setuser newuser allcommands ~foo:* ~bar:*
        r SET foo:1 a
        r SET bar:2 b
        catch {r SET zap:3 c} e
        r ACL setuser newuser allkeys; # Undo keys ACL
        set e
    } {*NOPERM*key*}

    test {Users can be configured to authenticate with any password} {
        r ACL setuser newuser nopass
        r AUTH newuser zipzapblabla
    } {OK}

    test {ACLs can exclude single commands} {
        r ACL setuser newuser -ping
        r INCR mycounter ; # Should not raise an error
        catch {r PING} e
        set e
    } {*NOPERM*}

    test {ACLs can include or exclude whole classes of commands} {
        r ACL setuser newuser -@all +@set +acl
        r SADD myset a b c; # Should not raise an error
        r ACL setuser newuser +@all -@string
        r SADD myset a b c; # Again should not raise an error
        # String commands instead should raise an error
        catch {r SET foo bar} e
        r ACL setuser newuser allcommands; # Undo commands ACL
        set e
    } {*NOPERM*}

    test {ACLs can include single subcommands} {
        r ACL setuser newuser +@all -client
        r ACL setuser newuser +client|id +client|setname
        r CLIENT ID; # Should not fail
        r CLIENT SETNAME foo ; # Should not fail
        catch {r CLIENT KILL type master} e
        set e
    } {*NOPERM*}

    # Note that the order of the generated ACL rules is not stable in Redis
    # so we need to match the different parts and not as a whole string.
    test {ACL GETUSER is able to translate back command permissions} {
        # Subtractive
        r ACL setuser newuser reset +@all ~* -@string +incr -debug +debug|digest
        set cmdstr [dict get [r ACL getuser newuser] commands]
        assert_match {*+@all*} $cmdstr
        assert_match {*-@string*} $cmdstr
        assert_match {*+incr*} $cmdstr
        assert_match {*-debug +debug|digest**} $cmdstr

        # Additive
        r ACL setuser newuser reset +@string -incr +acl +debug|digest +debug|segfault
        set cmdstr [dict get [r ACL getuser newuser] commands]
        assert_match {*-@all*} $cmdstr
        assert_match {*+@string*} $cmdstr
        assert_match {*-incr*} $cmdstr
        assert_match {*+debug|digest*} $cmdstr
        assert_match {*+debug|segfault*} $cmdstr
        assert_match {*+acl*} $cmdstr
    }

    test {ACL #5998 regression: memory leaks adding / removing subcommands} {
        r AUTH default ""
        r ACL setuser newuser reset -debug +debug|a +debug|b +debug|c
        r ACL setuser newuser -debug
        # The test framework will detect a leak if any.
    }

    test {ACL LOG shows failed command executions at toplevel} {
        r ACL LOG RESET
        r ACL setuser antirez >foo on +set ~object:1234
        r ACL setuser antirez +eval +multi +exec
        r AUTH antirez foo
        catch {r GET foo}
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry username] eq {antirez}}
        assert {[dict get $entry context] eq {toplevel}}
        assert {[dict get $entry reason] eq {command}}
        assert {[dict get $entry object] eq {get}}
    }

    test {ACL LOG is able to test similar events} {
        r AUTH antirez foo
        catch {r GET foo}
        catch {r GET foo}
        catch {r GET foo}
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry count] == 4}
    }

    test {ACL LOG is able to log keys access violations and key name} {
        r AUTH antirez foo
        catch {r SET somekeynotallowed 1234}
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry reason] eq {key}}
        assert {[dict get $entry object] eq {somekeynotallowed}}
    }

    test {ACL LOG RESET is able to flush the entries in the log} {
        r ACL LOG RESET
        assert {[llength [r ACL LOG]] == 0}
    }

    test {ACL LOG can distinguish the transaction context (1)} {
        r AUTH antirez foo
        r MULTI
        catch {r INCR foo}
        catch {r EXEC}
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry context] eq {multi}}
        assert {[dict get $entry object] eq {incr}}
    }

    test {ACL LOG can distinguish the transaction context (2)} {
        set rd1 [redis_deferring_client]
        r ACL SETUSER antirez +incr

        r AUTH antirez foo
        r MULTI
        r INCR object:1234
        $rd1 ACL SETUSER antirez -incr
        $rd1 read
        catch {r EXEC}
        $rd1 close
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry context] eq {multi}}
        assert {[dict get $entry object] eq {incr}}
        r ACL SETUSER antirez -incr
    }

    test {ACL can log errors in the context of Lua scripting} {
        r AUTH antirez foo
        catch {r EVAL {redis.call('incr','foo')} 0}
        r AUTH default ""
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry context] eq {lua}}
        assert {[dict get $entry object] eq {incr}}
    }

    test {ACL LOG can accept a numerical argument to show less entries} {
        r AUTH antirez foo
        catch {r INCR foo}
        catch {r INCR foo}
        catch {r INCR foo}
        catch {r INCR foo}
        r AUTH default ""
        assert {[llength [r ACL LOG]] > 1}
        assert {[llength [r ACL LOG 2]] == 2}
    }

    test {ACL LOG can log failed auth attempts} {
        catch {r AUTH antirez wrong-password}
        set entry [lindex [r ACL LOG] 0]
        assert {[dict get $entry context] eq {toplevel}}
        assert {[dict get $entry reason] eq {auth}}
        assert {[dict get $entry object] eq {AUTH}}
        assert {[dict get $entry username] eq {antirez}}
    }

    test {ACL LOG entries are limited to a maximum amount} {
        r ACL LOG RESET
        r CONFIG SET acllog-max-len 5
        r AUTH antirez foo
        for {set j 0} {$j < 10} {incr j} {
            catch {r SET obj:$j 123}
        }
        r AUTH default ""
        assert {[llength [r ACL LOG]] == 5}
    }

    test {When default user is off, new connections are not authenticated} {
        r ACL setuser default off
        catch {set rd1 [redis_deferring_client]} e
        r ACL setuser default on
        set e
    } {*NOAUTH*}
}
