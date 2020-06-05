DELETE FROM `command` WHERE `name` IN ('recruit add', 'recruit reset');

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('recruit add', 0, 'Syntax: .recruit add #playerName The command allows you to recruit the player whose name is #playerName. It does not use the #, only the name of our friend.'),
('recruit reset', 0, 'Syntax: .recruit reset Allows to reset the recruit, allowing the player to re-recruit after disconnecting.');
