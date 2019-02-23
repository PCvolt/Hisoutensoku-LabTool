# Hisoutensoku-LabTool
Touhou 12.3 - Hisoutensoku: SWRSToys module for a better labbing experience

As of now, the features are:
- Display of **grazing** state in transparent
- Display of **CH** state in red
- Display of **invulnerability** state in blue (do not use it as a reference for DP moves, they display strange behaviour)

- **Gaps** in a blockstring (but cannot display 1F gaps as per an error of the FF_GUARD flag) displayed in a windows console.
- **Frame advantage** if at the end of the blockstring both characters on ground displayed in the same windows console.

- **Position management**: 5 hardcoded positions + 1 customizable position, press * to register the position and press 4, 1, 2, 3, 6 or 5 with / to reposition.

The compilation is performed with the following command line: `g++ -m32 LabTool.cpp -shared -o LabTool.dll`
