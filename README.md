# Ultralight-Xenko_Integration
Provides a wrapper dll for ultralight to be used in Xenko, along with a Base Script for hosting the ultralight view.
This is currently in a prototype phase. Not all features are implemented yet.

Instructions:

Ultralight Dll:
1. Requires Cmake (latest) and Visual Studio 2019
2. Download and extract the Ultralight SDK (https://github.com/ultralight-ux/ultralight) and also take note of the licensing of the framework (https://ultralig.ht/)
3. Point Cmake to the UltralightDll directory and configure it to use VS2019 + x64.
4. Add the necessary cmake cache entries for:
	- UltralightSDKIncludes
	- UltralightSDKLibs
	- UltralightSDKBinaries
5. Build using Cmake. Open the resulting solution file and build the Dlls.

Xenko Script:
1. BaseUltralightUI is a SyncScript. It expects to be attached to an Entity with a UI Component.
2. Also included is UltralightPage.xkuipage. The UI Component should reference this UI Page. Its simply a Grid with a child ImageElement.
3. Copy all output files from Ultralight Dll to your xenko project's output folder, under a /dll/ folder. These files are referenced from the SyncScript.
4. In the xenko editor, select the BaseUltralightUI component and set the AssetPath and HtmlFile properties.
5. I left mouse event handling to the user. The dll methods are already in place to handle them, you just need to hook up to them in your syncscript.




Todo:
- Keyboard events
- Passing data between both environments
- Make the install easier.
- Provide more documentation