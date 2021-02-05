[![Build Status](https://dev.azure.com/dservice/dservice/_apis/build/status/SMU-Guildhall-Doug-Service.Directed-Focus-Study-Template?branchName=main)](https://dev.azure.com/dservice/dservice/_build/latest?definitionId=1&branchName=main)
# Slay The Spire GUI
The goal of this project is to create a GUI for a card game. Card games require GUI features like drag and drop, scaling, and animations to feel good. In industry there have been more and more virtual card games like Hearthstone, Legends of Runeterra, and Gwent. Also, the GUI features card games require cover many areas that are common in all games. The artifact will be a rogue-like deckbuilding game like Slay the Spire. 
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
 
Give a link to a user guide page.
## Supported Platforms
### Windows 10
#### Prerequisites
In this section you must list everything required to build your executables
and tests. Provide links to where the software can be found.
Explain any issues that developers may run into installing prerequisites.
**When a developer clones your repository and installs the listed
prerequisites the build must be successful. [Graded]**
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
## Performance
What performance tools are you using? Where to get them?
## Testing
 * Open the project in Visual Studio and go to Test->Run All Tests
    * Hotkey Ctrl + R then A

