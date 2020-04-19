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

bool recruitEnabled;

class RecruitCommand : public CommandScript
{
    public:

        RecruitCommand() : CommandScript("RecruitCommand") { }

        std::vector<ChatCommand> GetCommands() const override
        {
            static std::vector<ChatCommand> commandTable
            {
                { "add",        SEC_PLAYER, false, &HandleAddRecruitFriendCommand, "" },
                { "reset",      SEC_PLAYER, false, &HandleResetRecruitFriendCommand, "" }
            };

            static std::vector<ChatCommand> recruitCommandTable =
            {
                { "recruit",    SEC_PLAYER, false, nullptr, "", commandTable}
            };

            return recruitCommandTable;

        }

        static bool HandleAddRecruitFriendCommand(ChatHandler* handler, const char* args)
        {

            if (!recruitEnabled)
                return false;

            if (!*args)
                return false;

            Player* target = nullptr;
            std::string playerName;

            if (!handler->extractPlayerTarget((char*)args, &target, nullptr, &playerName))
                return false;

            uint32 myAccountId = handler->GetSession()->GetAccountId();
            uint32 targetAccountId = target->GetSession()->GetAccountId();

            QueryResult result = LoginDatabase.PQuery("UPDATE `account` SET `recruiter`= %u WHERE `id` = %u;", targetAccountId, myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Player %s has been recruited.", playerName.c_str());

            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, const char* /*args*/)
        {

            if (!recruitEnabled)
                return false;

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            QueryResult result = LoginDatabase.PQuery("UPDATE `account` SET `recruiter`= 0 WHERE `id` = %u;", myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Your recruited friend has been deleted.");

            return true;
        }
};

class RecruitCommandWorld : public WorldScript
{
public:
    RecruitCommandWorld() : WorldScript("RecruitCommandWorld") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload)
        {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/mod_recruit_friend.conf";
#ifdef WIN32
            cfg_file = "mod_recruit_friend.conf";
#endif
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());
            sConfigMgr->LoadMore(cfg_file.c_str());

            recruitEnabled = sConfigMgr->GetBoolDefault("RecruitFriend.enable", true);

        }
    }
};

void AddRecruitCommandScripts()
{
    new RecruitCommand();
    new RecruitCommandWorld();
}
