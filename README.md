# Chronomancer Siege #

![Chronomancer Siege Banner](https://patricevignola.com/img/chronomancer-siege-banner.jpg "Chronomancer Siege")

## Important Links ##
* [Visit the website](https://patricevignola.com/project/chronomancer-siege)
* [Watch the video](https://www.youtube.com/watch?v=AFGh_mkoSgw)
* [Download the game](https://github.com/PatriceVignola/Builds/releases/download/windows/chronomancer-siege.zip)

## Description ##

Chronomancer Siege is a game I made in less than 10 weeks for the Ubisoft Game Lab Competition 2017. It's an online cooperative brawler with a touch of puzzles, in which you must play with time to defeat your enemies.

The players can record/rewind at any time during a fight and there's always a lot going on. Therefore, one of the biggest technical challenges of this game was to perfectly rewind the state of all actors while, at the same time, minimizing the network lag and making it look smooth for the player.

Overall, I think we did a good job: the skeleton bones rewind smoothly and accurately every frame, there is no noticeable lag over the network and the game state is never broken.

## Technology Stack ##
* Unreal Engine 4.14
* Visual Studio (C++)
* Wwise
* Perforce

## Features I implemented ##
* States Recording and Rewinding
* Bones Recording and Rewinding
* Network Replication and Optimization
* Game Loop
* Level Streaming
* Checkpoints System
* Enemy Waves System
* Pillar Puzzles System
* Death Rewind and Respawn
* Wwise Events Integration
* Build Automation

## Note ##
The version control software we used during the development of Chronomancer Siege is Perforce. Therefore, the entire commit history is on the Perforce server instead of git. Also, this repository contains only the code for the game; the assets belong to the artists and will not be migrated to the git repository.
