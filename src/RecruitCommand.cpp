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

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

enum RecruitFriendTexts
{
    HELLO_RECRUIT_FRIEND = 35450,
    RECRUIT_FRIEND_DISABLE,
    RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED,
    RECRUIT_FRIEND_SUCCESS,
    RECRUIT_FRIEND_RESET_SUCCESS,
    RECRUIT_FRIEND_TARGET_ONESELF
};

/* Message to show that the module is being used. */
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

/* Register the query, to know who has requested it. */
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

class recruit_commandscript : public CommandScript
{
    public:
        recruit_commandscript() : CommandScript("recruit_commandscript") { }

        ChatCommandTable GetCommands() const override
        {
            static ChatCommandTable recruitSetCommandTable =
            {
                { "add",        SEC_PLAYER, false, &HandleAddRecruitFriendCommand, "" },
                { "reset",      SEC_PLAYER, false, &HandleResetRecruitFriendCommand, "" }
            };

            static ChatCommandTable commandTable =
            {
                { "recruit", SEC_PLAYER, true, nullptr, "", recruitSetCommandTable }
            };

            return commandTable;
        }

        static void getTargetAccountIdByName(std::string& name, uint32& accountId)
        {
            QueryResult result = CharacterDatabase.Query("SELECT `account` FROM `characters` WHERE `name` = '%s';", name);
            accountId = (*result)[0].Get<int32>();
        }

        static bool HandleAddRecruitFriendCommand(ChatHandler* handler, const char* args)
        {

            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            if (!*args)
                return false;

            Player* target = nullptr;

            std::string playerName;

            if (!handler->extractPlayerTarget((char*)args, &target, nullptr, &playerName))
                return false;

            uint32 targetAccountId;

            if (target)
                targetAccountId = target->GetSession()->GetAccountId();
            else
                getTargetAccountIdByName(playerName, targetAccountId);

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            registerQuery(handler, "add");

            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id`={};", myAccountId);

            if (result)
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_ALREADY_HAVE_RECRUITED);
            }
            else if (targetAccountId != myAccountId)
            {
                result = LoginDatabase.Query("UPDATE `account` SET `recruiter`={} WHERE `id`={};", targetAccountId, myAccountId);
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_SUCCESS);
            }
            else
            {
                ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_TARGET_ONESELF);
            }
            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, const char* /*args*/)
        {
            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
            {
                handler->SendSysMessage(RECRUIT_FRIEND_DISABLE);
                return false;
            }

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            registerQuery(handler, "reset");

            QueryResult result = LoginDatabase.Query("UPDATE `account` SET `recruiter`=0 WHERE `id`={};", myAccountId);

            ChatHandler(handler->GetSession()).SendSysMessage(RECRUIT_FRIEND_RESET_SUCCESS);

            return true;
        }
};

void AddRecruitCommandScripts()
{
    new recruit_commandscript();
    new RecruitFriendAnnouncer();
}
