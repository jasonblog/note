class Stock1:
    def buy(self):
        print "buy stock1"

    def sell(self):
        print "sell stock1"


class Stock2:
    def buy(self):
        print "buy stock2"

    def sell(self):
        print "sell stock2"


class Reality1:
    def buy(self):
        print "buy reality1"

    def sell(self):
        print "sell reality1"


class Fund:
    def __init__(self):
        self.stock1 = Stock1()
        self.stock2 = Stock2()
        self.reality1 = Reality1()

    def buy(self):
        self.stock1.buy()
        self.stock2.buy()
        self.reality1.buy()

    def sell(self):
        self.stock1.sell()
        self.stock2.sell()
        self.reality1.sell()


if __name__ == "__main__":
    fund = Fund()
    fund.buy()
    fund.sell()
