import abc


class Player:
    __metaclass__ = abc.ABCMeta

    def __init__(self, n):
        self.name = n

    @abc.abstractmethod
    def attack(self):
        """attack"""

    @abc.abstractmethod
    def defense(self):
        """defense"""


class Center(Player):
    def attack(self):
        print "center " + self.name + " attack"

    def defense(self):
        print "center " + self.name + " defense"


class ForeignCenter:
    def __init__(self, n):
        self.name = n

    def gong(self):
        print "foreign center " + self.name + " attack"

    def shou(self):
        print "foreign center " + self.name + " defense"


class Translator(Player):
    def __init__(self, n):
        Player.__init__(self, n)
        self.foreign_center = ForeignCenter(n)

    def attack(self):
        self.foreign_center.gong()

    def defense(self):
        self.foreign_center.shou()


if __name__ == "__main__":
    # center attack/defense
    center = Center("Russell")
    center.attack()
    center.defense()

    # foreign center gong/shou through translator's attack/defense
    translator = Translator("YaoMing")
    translator.attack()
    translator.defense()
