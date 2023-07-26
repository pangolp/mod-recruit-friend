# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# Módulo: Reclutar a un amigo

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/pangolp/mod-recruit-friend/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/pangolp/mod-recruit-friend)

## Nota importante

Recuerda, que dentro de la carpeta SQL, dentro de world, tenes una serie de textos, los cuales se van a añadir dentro de acore_string. El id, que se encuentra dentro del archivo, depende de la cantidad de modulos que tengas, por lo que antes de ejecutarlo, revisa los mismos, para no eliminar nada, que pueda estar siendo utilizado en otros modulos.

## ¿Para qué es el módulo?

El módulo permite a los jugadores reclutar a un amigo dentro del juego y obtener los beneficios del mismo.

**La vinculación se realiza a nivel de cuenta**

**Y no da recompensas adicionales, solo las originales. Las cuales son:**

- Experiencia de misión mejorada
- Mejora de la experiencia mediante la eliminación del npc
- Dale un nivel a un amigo, por cada nivel obtenido.
- Invoca a un amigo, una vez cada 60 minutos.

Espero que te sirva y puedas disfrutarlo.

Recuerde, que dentro del archivo worldserver.conf, usted dispone de unas variables, las cuales puede modificar, para de esa forma, extender los beneficios, mas alla del nivel 60 (que es el valor por defecto)

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

## Comandos disponibles

- `.recruit add <characterName>`. Ej: `.recruit add Pango`
- `.recruit reset`. Ej. `.recruit reset`
- `.recruit view`. Ej. `.recruit view`

## Para reclutar

![WoWScrnShot_062121_144533](https://user-images.githubusercontent.com/2810187/122805785-e8111a00-d29f-11eb-8cc9-d4c2269ac318.jpg)

![WoWScrnShot_062121_144620](https://user-images.githubusercontent.com/2810187/122805820-f19a8200-d29f-11eb-947d-059ab016a779.jpg)

Luego, tienes que cerrar el juego por completo. Porque debe de volver a cargar la información de la tabla auth del jugador.

![WoWScrnShot_062121_144734](https://user-images.githubusercontent.com/2810187/122805976-23abe400-d2a0-11eb-8645-150c4e48e4b8.jpg)

![WoWScrnShot_062121_144751](https://user-images.githubusercontent.com/2810187/122805986-26a6d480-d2a0-11eb-8c9f-e3e293cd13f5.jpg)

## Nuevo comando: `.recruit view`.

![WoWScrnShot_061823_212307](https://github.com/azerothcore/mod-individual-xp/assets/2810187/009a2418-5e20-4eeb-abfe-4a62d20e539d)

![WoWScrnShot_061823_212233](https://github.com/azerothcore/mod-individual-xp/assets/2810187/8ee4552a-7182-4142-8a1f-d21a2aaa1360)

## Créditos

- [Nefertum](https://gitlab.com/Nefertum) (autor del módulo)
- [Stevej / Pangolp](https://github.com/pangolp) (autor del módulo)
- [AzerothCore repositorio](https://github.com/azerothcore)
- [Website](http://azerothcore.org/)
- [Discord](https://discord.gg/PaqQRkd)
