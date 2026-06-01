-- Migrate strings from acore_string to module_string / module_string_locale
SET @ENTRY := 35450;
DELETE FROM `acore_string` WHERE `entry` BETWEEN @ENTRY+0 AND @ENTRY+8;

DELETE FROM `module_string` WHERE `module` = 'mod-recruit-friend';
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
('mod-recruit-friend', 1, 'This server is running the |cff4CFF00Friend Recruit |rmodule.'),
('mod-recruit-friend', 2, '[Recruit Friend] The module is disabled'),
('mod-recruit-friend', 3, '[Recruit Friend] You have already recruited a friend. If you want to recruit another one, you have to restart with the command : .recruit reset'),
('mod-recruit-friend', 4, '[Recruit Friend] Excellent, you have recruited in the right way. Close the game, and log in again, for the changes to take effect.'),
('mod-recruit-friend', 5, '[Recruit Friend] Excellent, your recruitment has been reset. Close the game and log back in for the changes to take effect.'),
('mod-recruit-friend', 6, '[Recruit Friend] You cannot recruit yourself.'),
('mod-recruit-friend', 7, '[Recruit Friend] Recruited character: {}'),
('mod-recruit-friend', 8, '[Recruit Friend] You must wait for {} sec. to use this command again.'),
('mod-recruit-friend', 9, '[Recruit Friend] You haven\'t recruited anyone. Use the .recruit add command to do this.');

DELETE FROM `module_string_locale` WHERE `module` = 'mod-recruit-friend';
INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
('mod-recruit-friend', 1, 'esES', 'Este servidor está ejecutando el módulo |cff4CFF00Recruit Friend.'),
('mod-recruit-friend', 1, 'esMX', 'Este servidor está ejecutando el módulo |cff4CFF00Recruit Friend.'),
('mod-recruit-friend', 2, 'esES', '[Reclutar Amigo] El módulo está desactivado'),
('mod-recruit-friend', 2, 'esMX', '[Reclutar Amigo] El módulo está desactivado'),
('mod-recruit-friend', 3, 'esES', '[Reclutar Amigo] Ya has reclutado a un amigo. Si quieres reclutar a otro, tienes que reiniciar con el comando : recruit reset.'),
('mod-recruit-friend', 3, 'esMX', '[Reclutar Amigo] Ya has reclutado a un amigo. Si quieres reclutar a otro, tienes que reiniciar con el comando : recruit reset.'),
('mod-recruit-friend', 4, 'esES', '[Reclutar Amigo] Excelente, has reclutado de la forma correcta. Cierra el juego e inicia sesión de nuevo para que los cambios surtan efecto.'),
('mod-recruit-friend', 4, 'esMX', '[Reclutar Amigo] Excelente, has reclutado de la forma correcta. Cierra el juego e inicia sesión de nuevo para que los cambios surtan efecto.'),
('mod-recruit-friend', 5, 'esES', '[Reclutar Amigo] Excelente tu reclutamiento ha sido reiniciado. Cierra el juego y vuelve a iniciar sesión para que los cambios surtan efecto.'),
('mod-recruit-friend', 5, 'esMX', '[Reclutar Amigo] Excelente tu reclutamiento ha sido reiniciado. Cierra el juego y vuelve a iniciar sesión para que los cambios surtan efecto.'),
('mod-recruit-friend', 6, 'esES', '[Reclutar Amigo] No te puedes reclutar a ti mismo.'),
('mod-recruit-friend', 6, 'esMX', '[Reclutar Amigo] No te puedes reclutar a ti mismo.'),
('mod-recruit-friend', 7, 'esES', '[Recruit Friend] Personaje reclutado: {}'),
('mod-recruit-friend', 7, 'esMX', '[Recruit Friend] Personaje reclutado: {}'),
('mod-recruit-friend', 8, 'esES', '[Reclutar Amigo] Debes esperar {} seg para utilizar este comando nuevamente.'),
('mod-recruit-friend', 8, 'esMX', '[Reclutar Amigo] Debes esperar {} seg para utilizar este comando nuevamente.'),
('mod-recruit-friend', 9, 'esES', '[Recruit Friend] No has reclutado a nadie. Utiliza el comando .recruit add para hacerlo.'),
('mod-recruit-friend', 9, 'esMX', '[Recruit Friend] No has reclutado a nadie. Utiliza el comando .recruit add para hacerlo.');
