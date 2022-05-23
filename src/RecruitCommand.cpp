/**
    Nefertumm and Stevej
    https://github.com/Nefertumm/
    https://github.com/pangolp
 */

#include "loader.h"
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
                std::string messageModule = "";
                switch (player->GetSession()->GetSessionDbLocaleIndex())
                {
                    case LOCALE_enUS:
                    case LOCALE_koKR:
                    case LOCALE_frFR:
                    case LOCALE_deDE:
                    case LOCALE_zhCN:
                    case LOCALE_zhTW:
                    case LOCALE_ruRU:
                    {
                        messageModule = "This server is running the |cff4CFF00Friend Recruit |rmodule.";
                        break;
                    }
                    case LOCALE_esES:
                    case LOCALE_esMX:
                    {
                        messageModule = "Este servidor está ejecutando el módulo |cff4CFF00Friend Recruit|r";
                    }
                    default:
                        break;
                }
                ChatHandler(player->GetSession()).SendSysMessage(messageModule);
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
            QueryResult result = LoginDatabase.Query("SELECT * FROM `account` WHERE `recruiter` <> 0 AND `id`={};", myAccountId);

            if (result)
            {
                std::string messageRecruitReset = "";
                switch (target->GetSession()->GetSessionDbLocaleIndex())
                {
                    case LOCALE_enUS:
                    case LOCALE_koKR:
                    case LOCALE_frFR:
                    case LOCALE_deDE:
                    case LOCALE_zhCN:
                    case LOCALE_zhTW:
                    case LOCALE_ruRU:
                    {
                        messageRecruitReset = "You already have recruited a friend. If you want to recruit another one, you have to reset with the command: reset.";
                        break;
                    }
                    case LOCALE_esES:
                    case LOCALE_esMX:
                    {
                        messageRecruitReset = "Ya has reclutado a un amigo. Si quieres reclutar a otro, tienes que reiniciar con el comando: reset.";
                    }
                    default:
                        break;
                }
                (ChatHandler(handler->GetSession())).SendSysMessage(messageRecruitReset);
                return true;
            }

            result = LoginDatabase.Query("UPDATE `account` SET `recruiter`={} WHERE `id`={};", targetAccountId, myAccountId);

            std::string messageRecruitSuccess = "";
            switch (target->GetSession()->GetSessionDbLocaleIndex())
            {
                case LOCALE_enUS:
                case LOCALE_koKR:
                case LOCALE_frFR:
                case LOCALE_deDE:
                case LOCALE_zhCN:
                case LOCALE_zhTW:
                case LOCALE_ruRU:
                {
                    messageRecruitSuccess = "Excellent, you have recruited in the right way. Close the game, and log in again, for the changes to take effect.";
                    break;
                }
                case LOCALE_esES:
                case LOCALE_esMX:
                {
                    messageRecruitSuccess = "Excelente, has reclutado de manera correcta. Cierra el juego, y vuelve a ingresar, para que los cambios surjan efecto.";
                }
                default:
                    break;
            }

            (ChatHandler(handler->GetSession())).SendSysMessage(messageRecruitSuccess);

            return true;
        }

        static bool HandleResetRecruitFriendCommand(ChatHandler* handler, const char* args)
        {

            if (!sConfigMgr->GetOption<bool>("RecruitFriend.enable", true))
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

            QueryResult result = LoginDatabase.Query("UPDATE `account` SET `recruiter`=0 WHERE `id`={};", myAccountId);

            std::string messageRecruitReset = "";
            switch (target->GetSession()->GetSessionDbLocaleIndex())
            {
            case LOCALE_enUS:
            case LOCALE_koKR:
            case LOCALE_frFR:
            case LOCALE_deDE:
            case LOCALE_zhCN:
            case LOCALE_zhTW:
            case LOCALE_ruRU:
            {
                messageRecruitReset = "Excellent your recruited restarted. Close the game and log back in for the changes to take effect.";
                break;
            }
            case LOCALE_esES:
            case LOCALE_esMX:
            {
                messageRecruitReset = "Excelente tu reclutado se reinicio. Cierra el juego y vuelve a ingresar para que los cambios surjan efecto.";
            }
            default:
                break;
            }

            (ChatHandler(handler->GetSession())).SendSysMessage(messageRecruitReset);

            return true;
        }
};

void AddRecruitCommandScripts()
{
    new RecruitCommand();
    new RecruitFriendAnnouncer();
}
