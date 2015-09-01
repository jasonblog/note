#!/usr/bin/python
import gdb

INDENT = '  '


class SuperTrace(gdb.Command):
    old_stack = []

    def __init__(self):
        super(SuperTrace, self).__init__("supertrace",
                                         gdb.COMMAND_SUPPORT,
                                         gdb.COMPLETE_NONE)

    def supertrace(self):
        def backtrace_generator():
            f = gdb.newest_frame()
            while f is not None:
                yield f
                f = gdb.Frame.older(f)

        fstack = []
        f = gdb.newest_frame()
        for f in backtrace_generator():
            frame_name = gdb.Frame.name(f)
            if frame_name is None:
                continue
                #frame_name = '??'
            filename = '??'
            line = 0
            try:
                symtab_and_line = gdb.Frame.find_sal(f)
                filename = symtab_and_line.symtab.filename
                line = symtab_and_line.line
            except:
                #continue
                pass
            args = [
                frame_name,
                filename,
                line,
            ]
            fstack.append(args)
        fstack.reverse()
        ostack = self.__class__.old_stack
        is_print = False
        for f in enumerate(fstack):
            if f[0] >= len(ostack):
                is_print = True
            else:
                if f[1] != ostack[f[0]]:
                    is_print = True
            if is_print:
                print '{}| {} at {}:{}'.format(
                      INDENT * f[0], f[1][0], f[1][1], f[1][2])
        self.__class__.old_stack = fstack

    def invoke(self, arg, from_tty):
        #num = 0
        #try:
        #    num = int(arg)
        #except Exception:
        #    pass
        try:
            self.supertrace()
        except Exception as e:
            print str(e)


class LastBreakpoints(gdb.Function):
    def __init__(self):
        super(LastBreakpoints, self).__init__("lastbp")

    def invoke(self):
        return len(gdb.breakpoints())


class BreakAll(gdb.Command):
    def __init__(self):
        super(self.__class__, self).__init__("breakall",
                                             gdb.COMMAND_SUPPORT,
                                             gdb.COMPLETE_NONE)

    def filelist(self):
        files = set()
        raw = gdb.execute('info functions', True, True)
        lines = raw.split('\n')
        for line in enumerate(lines):
            if line[1].startswith('File '):
                files.add(line[1][5:line[1].find(':')])
            #print '{0:3d}: {1}'.format(line[0], line[1])
        return files

    def invoke(self, arg, from_tty):
        for f in self.filelist():
            gdb.execute('rbreak {}:.'.format(f))


SuperTrace()
LastBreakpoints()
BreakAll()
