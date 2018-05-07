#ifndef design_patterns_memento_client_h
#define design_patterns_memento_client_h

#include "game_role.h"

void client()
{
    RoleStateCaretaker caretaker;
    GameRole role_LiXiaoYao("Li XiaoYao");
    role_LiXiaoYao.Show();
    caretaker.AddRecord(role_LiXiaoYao.SaveState());
    role_LiXiaoYao.Attack();
    role_LiXiaoYao.Show();
    role_LiXiaoYao.Recovery(caretaker.GetRecord(0));
    role_LiXiaoYao.Show();
}
#endif
