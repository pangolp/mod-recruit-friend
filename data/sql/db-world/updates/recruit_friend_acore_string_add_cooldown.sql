SET @ENTRY:=35457;
DELETE FROM `acore_string` WHERE `entry` IN (@ENTRY+0, @ENTRY+1);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@ENTRY+0, '[Recruit Friend] You must wait for %u sec. to use this command again.', '', '', '', '', '', '[Reclutar Amigo] Debes esperar %u seg para utilizar este comando nuevamente.', '[Reclutar Amigo] Debes esperar %u seg para utilizar este comando nuevamente.', ''),
(@ENTRY+1, '[Recruit Friend] You haven\'t recruited anyone. Use the .recruit add command to do this.', '', '', '', '', '', '[Recruit Friend] No has reclutado a nadie. Utiliza el comando .recruit add para hacerlo.', '[Recruit Friend] No has reclutado a nadie. Utiliza el comando .recruit add para hacerlo.', '');
