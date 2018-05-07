#ifndef design_patterns_command_command_h
#define design_patterns_command_command_h

#include "receiver.h"

class Command
{
private:
    int id_;
protected:
    Cook* cook_;
    void set_id(int id)
    {
        id_ = id;
    }
public:
    Command(Cook* cook): id_{0}, cook_{cook} {}
    virtual ~Command() {}
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    virtual void ExecuteCommand() {}
    int id() const
    {
        return id_;
    }
};

class BakeMuttonCommand : public Command
{

public:
    BakeMuttonCommand(Cook* cook): Command(cook)
    {
        set_id(1);
    }
    ~BakeMuttonCommand() {}

    void ExecuteCommand() override
    {
        cook_->MakeMutton();
    }
};

class BakeChickenWingCommand : public Command
{
public:
    BakeChickenWingCommand(Cook* cook): Command(cook)
    {
        set_id(2);
    }
    ~BakeChickenWingCommand() {}

    void ExecuteCommand() override
    {
        cook_->MakeChickenWing();
    }
};

#endif
