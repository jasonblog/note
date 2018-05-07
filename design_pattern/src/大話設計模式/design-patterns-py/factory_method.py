import abc


class Leifeng:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def wash(self):
        """"wash"""

    @abc.abstractmethod
    def sweep(self):
        """sweep"""

    @abc.abstractmethod
    def buy_rice(self):
        """buy rice"""


class Undergraduate(Leifeng):
    def wash(self):
        print "undergraduate wash"

    def sweep(self):
        print "undergraduate sweep"

    def buy_rice(self):
        print "undergraduate buy rice"


class Volunteer(Leifeng):
    def wash(self):
        print "volunteer wash"

    def sweep(self):
        print "volunteer sweep"

    def buy_rice(self):
        print "volunteer buy rice"


class IFactory:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def CreateLeifeng(self):
        """create class leifeng"""


class UndergraduateFactory(IFactory):
    def CreateLeifeng(self):
        return Undergraduate()


class VolunteerFactory(IFactory):
    def CreateLeifeng(self):
        return Volunteer()


if __name__ == "__main__":
    # create undergraduate to sweep
    i_factory = UndergraduateFactory()
    leifeng = i_factory.CreateLeifeng()
    leifeng.sweep()

    # create volunteer to wash
    i_factory = VolunteerFactory()  # just replace UndergraduateFactory with VolunteerFactory
    leifeng = i_factory.CreateLeifeng()
    leifeng.wash()
