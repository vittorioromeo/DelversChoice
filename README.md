# DelversChoice
Game originally created for GGJ2015...

---

Delver's Choice
_______________

Global Game Jam 2015 Entry
Developed at GGJ2015 Catania

---------------

CREDITS 

Developer: Vittorio Romeo
2D Artist: Vittorio Romeo
Audio: Nicola Bombaci
Designer: Sergio Zavettieri
Additional help: Davide Iuffrida

http://vittorioromeo.info
http://nicolabombaci.com

---------------

INTRODUCTION

Delver's Choice is a fast-paced, simple and accessible, retro-styled dungeon crawler, that uses only four numeric keys and a simple RPG battle system The player is constantly faced with randomly-generated challenges - having to make a choice is a small period of time is the point of the game. Choices depend on your current statistics, on the enemies' statistics and the items in the environment. 

Will you be able to reach the hardest rooms of the dungeon?

---------------

BASICS

Delver's Choice is played using only the 1, 2, 3, and 4 numeric keys.

There are three modes of play:
1. Normal mode - the "official" way to play the game
2. Pratice mode - the reccommended way to learn the basics. The timer is disabled.
3. Hardcore mode - for those who desire a real challenge. Enemies are stronger and the timer is faster.

The goal of the game is going as far as possible in the endless dungeon.
Every room of the dungeon gives the player from two to four possible pathways.

Pathways may contain items or enemies.

To clear a pathway, the player has to collect the items or defeat the enemies.
Pathways can be selected with numeric keys.

Upon clearing a pathway, pressing its key again will advance the player one room further.

---------------

COMBAT

After selecting a pathway with an enemy in it, the player and the enemy will automatically fight in turns until one of them is dead.
The player and enemies share the same combat mechanics.

Every fighter has an amount of HPS (health points). 
When they reach 0, the fighter dies.
If the player dies, the run is over.

Every fighter has a weapon and an armor as well.

Weapons have ATK (attack points), ST (elements they're strong against) and WK (elements they're weak against).
Armors have DEF (defense points) and TY (element types they're made of).

The player also has persistent bonus stats: bonus ATK and bonus DEF.

Every turn, the attacking fighter deals his (ATK + bonus ATK) minus the enemy's (DEF + bonus DEF) points of damage to the enemy HPS.
Combat will happen automatically until one of the fighters is dead.

If the attacking fighter uses a weapon strong against one of the types the enemy armor has, he will deal bonus damage.
Likewise, a weapon weak against one of the elements will deal less damage.

---------------

ITEMS

Items can block pathways or can come in bags.

When an item blocks a pathway, you're forced to pick it up to continue.
When a bag block a pathway, you can freely browse the bag and decide to pick up any number of items from it.

Items can be weapons and armors, or potions that increment and/or decrease multiple stats.

