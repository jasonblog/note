#ifndef design_patterns_memento_game_role_h
#define design_patterns_memento_game_role_h

#include <string>
#include <vector>
using std::string;
using std::vector;

struct RoleStateMemento {
    int vitality;
    int attack;
    int defense;

    RoleStateMemento(int _vitality, int _attack, int _defense):
        vitality{_vitality}, attack{_attack}, defense{_defense} {}
};

class RoleStateCaretaker
{
public:
    void AddRecord(const RoleStateMemento& backup)
    {
        backup_pool_.push_back(backup);
    }
    RoleStateMemento GetRecord(int id)
    {
        return backup_pool_[id];
    }
private:
    vector<RoleStateMemento> backup_pool_ = {};
};

class GameRole
{
public:
    GameRole(const string& name)
        : name_{name}
        , vitality_{100}
        , attack_{100}
        , defense_{100} {}

    RoleStateMemento SaveState()
    {
        return RoleStateMemento(vitality_, attack_, defense_);
    }
    void Recovery(const RoleStateMemento& backup)
    {
        vitality_ = backup.vitality;
        attack_ = backup.attack;
        defense_ = backup.defense;
    }
    void Show()
    {
        printf("%s's state is :\n", name_.c_str());
        printf("vitality: %d\n", vitality_);
        printf("attack:   %d\n", attack_);
        printf("defense:  %d\n", defense_);
    }
    void Attack()
    {
        printf("Attach, state changed.\n");
        vitality_ -= 10;
        defense_  -= 10;
    }
private:
    string name_;
    int vitality_;
    int attack_;
    int defense_;
};

#endif
