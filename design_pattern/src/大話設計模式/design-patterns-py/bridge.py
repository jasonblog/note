import abc


class HandsetSoft:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def run(self):
        """soft run"""


class HandsetGame(HandsetSoft):
    def run(self):
        print "run game"


class HandsetAddressList(HandsetSoft):
    def run(self):
        print "run address list"


class HandsetBrand:
    __metaclass__ = abc.ABCMeta

    def __init__(self, s):
        self.soft = s

    @abc.abstractmethod
    def run(self):
        """run"""


class HandsetBrandM(HandsetBrand):
    def run(self):
        print "handset brand M : ",
        self.soft.run()


class HandsetBrandN(HandsetBrand):
    def run(self):
        print "handset brand N : ",
        self.soft.run()


if __name__ == "__main__":
    handset_brand_m = HandsetBrandM(HandsetGame())
    handset_brand_m.run()
    handset_brand_m = HandsetBrandM(HandsetAddressList())
    handset_brand_m.run()

    handset_brand_n = HandsetBrandN(HandsetGame())
    handset_brand_n.run()
    handset_brand_n = HandsetBrandN(HandsetAddressList())
    handset_brand_n.run()
