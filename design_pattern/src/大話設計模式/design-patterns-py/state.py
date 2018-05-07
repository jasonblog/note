import abc


class Work:
    def __init__(self, h, f):
        self.hour = h
        self.finished = f
        self.state = WorkingState()

    def set_state(self, s):
        self.state = s

    def write_program(self):
        self.state.write_program(self)


class State:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def write_program(self, w):
        """write program according to workstate"""


class RestState(State):
    def write_program(self, w):
        print str(w.hour) + " : return to rest"


class SleepingState(State):
    def write_program(self, w):
        print str(w.hour)+ " : sleeping"


class OvertimeState(State):
    def write_program(self, w):
        if w.finished is True:
            w.set_state(RestState())
            w.write_program()
        elif w.hour < 21:
            print str(w.hour) + " : overtime"
        else:
            w.set_state(SleepingState())
            w.write_program()


class WorkingState(State):
    def write_program(self, w):
        if w.hour < 17:
            print str(w.hour) + " : working"
        else:
            w.set_state(OvertimeState())
            w.write_program()


if __name__ == "__main__":
    work = Work(15, False)
    work.write_program()

    work = Work(20, False)
    work.write_program()

    work = Work(22, False)
    work.write_program()

    work = Work(20, True)
    work.write_program()