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

static bool recruitEnabled, recruitAnnounceEnable;

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

        static void getTargetAccountIdByName(std::string& name, uint32& accountId)
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT `account` FROM `characters` WHERE `name` = '%s';", name);
            accountId = (*result)[0].GetInt32(); 
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

            uint32 targetAccountId;

            if (target)
                targetAccountId = target->GetSession()->GetAccountId();
            else
                getTargetAccountIdByName(playerName, targetAccountId);

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            /* Player's current recruit attribute */
            QueryResult result = LoginDatabase.PQuery("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id` = %u;", myAccountId);

            if (result)
            {
                (ChatHandler(handler->GetSession())).PSendSysMessage("You already have recruited a friend. If you want to recruit another one, you have to reset with the command.");
                return true;
            }

            result = LoginDatabase.PQuery("UPDATE `account` SET `recruiter`= %u WHERE `id` = %u;", targetAccountId, myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Player %s has been recruited. Please relog again.", playerName.c_str());

            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, const char* /*args*/)
        {

            if (!recruitEnabled)
                return false;

            uint32 myAccountId = handler->GetSession()->GetAccountId();

            QueryResult result = LoginDatabase.PQuery("UPDATE `account` SET `recruiter`= 0 WHERE `id` = %u;", myAccountId);

            (ChatHandler(handler->GetSession())).PSendSysMessage("Your recruited friend has been deleted. Please relog again.");

            return true;
        }
};

class RecruitFriendAnnouncer : public PlayerScript
{
    public:
        RecruitFriendAnnouncer() : PlayerScript("RecruitFriendAnnouncer") {}

        void OnLogin(Player* player)
        {
            if (recruitAnnounceEnable)
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Friend Recruit |rmodule.");
            }
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
            recruitAnnounceEnable = sConfigMgr->GetBoolDefault("RecruitFriend.announceEnable", true);

        }
    }
};

void AddRecruitCommandScripts()
{
    new RecruitCommand();
    new RecruitCommandWorld();
    new RecruitFriendAnnouncer();
}
