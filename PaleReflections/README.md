# *Pale Reflections*

<img src=".\img\palereflection_itch.gif"/>


Pale Reflections is a 3D first person survival horror experience featuring the use of mirrors and reflections as the major mechanic of the game. 
It was created in 8 weeks as a school project with a team size of 6 persons, 3 programmers and 3 artists.
It's stealth/horror game where you only see the monsters through the mirrors.

To make the reflections in the mirrors we changed a lot of settings in Unreal Engine
Regarding raycasting and visibility until we found a setup that worked for us. We started out using a capturing camera and and some logic for turning stuff on and off depending on if the player could see the mirror object. But that system was a bit inflexible regarding materials and capturing the game world multiple times per frame was very expensive. We found out that using Raytracing for reflections worked quite well for us after some experimenting, and allowed the artist to have more freedom in the level design. Of curse with the drawback of requiring newer and higher end graphic cards to run the game well. But the performance cost was not higher than the other system, in many cases it gave better performance, and it required less setup and adjustment during level design.

My primary role was building the main enemy AI and the roof enemy logic.
The main monster has sight, hearing, patrolling and different states
affecting speed, animations and behavior.
The AI states was done using blueprints and the different Unreal Engine tools
for pathfinding and behaviors.

It was a challenge getting the enemy to be the right kind of stupid but still scary. And to design the levels in a way that clued the player to use our tools for distraction and weapons the way they were designed.
Even communicating that there where invisible enemies, only visible in the mirror was a challenge and in my opinion still needed more work to really be polished. But we got a lot of positive feedback from playtesters, finding that its scary. And i still like the concept of the game.


<img src=".\img\pr_smoke.webp"/>

I worked on the Roof Cave in setup, made the smoke particle systems and trigger events and made the block out design for that part of the level.

<img src=".\img\pr_vials.webp"/>

I also made the particle systems and shaders for the vial explosions, and blood effects.

And all the integration with the main enemy AI of the above.
The work also involved talking to the creature artist and planning 
for the different animations needed and possible behavior.

<img src=".\img\pr_death.webp"/>


Me and the creature designer, Rasmus Andersson, also created the death cutscene together.

And I made and implemented the blood camera shader using Unreals node material system and a simple damage system for the player and monsters.


[See the game on Itch](https://yrgo.itch.io/pale-reflections)

**CREDITS**

Niklas Fischer (programmer, animation)

Billy Becker (programmer, UI design)

Freddy Erdal (programmer, sound design, voice acting (monster))

Aleksa Kleine (artist, textures, props)

Anastasia Marchevskaia (artist, textures, environment, props)

Rasmus Andersson (artist, monster design, animation)