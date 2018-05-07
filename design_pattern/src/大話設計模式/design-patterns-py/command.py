import abc


class Barbecuer(object):
    def bake_mutton(self):
        print "bake mutton"

    def bake_chicken(self):
        print "bake chicken"


class Command(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self, r):
        self.receiver = r

    @abc.abstractmethod
    def execute_command(self):
        pass


class BakeMuttonCommand(Command):
    def execute_command(self):
        self.receiver.bake_mutton()


class BakeChickenCommand(Command):
    def execute_command(self):
        self.receiver.bake_chicken()


class Waiter(object):
    def __init__(self):
        self.commands = []

    def set_order(self, o):
        if isinstance(o, BakeChickenCommand):
            print "chicken sold out"
        else:
            print "add order: " + type(o).__name__
            self.commands.append(o)

    def cancel_order(self, o):
        print "cancel order: " + type(o).__name__
        self.commands.remove(o)

    def notify(self):
        for command in self.commands:
            command.execute_command()


if __name__ == "__main__":
    barbecuer = Barbecuer()
    bake_mutton_command = BakeMuttonCommand(barbecuer)
    bake_chicken_command = BakeChickenCommand(barbecuer)
    waiter = Waiter()
    waiter.set_order(bake_mutton_command)
    waiter.set_order(bake_mutton_command)
    waiter.set_order(bake_chicken_command)
    waiter.notify()