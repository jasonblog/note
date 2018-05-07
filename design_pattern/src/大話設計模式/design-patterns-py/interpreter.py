import abc


class Context(object):
    def __init__(self, t):
        self.text = t


class Expression(object):
    def interpret(self, c):
        text = c.text.split(' ')
        key = text[0]
        value = float(text[1])
        c.text = ' '.join(text[2:])
        self.execute(key, value)

    @abc.abstractmethod
    def execute(self, k, v):
        pass


class Scale(Expression):
    def execute(self, k, v):
        scale_dic = {1: 'bass', 2: 'alto', 3: 'treble'}
        print scale_dic[v] + " ",


class Note(Expression):
    def execute(self, k, v):
        print k + " ",


class ExpressionFactory(object):
    @staticmethod
    def create_expression(t):
        type = "Scale" if t == 'O' else "Note"
        obj = globals()[type]()
        return obj


if __name__ == "__main__":
    context = Context("O 2 E 0.5 G 0.5 A 3")
    while len(context.text):
        expression = ExpressionFactory.create_expression(context.text[0])
        expression.interpret(context)

