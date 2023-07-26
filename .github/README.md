# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# Module: Recruit a friend

- Latest build status with azerothcore:

[![Build Status](https://github.com/pangolp/mod-recruit-friend/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/pangolp/mod-recruit-friend)

[English](README.md) | [Español](README_ES.md)

## Important note

Remember, that inside the SQL folder, inside world, you have a series of texts, which are going to be added inside acore_string. The id, which is inside the file, depends on the number of modules you have, so before executing it, check them, so as not to delete anything that may be used in other modules.

## What is the module for?

The module allows players to recruit a friend within the game and get the benefits of the game.

**The linking is at the account level**

**And it does not give additional rewards, only the original ones. Which are:**

- Enhanced mission experience
- Enhanced experience by eliminating npc
- Give a level to a friend, for each level obtained.
- Invoke a friend, once every 60 minutes.

I hope it serves you and you can enjoy it.

Remember that within the worldserver.conf file, you have some variables, which you can modify to extend the benefits beyond level 60 (which is the default value).

```
#
#    RecruitAFriend.MaxLevel
#        Description: Highest level up to which a character can benefit from the Recruit-A-Friend
#                     experience multiplier.
#        Default:     60

RecruitAFriend.MaxLevel = 60

#
#    RecruitAFriend.MaxDifference
#        Description: Highest level difference between linked Recruiter and Friend benefit from
#                     the Recruit-A-Friend experience multiplier.
#        Default:     4

RecruitAFriend.MaxDifference = 4
```

## Available commands

- `.recruit add <characterName>`. Ex: `.recruit add Pango`
- `.recruit reset`. Ex. `.recruit reset`
- `.recruit view`. Ex. `.recruit view`

## To recruit

![WoWScrnShot_062121_144533](https://user-images.githubusercontent.com/2810187/122805785-e8111a00-d29f-11eb-8cc9-d4c2269ac318.jpg)

![WoWScrnShot_062121_144620](https://user-images.githubusercontent.com/2810187/122805820-f19a8200-d29f-11eb-947d-059ab016a779.jpg)

Then, you have to close the game completely. Because it must reload the player's auth table information.

![WoWScrnShot_062121_144734](https://user-images.githubusercontent.com/2810187/122805976-23abe400-d2a0-11eb-8645-150c4e48e4b8.jpg)

![WoWScrnShot_062121_144751](https://user-images.githubusercontent.com/2810187/122805986-26a6d480-d2a0-11eb-8c9f-e3e293cd13f5.jpg)

## New command: `.recruit view`.

![WoWScrnShot_061823_212307](https://github.com/azerothcore/mod-individual-xp/assets/2810187/009a2418-5e20-4eeb-abfe-4a62d20e539d)

![WoWScrnShot_061823_212233](https://github.com/azerothcore/mod-individual-xp/assets/2810187/8ee4552a-7182-4142-8a1f-d21a2aaa1360)

## Credits

- [Nefertum](https://gitlab.com/Nefertum) (module author)
- [Stevej / Pangolp](https://github.com/pangolp) (module author)
- [AzerothCore: repository](https://github.com/azerothcore)
- [website](http://azerothcore.org/)
- [Discord](https://discord.gg/PaqQRkd)
