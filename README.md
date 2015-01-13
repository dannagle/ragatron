# Ragatron

Ragatron is a Windows-only (for now) open source hacking tool for HTML5 games developed by [Lost Decade Games](http://www.lostdecadegames.com). It specifically targets [A Wizard's Lizard](http://www.wizardslizard.com) and [Lava Blade](http://www.lavablade.com).

![AWL hack screenshot](hacked_awl_crop.jpg)

**Ragatron is neither endorsed nor supported by Lost Decade Games. ** Use at your own risk! However, *you* can support Lost Decade Games by purchasing copies of their games to hack with this tool. ;-)

## Hacks for A Wizard's Lizard
The only supported build is Humble Bundle version 2014-10-06

![Ragatron AWL hack](ragatron_screenshot_awl.png)

- Start with 1 million gold
- Start with 5,000 health
- Start with 5,000 soul orbs
- Fast Soul Orb Cooldown
- Fast Dash Cooldown
- Fast Totem Cooldown
- Fast Skyrocket
- Fast Quad Spear
- Expose Developer Tools

## Hacks for Lava Blade
The only supported build is the version on Humble Bundle.

![Ragatron Lava Blade hack](ragatron_screenshot_lavablade.png)

- Healing at camp costs 1 gold
- Lars has extra movement
- Luna has extra attack
- Seraph has extra health
- Sobek has extra magic
- Blitz has extra movement, attack, and health
- Earn more gold
- Expose Developer Tools

### Instructions


Make your media edits in the generated work directory. DO NOT MOVE ANY FILES! Instead, directly edit the original files.  All the super-critical files have been secured.

If you would like to cheat in the game, those are available through the Ragatron interface. It will drop in your cheats at the last minute from its own secure copy of the game.

If you mess something up, just click "Unpack" again to undo your changes. Ragatron does not modify any original files.


### Dependencies

- [Qt 5.3](http://www.qt.io/) SDK. The tested versions are compiled using MinGW. Qt version 5.4 should work too.

- [QuaZIP](http://quazip.sourceforge.net/) is needed to unpackage and repackage the game binaries. It is tricky to setup. Therefore, pre-compiled shared libraries (for Mac and Windows) are inside this repo (as well as the zip source). You will need to modify the ragatron.pro file to support your setup.

### Web Site

Official web site is [Ragatron.com](http://ragatron.com/)


### Todo

- Finish Mac support. It currently just launches. It does nothing useful.
- Expand to support other games
- Auto-detect the HTML5 binary joins (currently hard-coded).
- More cheats!
- Mod export/import support


### License

GPL v2 or later


###Author
&copy; 2015 Dan Nagle -  [@NagleCode](http://twitter.com/NagleCode) - [DanNagle.com](http://DanNagle.com)
