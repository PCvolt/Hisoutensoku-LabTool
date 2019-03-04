# Hisoutensoku-LabTool
## DOWNLOAD and INSTALLATION
Download here: https://www.mediafire.com/file/kvt97b5djlts3fv/LabTool.rar/file

1. Create a `LabTool` folder under the `C:\...\Touhou 12.3 - Hisoutensoku\Modules`, and put the downloaded files.
2. In the root folder of Hisoutensoku, open `SWRSToys.ini` and add a line `LabTool = Modules/LabTool/LabTool.dll`.
3. Profit.

## FEATURES
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience

As of now, the features are:
- Display of **grazing** (transparent), **CH** (red), **invulnerability** state in (blue) (do not trust it for DP moves, as they are strangely hardcoded).
- **Trade-mash time** in a blockstring and between two hits (cannot display instant-frame possible mashes, such as tenshi j5A j2B) displayed in a windows console.
- **Frame advantage** if at the end of the blockstring both characters on ground displayed in the same windows console.
- **Highjump cancel advantage** allows you to see how + you are if you decide to reset from a bullet.
- **Position management**: 5 hardcoded positions + 1 position customizable with the save key, press 4, 1, 2, 3, 6 or 5 with the reset key. You can set your own keys in the .ini file.
- **Skills reset** With a key customizable in the .ini, reset the levels and the skills you used to default.

## THE CODE
Any help is welcome.
Visual Studio seems mandatory for the output .dll to be as portable as possible.
