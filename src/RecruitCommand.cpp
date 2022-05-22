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

class RecruitFriendAnnouncer : public PlayerScript
{
    public:
        RecruitFriendAnnouncer() : PlayerScript("RecruitFriendAnnouncer") {}

        void OnLogin(Player* player)
        {
            if (sConfigMgr->GetOption<bool>("RecruitFriend.announceEnable", true))
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Friend Recruit |rmodule.");
            }
        }
};

using namespace Acore::ChatCommands;

class RecruitCommand : public CommandScript
{
    public:

        RecruitCommand() : CommandScript("RecruitCommand") { }

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
                return false;

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

            /* Player's current recruit attribute */
            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id` = %u;", myAccountId);

            if (result)
            {
                (ChatHandler(handler->GetSession())).PSendSysMessage("You already have recruited a friend. If you want to recruit another one, you have to reset with the command.");
                return true;
            }

            result = LoginDatabase.Query("UPDATE `account` SET `recruiter`= %u WHERE `id` = %u;", targetAccountId, myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Player %s has been recruited. Please relog again.", playerName.c_str());

            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, const char* /*args*/)
        {

            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
                return false;

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            QueryResult result = LoginDatabase.Query("UPDATE `account` SET `recruiter`= 0 WHERE `id` = %u;", myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Your recruited friend has been deleted. Please relog again.");

            return true;
        }
};

void AddRecruitCommandScripts()
{
    new RecruitCommand();
    new RecruitFriendAnnouncer();
}
