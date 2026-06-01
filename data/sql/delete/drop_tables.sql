-- In World
DELETE FROM `command` WHERE `name` IN ('recruit add', 'recruit reset');

SET @ENTRY:=35450;
DELETE FROM `acore_string` WHERE `entry` BETWEEN @ENTRY+0 AND @ENTRY+6;

DELETE FROM `module_string` WHERE `module` = 'mod-recruit-friend';
DELETE FROM `module_string_locale` WHERE `module` = 'mod-recruit-friend';
