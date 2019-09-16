# Hisoutensoku-LabTool
## DOWNLOAD and INSTALLATION
Download here: https://github.com/PCvolt/Hisoutensoku-LabTool/releases/download/1.0/LabTool.rar

1. Create a `LabTool` folder under the `C:\...\Touhou 12.3 - Hisoutensoku\Modules`, and put the downloaded files.
2. In the root folder of Hisoutensoku, open `SWRSToys.ini` and add a line `LabTool = Modules/LabTool/LabTool.dll`.
3. Profit.

## FEATURES
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience

### STATES AND POSITIONS
- Display of **grazing** in *transparent*, **CH** in *red*, **invulnerability** in *blue* (do not trust it for DP moves, as they are strangely hardcoded), **guard unavailable** in *purple*.
- **Position management**: 5 hardcoded positions + 1 position customizable with the save key, press 4, 1, 2, 3, 6 or 5 with the reset key. You can set your own keys in the .ini file.
### FRAMEDATA (Displayed on windows console)
- **Gap time** in a blockstring, it displays the number of idle frames the defender comes back to between the first and second hit. If nothing is displayed, the string is tight, else, it is safely mashable by the amount displayed on the console. "Xf gap gets beaten by Xf move, and trades with X+1f move" is how you should interpret it.
- **Frame advantage**: both characters must be on ground for the result to be displayed. 
- **Highjump cancel advantage**: p2 remains on the ground while p1 must hjc for the result to be displayed.

### AUTOMATED MACROS (Work in Progress)
Only available in VS Player. Make sure to set two different keymappings to player 1 and player 2. Player 2 will do the following:
- tech (neutral, left, right, random)
- wakeup options (nothing, jump, highjump, backdash(not functional), mash4A, 623B, card)
- block first hit only (no block, block high, block low)
- BE on first block (no BE, BE2, BE1, (j)BE6, (j)BE4)

Please DO REPORT bugs to me.

### MISCELLANEOUS
- **Random CH** with a key customizable in the .ini, toggle on and off a random CH state to train CH combos on reaction. Untoggle the State Display for more realism.
- **Skills reset** with a key customizable in the .ini, reset the levels and the skills you used to default.

## THE CODE
Any help is welcome.
Use Visual Studio for compilation as .dll.

### KNOWN BUGS
- randomCH function completely disables the CH state for player 2.
- yuyuko 3Arb against standing is displayed as +12F while she is -12F.
- suika crouch in corner, youmu d22 is displayed as -17, but if youmu holds a direction she is displayed as +18.
