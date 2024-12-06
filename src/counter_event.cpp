#include "counter_event.hpp"

enum E_WeldAction : unsigned int
{
    eInitAction = 0,              // 初始姿态
    eGrind_MovorOff = 0b00000001, // 移动到打磨位 or 打磨关闭
    eGrind_OnorDown = 0b01000001, // 打磨开启 or 打磨下降
    eGrind_Up = 0b01001001,       // 打磨举升
    eWeld_MovorDwon = 0b00000010, // 移动到焊接位
    eWeld_Fix = 0b00000110,       // 定位气缸开
    eWeld_Up = 0b00010110,        // 焊接举升
    eWeld_On = 0b10010110,        // 焊接起弧
    eWeld_Down = 0b00100010,      // 焊接起弧关+焊接下降+定位气缸关
    eNone_Action = 255            // 初始状态
};

const QVector<E_WeldAction> ActionList = {eGrind_MovorOff, eGrind_OnorDown, eGrind_Up, eGrind_OnorDown, eGrind_MovorOff, eWeld_MovorDwon, eWeld_Fix, eWeld_Up, eWeld_On, eWeld_Down, eInitAction};
const QVector<int> ActionTime = {40, 20, 100, 20, 20, 40, 40, 40, 40, 40, 5};
const QVector<std::string> ActionName = {"GrindMovorOff", "Grind_OnorDown", "Grind_Up", "Weld_MovorDwon", "Grind_MovorOff", "Weld_MovorDwon", "Weld_Fix", "Weld_Up", "Weld_On", "Weld_Down", "InitAction"};

bool DoWeld(int execute)
{
    auto log = spdlog::get("logger");
    static quint8 index_tool = 1; // 执行焊枪编号范围1~5
    static quint8 index_act = 0;
    static quint8 time_cnt = 0; // 周期计数，控制动作间隔

    //=====================结束碰钉 ==========================
    if (execute == -1)
    {
        unsigned char tem = ActionList[index_act] & 0b00100011; // 关闭打磨、碰钉、定位气缸、打磨降
        tem |= 0b00100000;                                      // 碰钉枪下降
        log->info("{}: m_Comm->SetToolsAction(index_tool, (E_WeldAction)tem); ", __LINE__);
        log->info("{}: m_Comm->SetToolsAction(11 - index_tool, (E_WeldAction)tem); ", __LINE__);
        log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

        index_tool = 1;
        index_act = 0;
        time_cnt = 0;
        log->info("结束碰钉作业");
        return true;
    }

    //=====================暂停碰钉 ==========================
    static quint32 pause_cnt = 0;
    if (execute == 0)
    {
        pause_cnt++;
        if (pause_cnt > 600) // 暂停时间过长，停止
        {
            unsigned char tem = ActionList[index_act] & 0b10111111; // 关闭打磨
            log->info("{}: m_Comm->SetToolsAction(index_tool, (E_WeldAction)tem); ", __LINE__);
            log->info("{}: m_Comm->SetToolsAction(11 - index_tool, (E_WeldAction)tem); ", __LINE__);
            log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

            pause_cnt = 0;
            log->warn("暂停时间过长，关闭打磨及接触器");
        }
        log->info("自动碰钉暂停");
        return true;
    }

    //=============== 执行10把焊枪轮次焊接1~5,6~10 ==============
    if (index_act < ActionList.size())
    {
        if (time_cnt == 0)
        {
            // 执行动作
            log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index_tool, index_act);
            log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, 11 - index_tool, index_act);
            log->info("焊枪{}执行动作:{} {}", index_tool, index_act, ActionName[index_act]);

            // 在特定动作连接焊枪
            if (ActionList[index_act] == eWeld_Up)
            {
                log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, index_tool);
                log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, 11 - index_tool);
                log->debug("接触器吸合");
            }
            if (ActionList[index_act] == eWeld_Down)
            {
                log->info("{}: m_Comm->SetGunConnect(0); ", __LINE__);
                log->debug("接触器断开");
            }
            time_cnt++;
        }
        else
        {
            // 计数等待
            time_cnt++;

            if (time_cnt > ActionTime[index_act]) // 等待结束，进入下一个动作
            {
                index_act++;
                time_cnt = 0;
            }
        }

        return false;
    }
    else
    {
        // 完成所有动作，切换到下一把焊枪
        log->info("完成焊枪：{}", index_tool);

        log->info("{}: m_Comm->SetToolsAction(index_tool, eInitAction); //1~5号枪动作 ", __LINE__);
        log->info("{}: m_Comm->SetToolsAction(11 - index_tool, eInitAction);//6~10号枪动作", __LINE__);

        index_act = 0;
        index_tool++;

        if (index_tool > 5) // 所有焊枪动作完成
        {
            index_tool = 1;
            log->info("所有焊枪动作完成");
            return true;
        }

        return false;
    }
    return true;
}

bool NewDoWeld(int execute)
{
    bool exit_flag{false};
    auto log = spdlog::get("logger");
    static quint8 index_tool = 1; // 执行焊枪编号范围1~5
    static quint8 index_act = 0;
    static quint8 time_cnt = 0; // 周期计数，控制动作间隔
    bool tools_exit{false};
    //=============== 执行10把焊枪轮次焊接1~5,6~10 ==============
    if (index_act < ActionList.size())
    {
        if (time_cnt == 0)
        {
            if (index_act == 8)
            {
                log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, index_tool);
                log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, 11 - index_tool);
                log->debug("接触器吸合: {}, {}", index_tool, 11 - index_tool);
            }
            else
            {
                log->info("所有焊枪执行动作:{} {}", index_act, ActionName[index_act]);
                for (int i = 0; i < 10; i++)
                    log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, i, index_act);
            }

            time_cnt++;
        }
        else
        {
            time_cnt++;
            if (time_cnt > ActionTime[index_act]) // 等待结束，进入下一个动作
            {
                if (index_act == 8)
                {
                    if (index_tool > 5)
                    {
                        index_act++;
                    }
                    else
                    {
                        index_tool++;
                    }
                }
                else
                {
                    index_act++;
                }
                time_cnt = 0;
            }
        }
    }
    else
    {
        log->info("{}: m_Comm->SetGunConnect(0); ", __LINE__);
        exit_flag = true;
    }

    return exit_flag;
}

enum class ActionKey
{
    Grind_MovorOff1 = 0,
    Grind_OnorDown1 = 40,
    Grind_Up = 60,
    Grind_OnorDown2 = 160,
    Grind_MovorOff2 = 180,
    Weld_MovorDwon = 200,
    Weld_Fix = 240,
    Weld_Up = 280,
    Weld_On = 320,
    Weld_Down = 360,
    InitAction = 400,
    End = 405
};

std::map<ActionKey, std::string> ActionMap =
    {
        {ActionKey::Grind_MovorOff1, "Grind_MovorOff1 "},
        {ActionKey::Grind_OnorDown1, "Grind_OnorDown1 "},
        {ActionKey::Grind_Up, "Grind_Up "},
        {ActionKey::Grind_OnorDown2, "Grind_OnorDown2 "},
        {ActionKey::Grind_MovorOff2, "Grind_MovorOff2 "},
        {ActionKey::Weld_MovorDwon, "Weld_MovorDwon "},
        {ActionKey::Weld_Fix, "Weld_Fix "},
        {ActionKey::Weld_Up, "Weld_Up "},
        {ActionKey::Weld_On, "Weld_On "},
        {ActionKey::Weld_Down, "Weld_Down "},
        {ActionKey::InitAction, "InitAction "}};

std::map<std::string, int> ValueMap;

void DoWelding(int tool_a, int tool_b, int key)
{
    std::bitset<8> value;
    auto log = spdlog::get("logger");
    switch (static_cast<ActionKey>(key))
    {
    case ActionKey::Grind_MovorOff1:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Grind_MovorOff1]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Grind_MovorOff1]);
        value = eGrind_MovorOff;
        break;
    }
    case ActionKey::Grind_OnorDown1:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Grind_OnorDown1]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Grind_OnorDown1]);
        value = eGrind_OnorDown;
        break;
    }
    case ActionKey::Grind_Up:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Grind_Up]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Grind_Up]);
        value = eGrind_Up;
        break;
    }
    case ActionKey::Grind_OnorDown2:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Grind_OnorDown2]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Grind_OnorDown2]);
        value = eGrind_OnorDown;
        break;
    }
    case ActionKey::Grind_MovorOff2:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Grind_MovorOff2]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Grind_MovorOff2]);
        value = eGrind_MovorOff;
        break;
    }
    case ActionKey::Weld_MovorDwon:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Weld_MovorDwon]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Weld_MovorDwon]);
        value = eWeld_MovorDwon;
        break;
    }
    case ActionKey::Weld_Fix:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Weld_Fix]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Weld_Fix]);
        value = eWeld_Fix;
        break;
    }
    case ActionKey::Weld_Up:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Weld_Up]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Weld_Up]);
        value = eWeld_Up;
        break;
    }
    case ActionKey::Weld_On:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Weld_On]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Weld_On]);
        log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, tool_a);
        log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, tool_b);
        value = eWeld_On;
        break;
    }
    case ActionKey::Weld_Down:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::Weld_Down]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::Weld_Down]);
        log->info("{}: m_Comm->SetGunConnect(0); ", __LINE__);
        value = eWeld_Down;
        break;
    }
    case ActionKey::InitAction:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, tool_a, ActionMap[ActionKey::InitAction]);
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]);//6~10号枪动作 ", __LINE__, tool_b, ActionMap[ActionKey::InitAction]);
        value = eInitAction;
        break;
    }
    }
    if (value.count() != 0)
    {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::tm *now_tm = std::localtime(&timestamp);
        std::stringstream os;
        os << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

        auto it = ValueMap.find(os.str());
        if (it != ValueMap.end())
        {
            it->second += value.count();
        }
        else
        {
            ValueMap[os.str()] = value.count();
        }
        log->warn("{}: value.count(): {}, timestamp: {}", __LINE__, value.count(), os.str());
    }
}

void SingleToolDoWeldingExecuteUnit(int index, int key)
{
    std::bitset<8> value;
    auto log = spdlog::get("logger");
    switch (static_cast<ActionKey>(key))
    {
    case ActionKey::Grind_MovorOff1:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Grind_MovorOff1]);
        value = eGrind_MovorOff;
        break;
    }
    case ActionKey::Grind_OnorDown1:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Grind_OnorDown1]);
        value = eGrind_OnorDown;
        break;
    }
    case ActionKey::Grind_Up:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Grind_Up]);
        value = eGrind_Up;
        break;
    }
    case ActionKey::Grind_OnorDown2:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Grind_OnorDown2]);
        value = eGrind_OnorDown;
        break;
    }
    case ActionKey::Grind_MovorOff2:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Grind_MovorOff2]);
        value = eGrind_MovorOff;
        break;
    }
    case ActionKey::Weld_MovorDwon:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Weld_MovorDwon]);
        value = eWeld_MovorDwon;
        break;
    }
    case ActionKey::Weld_Fix:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Weld_Fix]);
        value = eWeld_Fix;
        break;
    }
    case ActionKey::Weld_Up:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Weld_Up]);
        value = eWeld_Up;
        break;
    }
    case ActionKey::Weld_On:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Weld_On]);
        log->info("{}: m_Comm->SetGunConnect({}); ", __LINE__, index);
        value = eWeld_On;
        break;
    }
    case ActionKey::Weld_Down:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::Weld_Down]);
        log->info("{}: m_Comm->SetGunConnect(0); ", __LINE__);
        value = eWeld_Down;
        break;
    }
    case ActionKey::InitAction:
    {
        log->info("{}: m_Comm->SetToolsAction({}, ActionList[{}]); //1~5号枪动作 ", __LINE__, index, ActionMap[ActionKey::InitAction]);
        value = eInitAction;
        break;
    }
    }
    if (value.count() != 0)
    {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::tm *now_tm = std::localtime(&timestamp);
        std::stringstream os;
        os << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

        auto it = ValueMap.find(os.str());
        if (it != ValueMap.end())
        {
            it->second += value.count();
        }
        else
        {
            ValueMap[os.str()] = value.count();
        }
        log->warn("{}: value.count(): {}, timestamp: {}", __LINE__, value.count(), os.str());
    }
}

bool NewNewDoWeld(int execute)
{
    auto log = spdlog::get("logger");
    static quint8 index_tool = 1;  // 执行焊枪编号范围1~5
    static quint8 index_tool2 = 1; // 执行焊枪编号范围1~5
    static quint8 index_act = 0;
    static int time_cnt = 0;  // 周期计数，控制动作间隔
    static int time_cnt2 = 0; // 周期计数，控制动作间隔
    static bool offset_flag{false};
    static bool end_flag{true};

    //=====================结束碰钉 ==========================
    if (execute == -1)
    {
        unsigned char tem = ActionList[index_act] & 0b00100011; // 关闭打磨、碰钉、定位气缸、打磨降
        tem |= 0b00100000;                                      // 碰钉枪下降
        log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool);
        log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, 11 - index_tool);
        if (offset_flag)
        {
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool2);
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, 11 - index_tool2);
        }
        log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

        index_tool = 1;
        index_tool2 = 1;
        index_act = 0;
        time_cnt = 0;
        time_cnt2 = 0;
        log->info("结束碰钉作业");
        return true;
    }

    //=====================暂停碰钉 ==========================
    static quint32 pause_cnt = 0;
    if (execute == 0)
    {
        pause_cnt++;
        if (pause_cnt > 600) // 暂停时间过长，停止
        {
            unsigned char tem = ActionList[index_act] & 0b10111111; // 关闭打磨
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool);
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, 11 - index_tool);
            if (offset_flag)
            {
                log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool2);
                log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, 11 - index_tool2);
            }
            log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

            pause_cnt = 0;
            log->warn("暂停时间过长，关闭打磨及接触器");
        }
        log->info("自动碰钉暂停");
        return true;
    }

    //=============== 执行10把焊枪轮次焊接1~5,6~10 ==============
    DoWelding(index_tool, 11 - index_tool, time_cnt);
    time_cnt++;
    if (time_cnt > static_cast<int>(ActionKey::End))
    {
        time_cnt = 0;
        index_tool = index_tool2 + 1;
    }

    if (!offset_flag)
    {
        // if (time_cnt > static_cast<int>(ActionKey::Grind_MovorOff2))
        if (time_cnt > 200)
        {
            index_tool2 = index_tool + 1;
            offset_flag = true;
        }
    }
    if (offset_flag)
    {
        if (end_flag)
        {
            DoWelding(index_tool2, 11 - index_tool2, time_cnt2);
            time_cnt2++;
            if (time_cnt2 > static_cast<int>(ActionKey::End))
            {
                time_cnt2 = 0;
                index_tool2 = index_tool + 1;
                if (index_tool2 > 5)
                {
                    end_flag = false;
                }
            }
        }
    }

    if (index_tool > 5)
    {
        return true;
    }

    return false;
}

bool DoubleToolsDoWeldAction(int execute)
{
    auto log = spdlog::get("logger");
    static quint8 index_tool = 1;   // 执行焊枪编号范围1~5
    static quint8 index_tool2 = 10; // 执行焊枪编号范围1~5
    static quint8 index_act = 0;
    static int time_cnt = 0;  // 周期计数，控制动作间隔
    static int time_cnt2 = 0; // 周期计数，控制动作间隔
    static bool offset_flag{false};
    static bool end_flag{true};

    //=====================结束碰钉 ==========================
    if (execute == -1)
    {
        unsigned char tem = ActionList[index_act] & 0b00100011; // 关闭打磨、碰钉、定位气缸、打磨降
        tem |= 0b00100000;                                      // 碰钉枪下降
        log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool);
        if (offset_flag)
        {
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool2);
        }
        log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

        index_tool = 1;
        index_tool2 = 10;
        index_act = 0;
        time_cnt = 0;
        time_cnt2 = 0;
        log->info("结束碰钉作业");
        return true;
    }

    //=====================暂停碰钉 ==========================
    static quint32 pause_cnt = 0;
    if (execute == 0)
    {
        pause_cnt++;
        if (pause_cnt > 600) // 暂停时间过长，停止
        {
            unsigned char tem = ActionList[index_act] & 0b10111111; // 关闭打磨
            log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool);
            if (offset_flag)
            {
                log->info("{}: m_Comm->SetToolsAction({}, (E_WeldAction)tem); ", __LINE__, index_tool2);
            }
            log->info("{}: m_Comm->SetGunConnect(0);//关闭接触器 ", __LINE__);

            pause_cnt = 0;
            log->warn("暂停时间过长，关闭打磨及接触器");
        }
        log->info("自动碰钉暂停");
        return true;
    }

    //=============== 执行10把焊枪轮次焊接1~5,6~10 ==============
    if (index_tool > 5)
    {
        goto __JUMP_First;
    }
    SingleToolDoWeldingExecuteUnit(index_tool, time_cnt);
    time_cnt++;
    if (time_cnt > static_cast<int>(ActionKey::End))
    {
        time_cnt = 0;
        index_tool++;
    }
__JUMP_First:

    if (!offset_flag)
    {
        if (time_cnt > 200)
        {
            offset_flag = true;
        }
    }
    if (offset_flag)
    {
        if (end_flag)
        {
            SingleToolDoWeldingExecuteUnit(index_tool2, time_cnt2);
            time_cnt2++;
            if (time_cnt2 > static_cast<int>(ActionKey::End))
            {
                time_cnt2 = 0;
                index_tool2--;
                if (index_tool2 <= 5)
                {
                    end_flag = false;
                }
            }
        }
    }

    if (index_tool > 5 && !end_flag)
    {
        index_tool = 1;
        index_tool2 = 10;
        index_act = 0;
        time_cnt = 0;
        time_cnt2 = 0;
        return true;
    }

    return false;
}

void Loop()
{
    auto log = spdlog::get("logger");
    auto begin = std::chrono::steady_clock::now();
    int execute{1};
    int cnt{1};
    while (true)
    {
        //      cnt++;
        //      if (cnt > 600)
        //      {
        //          execute = -1;
        //      }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (DoubleToolsDoWeldAction(execute))
        {
            auto duration = std::chrono::steady_clock::now() - begin;
            log->warn("whole duration: {} (s)\n exit from loop...", std::chrono::duration_cast<std::chrono::seconds>(duration).count());
            std::vector<std::pair<std::string, int>> container;
            for (const auto &it : ValueMap)
            {
                container.push_back(std::make_pair(it.first, it.second));
            }
            std::stable_sort(container.begin(), container.end(), [](std::pair<std::string, int> a, std::pair<std::string, int> b)
                             { return a.second > b.second; });
            for (const auto &it : container)
            {
                log->info("timestamp: {}, value count: {}", it.first, it.second);
            }
            break;
        }
    }
}
