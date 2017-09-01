## GBAk
A complete and simplified Gameboy Advance development Kit.
Contains the binaries of a GCC ARM Cross-compiler. 
The arm-agb-elf-gcc targets the AGB processor of the GBA.  
This repository is free to use and free to modify.

#### Resources
The following links are excellent resources to learn more about 
the GBA hardware and all credit goes to the authors!
	http://www.coranac.com/tonc/text/
	http://www.komodoopenlab.com/pub/mirroredpages/gbaspecs/cowbitespec.html#REG_BG0
	http://cs.umw.edu/~finlayson/class/spring16/cpsc305/notes/

#### Setup Environment
The install script downloads and makes map editing tools 
created by github.com/IanFinlayson. 
- Set GBAK variable:
	sudo echo export GBAK=/path/to/gbak >> .bashrc
- Install tools:
	./install

#### Build
This also comes packaged with a Makefile to simplify compilation.
The Makefile assumes c source files are located inside a src/ directory, 
and looks for header inside a inc/ directory.
- First move source code into the inc and src directories
- To Build 
	cd path/to/gbak
	make
This will export demo.gba file that can be loaded by an emulator.
To change the output name, modify the OUT variable in the makefile.
If you are on linux you can install visualboyadvance with
	sudo apt-get install visualboyadvance
and use it to load the .gba file with
	gvba demo.gba
	
