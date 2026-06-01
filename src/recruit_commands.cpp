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

    void OnPlayerLogin(Player* player) override
    {
        if (recruitFriend.announceEnable)
        {
            ChatHandler(player->GetSession()).SendSysMessage(HELLO_RECRUIT_FRIEND);
        }
    }
};

void registerQuery(ChatHandler* handler, char const* commandType)
{
    uint32 myAccountId = handler->GetSession()->GetAccountId();

    std::string accountName;
    AccountMgr::GetName(myAccountId, accountName);

    std::string characterName = handler->GetSession()->GetPlayerName();
    std::string ipAccount = handler->GetSession()->GetRemoteAddress();

    LoginDatabase.Execute("INSERT INTO `recruit_info` (`accountId`, `accountName`, `characterName`, `ip`, `command`) VALUES ({}, '{}', '{}', '{}', '{}')", myAccountId, accountName, characterName, ipAccount, commandType);
}

// Returns true if the command is still on cooldown (message sent), false if the player can proceed.
static bool isCommandOnCooldown(ChatHandler* handler, uint32 accountId)
{
    auto it = recruitFriend.commandCooldown.find(accountId);
    if (it == recruitFriend.commandCooldown.end())
        return false;

    std::time_t currentTime = std::time(nullptr);
    uint32 delta = static_cast<uint32>(std::difftime(currentTime, it->second));
    uint32 cooldownSeconds = recruitFriend.cooldownValue / IN_MILLISECONDS;

    if (delta <= cooldownSeconds)
    {
        handler->PSendSysMessage(RECRUIT_FRIEND_COOLDOWN, cooldownSeconds - delta);
        return true;
    }

    recruitFriend.commandCooldown.erase(it);
    return false;
}

static void getTargetAccountIdByName(std::string& name, uint32& accountId)
{
    QueryResult result = CharacterDatabase.Query("SELECT `account` FROM `characters` WHERE `name`='{}'", name);

    if (!result)
    {
        accountId = 0;
        return;
    }

    accountId = (*result)[0].Get<uint32>();
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
                return false;

            if (target)
                targetAccountId = target->GetSession()->GetAccountId();
            else
                getTargetAccountIdByName(playerName, targetAccountId);

            if (targetAccountId == 0)
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, characterName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if (recruitFriend.cooldownEnabled)
            {
                if (isCommandOnCooldown(handler, myAccountId))
                    return true;
                recruitFriend.commandCooldown[myAccountId] = std::time(nullptr);
            }

            registerQuery(handler, "add");

            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id`={}", myAccountId);

            if (result)
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED);
            }
            else if (targetAccountId != myAccountId)
            {
                LoginDatabase.Execute("UPDATE `account` SET `recruiter`={} WHERE `id`={}", targetAccountId, myAccountId);
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
                if (isCommandOnCooldown(handler, myAccountId))
                    return true;
                recruitFriend.commandCooldown[myAccountId] = std::time(nullptr);
            }

            registerQuery(handler, "reset");

            LoginDatabase.Execute("UPDATE `account` SET `recruiter`=0 WHERE `id`={}", myAccountId);

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
                if (isCommandOnCooldown(handler, myAccountId))
                    return true;
                recruitFriend.commandCooldown[myAccountId] = std::time(nullptr);
            }

            registerQuery(handler, "view");

            QueryResult result = LoginDatabase.Query("SELECT `recruiter` FROM `account` WHERE `id`={}", myAccountId);

            if (result)
            {
                uint32 recruiterId = (*result)[0].Get<uint32>();

                if (recruiterId == 0)
                {
                    ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_VIEW_EMPTY);
                    return true;
                }

                QueryResult resultCharacters = CharacterDatabase.Query("SELECT `name` FROM `characters` WHERE `account`={}", recruiterId);
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
            sConfigMgr->LoadModulesConfigs();

        recruitFriend.announceEnable = sConfigMgr->GetOption<bool>("RecruitFriend.announceEnable", true);
        recruitFriend.commandEnable = sConfigMgr->GetOption<bool>("RecruitFriend.enable", true);
        recruitFriend.cooldownEnabled = sConfigMgr->GetOption<bool>("RecruitFriend.cooldownEnabled", true);
        recruitFriend.cooldownValue = sConfigMgr->GetOption<uint32>("RecruitFriend.cooldownValue", 300000);
    }
};

void AddRecruitCommandScripts()
{
    new RecruitCommandscript();
    new RecruitFriendAnnouncer();
    new RecruitFriendWorld();
}
