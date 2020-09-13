# Ultralight-Stride_Integration
Provides a Base Script for hosting the ultralight view. Intended to be used with ImpromptuNinjas.UltralightSharp C# wrapper.
This is currently in a prototype phase. Not all features are implemented yet.

Instructions:

Stride Script:
1. Import the BaseUltralightUI script into your project.
2. Import ImpromptuNinjas.UltralightSharp NuGet library.
3. Import or recreate the provided prefab and UIPage files. The UI Page is just a Grid Element with an Image Element nested, with specific names. The prefab is just an entity containing the UI Page.
4. Create a new script that inherits from BaseUltralightUI, and provide your own AssetPath and HTMLFile properties. Implement Start() and Update(); make sure to call base.Start() and base.Update() appropriately.
5. See BaseUltralightUI for example of calling into js code.

Todo:
- Call C# methods from JS.