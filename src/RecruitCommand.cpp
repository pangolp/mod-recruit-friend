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
    HELLO_RECRUIT_FRIEND = 35450,
    RECRUIT_FRIEND_DISABLE,
    RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED,
    RECRUIT_FRIEND_SUCCESS,
    RECRUIT_FRIEND_RESET_SUCCESS,
    RECRUIT_FRIEND_TARGET_ONESELF,
    RECRUIT_FRIEND_NAMES,
    RECRUIT_FRIEND_COOLDOWN,
};

std::map<uint32, std::time_t> commandCooldown;

class RecruitFriendAnnouncer : public PlayerScript
{
    public:
        RecruitFriendAnnouncer() : PlayerScript("RecruitFriendAnnouncer") {}

        void OnLogin(Player* player)
        {
            if (sConfigMgr->GetOption<bool>("RecruitFriend.announceEnable", true))
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
    QueryResult info = LoginDatabase.Query("INSERT INTO `recruit_info` (`accountId`, `accountName`, `characterName`, `ip`, `command`) VALUES ({}, '{}', '{}', '{}', '{}');", myAccountId, accountName.c_str(), characterName.c_str(), ipAccount.c_str(), commandType);
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
                { "add",  HandleAddRecruitFriendCommand,  SEC_PLAYER, Console::Yes },
                { "reset",  HandleResetRecruitFriendCommand,  SEC_PLAYER, Console::Yes },
                { "view",  HandleViewRecruitFriendCommand,  SEC_PLAYER, Console::Yes }
            };

            static ChatCommandTable commandTable =
            {
                { "recruit",  recruitSetCommandTable }
            };

            return commandTable;
        }

        static void getTargetAccountIdByName(std::string& name, uint32& accountId)
        {
            QueryResult result = CharacterDatabase.Query("SELECT `account` FROM `characters` WHERE `name`='{}';", name);
            accountId = (*result)[0].Get<int32>();
        }

        static bool HandleAddRecruitFriendCommand(ChatHandler* handler, std::string args)
        {

            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            if (!args.empty())
                return false;

            Player* target = nullptr;

            std::string playerName;

            if (!handler->extractPlayerTarget((char*)args.c_str(), &target, nullptr, &playerName))
                return false;

            uint32 targetAccountId;

            if (target)
                targetAccountId = target->GetSession()->GetAccountId();
            else
                getTargetAccountIdByName(playerName, targetAccountId);

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if(sConfigMgr->GetOption<bool>("RecruitFriend.cooldownEnabled", true))
            {
                uint32 cooldownValue = sConfigMgr->GetOption<uint32>("RecruitFriend.cooldownValue", 300000);
                std::time_t currentTime = std::time(0);

                if( currentTime - commandCooldown[myAccountId] <= cooldownValue)
                {
                    ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_COOLDOWN);
                    return true;
                }
                else
                {
                    commandCooldown.erase(myAccountId);
                }
            }

            registerQuery(handler, "add");

            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id`={};", myAccountId);

            if (result)
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED);
            }
            else if (targetAccountId != myAccountId)
            {
                result = LoginDatabase.Query("UPDATE `account` SET `recruiter`={} WHERE `id`={};", targetAccountId, myAccountId);
                commandCooldown[myAccountId] = std::time(0);
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_SUCCESS);
            }
            else
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_TARGET_ONESELF);
            }
            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, std::string /*args*/)
        {
            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            if(sConfigMgr->GetOption<bool>("RecruitFriend.cooldownEnabled", true))
            {
                uint32 cooldownValue = sConfigMgr->GetOption<uint32>("RecruitFriend.cooldownValue", 300000);
                std::time_t currentTime = std::time(0);

                if( currentTime - commandCooldown[myAccountId] <= cooldownValue)
                {
                    ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_COOLDOWN);
                    return true;
                }
                else
                {
                    commandCooldown.erase(myAccountId);
                }
            }

            registerQuery(handler, "reset");

            QueryResult result = LoginDatabase.Query("UPDATE `account` SET `recruiter`=0 WHERE `id`={};", myAccountId);

            ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_RESET_SUCCESS);

            return true;
        }

        static bool HandleViewRecruitFriendCommand(ChatHandler* handler, std::string /*args*/)
        {
            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            registerQuery(handler, "view");

            QueryResult result = LoginDatabase.Query("SELECT `recruiter` FROM `account` WHERE `id`={};", myAccountId);

            if (result)
            {
                Field* fields = result->Fetch();
                QueryResult resultCharacters = CharacterDatabase.Query("SELECT `name` FROM `characters` WHERE `account`={};", fields[0].Get<uint8>());
                if (resultCharacters)
                {
                    do
                    {
                        Field* fieldsCharacters = resultCharacters->Fetch();
                        handler->PSendSysMessage(RECRUIT_FRIEND_NAMES, fieldsCharacters[0].Get<std::string>());
                    } while (resultCharacters->NextRow());
                }
            }

            return true;
        }
};

void AddRecruitCommandScripts()
{
    new RecruitCommandscript();
    new RecruitFriendAnnouncer();
}
