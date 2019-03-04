# Hisoutensoku-LabTool
## DOWNLOAD and INSTALLATION
Download here: http://www.mediafire.com/file/0xg1yhteu8591d5/LabTool.rar/file

1. Create a `LabTool` folder under the `C:\...\Touhou 12.3 - Hisoutensoku\Modules`, and put the downloaded files.
2. In the root folder of Hisoutensoku, open `SWRSToys.ini` and add a line `LabTool = Modules/LabTool/LabTool.dll`.
3. Profit.

## FEATURES
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience

As of now, the features are:
- Display of **grazing** state in transparent
- Display of **CH** state in red
- Display of **invulnerability** state in blue (do not use it as a reference for DP moves, they display strange behaviour)

- **Trade-mash time** in a blockstring (cannot display instant frame possible mashes, such as tenshi j5A j2B) displayed in a windows console.
- **Frame advantage** if at the end of the blockstring both characters on ground displayed in the same windows console.

- **Position management**: 5 hardcoded positions + 1 position customizable with the save key, press 4, 1, 2, 3, 6 or 5 with the reset key. You can set your own keys in the .ini file.

## THE CODE
Any help is welcome.
Visual Studio seems mandatory for the output .dll to be as portable as possible.
