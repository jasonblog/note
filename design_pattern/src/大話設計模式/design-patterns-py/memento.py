class StateMemento(object):
    def __init__(self, h, m):
        self.hp = h
        self.mp = m


class GameRole(object):
    def __init__(self):
        self.hp = 100
        self.mp = 100

    def fight(self):
        self.hp = 0
        self.mp = 0

    def create_memento(self):
        return StateMemento(self.hp, self.mp)

    def recovery_state(self, m):
        self.hp = m.hp
        self.mp = m.mp

    def state_display(self):
        print str(self.hp) + " " + str(self.mp)


class StateCaretaker(object):
    def __init__(self, m):
        self.__memento = m  # .__memento ==> ._StateCaretaker__memento (Name Mangling)

    def get_memento(self):
        return self.__memento


if __name__ == "__main__":
    game_role = GameRole()
    state_caretaker = StateCaretaker(game_role.create_memento())
    game_role.state_display()

    game_role.fight()
    game_role.state_display()

    game_role.recovery_state(state_caretaker.get_memento())
    game_role.state_display()