SET @ENTRY:=35456;
DELETE FROM `acore_string` WHERE `entry`=@ENTRY+0;
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@ENTRY+0, '[Recruit Friend] Recruited character: %s', '', '', '', '', '', '[Recruit Friend] Personaje reclutado: %s', '[Recruit Friend] Personaje reclutado: %s', '');
