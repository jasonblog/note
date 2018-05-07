import abc


class ExamPaper(object):
    __metaclass__ = abc.ABCMeta

    def question1(self):
        print "question1: " + self.answer1()

    @abc.abstractmethod
    def answer1(self):
        return ""

    def question2(self):
        print "question2: " + self.answer2()

    @abc.abstractmethod
    def answer2(self):
        return ""

    def question3(self):
        print "question3: " + self.answer3()

    @abc.abstractmethod
    def answer3(self):
        return ""


class ExamPaperA(ExamPaper):
    def answer1(self):
        return "a"

    def answer2(self):
        return "a"

    def answer3(self):
        return "b"


class ExamPaperB(ExamPaper):
    def answer1(self):
        return "b"

    def answer2(self):
        return "b"

    def answer3(self):
        return "c"


if __name__ == "__main__":
    test_paper = ExamPaperA()
    print "Paper A:"
    test_paper.question1()
    test_paper.question2()
    test_paper.question3()

    print "Paper B:"
    test_paper = ExamPaperB()
    test_paper.question1()
    test_paper.question2()
    test_paper.question3()
