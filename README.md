# Hisoutensoku-LabTool
## DOWNLOAD and INSTALLATION
Download here: https://github.com/PCvolt/Hisoutensoku-LabTool/releases/download/1.0/LabTool.rar

1. Create a `LabTool` folder under the `C:\...\Touhou 12.3 - Hisoutensoku\Modules`, and put the downloaded files.
2. In the root folder of Hisoutensoku, open `SWRSToys.ini` and add a line `LabTool = Modules/LabTool/LabTool.dll`.
3. Profit.

## FEATURES
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience in Practice Mode, VS Player Mode and Replay Mode.

### STATES AND POSITIONS
- Display of **grazing** in *transparent*, **CH** in *red*, **invulnerability** in *blue* (do not trust it for DP moves, as they are strangely hardcoded), **guard unavailable** in *purple*.
- **Position management**: 5 hardcoded positions + 1 position customizable with the save key, press 4, 1, 2, 3, 6 or 5 with the reset key. You can set your own keys in the .ini file.
### FRAMEDATA (Displayed on windows console)
- **Gap time** in a blockstring, it displays the number of idle frames the defender comes back to between the first and second hit. If nothing is displayed, the string is tight, else, it is safely mashable by the amount displayed on the console. "Xf gap gets beaten by Xf move, and trades with X+1f move" is how you should interpret it.
- **Frame advantage**: both characters must be on ground for the result to be displayed. 
- **Highjump cancel advantage**: p2 remains on the ground while p1 must hjc for the result to be displayed.

### AUTOMATED MACROS
With the event of SokuEngine and PracticeEX, macros were removed as the aforementioned tools are able to perform any kind of macros. Only Labtool 1.0 (not stable) version featured macros.

### MISCELLANEOUS
- **Skills reset** with a key customizable in the .ini, reset the levels and the skills you used to default.

You can set the properties of the terminal to pop up at a fixed place and change its default size.

### SWRSTOYS
You can find the series of SWRSToys mods here, written and compiled by the community: https://github.com/delthas/SWRSToys
