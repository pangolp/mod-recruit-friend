SET @ENTRY:=35457;
DELETE FROM `acore_string` WHERE `entry`=@ENTRY+0;
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@ENTRY+0, '[Recruit Friend] You have to wait the cooldown to recruit a friend', '', '', '', '', '', '[Reclutar Amigo] Debes esperar el cooldown para reclutar un amigo', '[Reclutar Amigo] Debes esperar el cooldown para reclutar un amigo', '');
