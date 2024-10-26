/**
    Nefertumm and Stevej
    https://github.com/Nefertumm/
    https://github.com/pangolp
*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "LoginDatabase.h"
#include <map>

enum RecruitFriendTexts
{
    HELLO_RECRUIT_FRIEND                        = 35450,
    RECRUIT_FRIEND_DISABLE                      = 35451,
    RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED       = 35452,
    RECRUIT_FRIEND_SUCCESS                      = 35453,
    RECRUIT_FRIEND_RESET_SUCCESS                = 35454,
    RECRUIT_FRIEND_TARGET_ONESELF               = 35455,
    RECRUIT_FRIEND_NAMES                        = 35456,
    RECRUIT_FRIEND_COOLDOWN                     = 35457,
    RECRUIT_VIEW_EMPTY                          = 35458
};

struct RecruitFriendStruct
{
    std::map<uint32, std::time_t> commandCooldown;
    bool announceEnable, commandEnable, cooldownEnabled;
    uint32 cooldownValue;
};

RecruitFriendStruct recruitFriend;

class RecruitFriendAnnouncer : public PlayerScript
{
public:
    RecruitFriendAnnouncer() : PlayerScript("RecruitFriendAnnouncer") {}

    void OnLogin(Player* player)
    {
        if (recruitFriend.announceEnable)
        {
            ChatHandler(player->GetSession()).SendSysMessage(HELLO_RECRUIT_FRIEND);
        }
    }
};

void registerQuery(ChatHandler* handler, const char* commandType)
{
    uint32 myAccountId = handler->GetSession()->GetAccountId();

    std::string accountName;

    AccountMgr::GetName(myAccountId, accountName);

    std::string characterName = handler->GetSession()->GetPlayerName();

    std::string ipAccount = handler->GetSession()->GetRemoteAddress();

    QueryResult info = LoginDatabase.Query("INSERT INTO `recruit_info` (`accountId`, `accountName`, `characterName`, `ip`, `command`) VALUES ({}, '{}', '{}', '{}', '{}')", myAccountId, accountName.c_str(), characterName.c_str(), ipAccount.c_str(), commandType);
}

static void waitToUseCommand(ChatHandler* handler, uint32 myAccountId)
{
    std::time_t currentTime = std::time(0);

    uint32 delta = std::difftime(currentTime, recruitFriend.commandCooldown[myAccountId]);

    if (delta <= (uint32)recruitFriend.cooldownValue / 1000)
    {
        ChatHandler(handler->GetSession()).PSendSysMessage(RECRUIT_FRIEND_COOLDOWN, ((uint32)recruitFriend.cooldownValue / IN_MILLISECONDS) - delta);
    }
    else
    {
        recruitFriend.commandCooldown.erase(myAccountId);
    }
}

static void getTargetAccountIdByName(std::string& name, uint32& accountId)
{
    QueryResult result = CharacterDatabase.Query("SELECT `account` FROM `characters` WHERE `name`='{}'", name);

    accountId = (*result)[0].Get<int32>();
}

using namespace Acore::ChatCommands;

class RecruitCommandscript : public CommandScript
{
    public:
        RecruitCommandscript() : CommandScript("RecruitCommandscript") {}

        ChatCommandTable GetCommands() const override
        {
            static ChatCommandTable recruitSetCommandTable =
            {
                { "add",  HandleAddRecruitFriendCommand, SEC_PLAYER, Console::No },
                { "reset",  HandleResetRecruitFriendCommand, SEC_PLAYER, Console::No },
                { "view",  HandleViewRecruitFriendCommand, SEC_PLAYER, Console::No }
            };

            static ChatCommandTable commandTable =
            {
                { "recruit",  recruitSetCommandTable }
            };

            return commandTable;
        }

        static bool HandleAddRecruitFriendCommand(ChatHandler* handler, std::string characterName)
        {

            if (!recruitFriend.commandEnable)
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            if (characterName.empty())
                return false;

            Player* target = nullptr;

            std::string playerName;
            uint32 targetAccountId;

            if (!handler->extractPlayerTarget(characterName.data(), &target, nullptr, &playerName))
            {
                return false;
            }

            if (target)
            {
                targetAccountId = target->GetSession()->GetAccountId();
            }
            else
            {
                getTargetAccountIdByName(playerName, targetAccountId);
            }

            if (targetAccountId == 0)
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, characterName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if (recruitFriend.cooldownEnabled)
            {
                waitToUseCommand(handler, myAccountId);

                if (!recruitFriend.commandCooldown[myAccountId])
                    recruitFriend.commandCooldown[myAccountId] = std::time(0);
                else
                    return true;
            }

            registerQuery(handler, "add");

            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id`={}", myAccountId);

            if (result)
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED);
            }
            else if (targetAccountId != myAccountId)
            {
                result = LoginDatabase.Query("UPDATE `account` SET `recruiter`={} WHERE `id`={}", targetAccountId, myAccountId);

                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_SUCCESS);
            }
            else
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_TARGET_ONESELF);
            }
            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler)
        {
            if (!recruitFriend.commandEnable)
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if (recruitFriend.cooldownEnabled)
            {
                waitToUseCommand(handler, myAccountId);

                if (!recruitFriend.commandCooldown[myAccountId])
                    recruitFriend.commandCooldown[myAccountId] = std::time(0);
                else
                    return true;
            }

            registerQuery(handler, "reset");

            QueryResult result = LoginDatabase.Query("UPDATE `account` SET `recruiter`=0 WHERE `id`={}", myAccountId);

            ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_RESET_SUCCESS);

            return true;
        }

        static bool HandleViewRecruitFriendCommand(ChatHandler* handler)
        {
            if (!recruitFriend.commandEnable)
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if (recruitFriend.cooldownEnabled)
            {
                waitToUseCommand(handler, myAccountId);

                if (!recruitFriend.commandCooldown[myAccountId])
                    recruitFriend.commandCooldown[myAccountId] = std::time(0);
                else
                    return true;
            }

            registerQuery(handler, "view");

            QueryResult result = LoginDatabase.Query("SELECT `recruiter` FROM `account` WHERE `id`={}", myAccountId);

            if (result)
            {
                QueryResult resultCharacters = CharacterDatabase.Query("SELECT `name` FROM `characters` WHERE `account`={}", (*result)[0].Get<uint32>());
                if (resultCharacters)
                {
                    do
                    {
                        handler->PSendSysMessage(RECRUIT_FRIEND_NAMES, (*resultCharacters)[0].Get<std::string>());
                    } while (resultCharacters->NextRow());
                }
                else
                    ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_VIEW_EMPTY);
            }

            return true;
        }
};

class RecruitFriendWorld : public WorldScript
{
public:
    RecruitFriendWorld() : WorldScript("RecruitFriendWorld") {}

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload)
        {
            sConfigMgr->LoadModulesConfigs();
            recruitFriend.announceEnable = sConfigMgr->GetOption<bool>("RecruitFriend.announceEnable", true);
            recruitFriend.commandEnable = sConfigMgr->GetOption<bool>("RecruitFriend.enable", true);
            recruitFriend.cooldownEnabled = sConfigMgr->GetOption<bool>("RecruitFriend.cooldownEnabled", true);
            recruitFriend.cooldownValue = sConfigMgr->GetOption<uint32>("RecruitFriend.cooldownValue", 300000);
        }
    }
};

void AddRecruitCommandScripts()
{
    new RecruitCommandscript();
    new RecruitFriendAnnouncer();
    new RecruitFriendWorld();
}
