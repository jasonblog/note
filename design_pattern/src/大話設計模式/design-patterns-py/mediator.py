import abc


class Country(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self, un):
        self.un = un

    def declare(self, m):
        self.un.declare(self, m)

    @abc.abstractmethod
    def get_message(self, m):
        pass


class Usa(Country):
    def get_message(self, m):
        print "USA gets: \"" + m + "\""


class Iraq(Country):
    def get_message(self, m):
        print "Iraq gets: \"" + m + "\""


class UnitedNations:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def declare(self, c, m):
        pass


class SecurityCouncil(UnitedNations):
    def __init__(self):
        self.usa = None
        self.iraq = None

    def declare(self, c, m):
        if c == self.usa:
            self.iraq.get_message(m)
        elif c == self.iraq:
            self.usa.get_message(m)
        else:
            print "err: SecurityCouncil/declare"


if __name__ == "__main__":
    security_council = SecurityCouncil()
    usa = Usa(security_council)
    iraq = Iraq(security_council)
    security_council.usa = usa
    security_council.iraq = iraq
    usa.declare("Stop nuclear weapons")
    iraq.declare("No nuclear here")