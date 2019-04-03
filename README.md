# Hisoutensoku-LabTool
## DOWNLOAD and INSTALLATION
Download here: https://www.mediafire.com/file/b7iz6f24n4bi2gj/LabTool.rar/file

1. Create a `LabTool` folder under the `C:\...\Touhou 12.3 - Hisoutensoku\Modules`, and put the downloaded files.
2. In the root folder of Hisoutensoku, open `SWRSToys.ini` and add a line `LabTool = Modules/LabTool/LabTool.dll`.
3. Profit.

## FEATURES
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience

### STATES AND POSITIONS
- Display of **grazing** in *transparent*, **CH** in *red*, **invulnerability** in *blue* (do not trust it for DP moves, as they are strangely hardcoded).
- **Position management**: 5 hardcoded positions + 1 position customizable with the save key, press 4, 1, 2, 3, 6 or 5 with the reset key. You can set your own keys in the .ini file.
### FRAMEDATA (Displayed on windows console)
- **Trademash time** in a blockstring and between two hits, even if instantly mashable strings. If nothing is displayed, the string is tight.
- **Frame advantage**: both characters must be on ground for the result to be displayed.
- **Highjump cancel advantage**: p2 remains on the ground while p1 must hjc for the result to be displayed.

### MISCELLANEOUS
- **Random CH** with a key customizable in the .ini, toggle on and off a random CH state to train CH combos on reaction. Untoggle the State Display for more realism.
- **Skills reset** with a key customizable in the .ini, reset the levels and the skills you used to default.

## THE CODE
Any help is welcome.
Use Visual Studio for compilation as .dll.

### KNOWN BUGS
- yuyuko 3Arb against standing is displayed as +12F while she is -12F. 
