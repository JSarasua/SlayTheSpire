[![Build Status](https://dev.azure.com/dservice/dservice/_apis/build/status/SMU-Guildhall-Doug-Service.Directed-Focus-Study-Template?branchName=main)](https://dev.azure.com/dservice/dservice/_build/latest?definitionId=1&branchName=main)
# Slay The Spire-like GUI
The goal of this project is to create a GUI for a card game like [Slay the Spire](https://store.steampowered.com/app/646570/Slay_the_Spire/). Card games require GUI features like drag and drop, scaling, and animations to feel good. In industry there have been more and more virtual card games like Hearthstone, Legends of Runeterra, and Gwent. Also, the GUI features card games require cover many areas that are common in all games. The artifact will be a rogue-like deckbuilding game like Slay the Spire. 
## Unique Features
 * GUI
   * Widget Based
   * Builds from XML
   * Event System integration
   * Common UI features
 * Slay the Spire-like Game
    * Card Definitions using XML
    * Basic card game operations
    * Mouse controls
    * Slay the Spire-like fights 

## Supported Platforms
### Windows 10
#### Prerequisites
  * [Microsoft Visual Studio 2019](https://visualstudio.microsoft.com/downloads/)
  * [TinyXML2](http://grinninglizard.com/tinyxml/)
  * [fmod](https://www.fmod.com/)


## Building
 * Confirm Engine folder is linked to game
     * Set Linker->General->Additional Library Directories to $(SolutionDir)Code/;$(SolutionDir)../Engine/Code/


## Debugging
  * For the games configuration properties
      * Debugging->Command to **$(SolutionDir)Run/$(TargetFileName)**
      * Debugging->Working Directory to **$(SolutionDir)Run**

## Testing
 * Open the project in Visual Studio and go to Test->Run All Tests
    * Hotkey Ctrl + R then A

