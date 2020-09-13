using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using ImpromptuNinjas.UltralightSharp.Enums;
using ImpromptuNinjas.UltralightSharp.Safe;
using Newtonsoft.Json;
using Renaissance.Components;
using Stride.Core;
using Stride.Engine;
using Stride.Graphics;
using Stride.Rendering.Sprites;
using Stride.UI.Controls;

namespace Renaissance.Gameplay.GUI
{
    public class BaseUltralightUI : StartupScript
    {
        #region Fields

        /// <summary>
        /// Should be only one renderer per Game.
        /// </summary>
        protected static Renderer _renderer = null;

        /// <summary>
        /// View created by Ultralight.
        /// </summary>
        protected View _view { get; set; }

        protected Texture _texture;
        protected SpriteFromTexture _sprite;
        protected ImageElement _imageElement;
        protected uint _width;
        protected uint _height;

        #endregion Fields

        #region Properties

        /// <summary>
        /// Full path to directory containing html files.
        /// </summary>
        [DataMemberIgnore]
        public string AssetDirectory { get; set; } = "\\assets\\";

        /// <summary>
        /// File path to main html file. Should be inside the AssetDirectory folder.
        /// </summary>
        [DataMemberIgnore]
        public string HtmlFile { get; set; } = "index.html";

        #endregion Properties

        #region Constructor

        #endregion Constructor

        #region Methods

        public override void Start()
        {
            base.Start();
            var uiComponent = Entity.Get<UIComponent>();
            _imageElement = uiComponent.Page.RootElement.VisualChildren.FirstOrDefault() as ImageElement;
            _width = (uint)uiComponent.Resolution.X;
            _height = (uint)uiComponent.Resolution.Y;
            _texture = Texture.New2D(this.GraphicsDevice, (int)_width, (int)_height, Stride.Graphics.PixelFormat.B8G8R8A8_UNorm, TextureFlags.ShaderResource | TextureFlags.RenderTarget);
            _sprite = new SpriteFromTexture();
            if (_renderer == null)
            {
                InitializeUltralight();
            }
            var session = new Session(_renderer, false, "");
            _view = new View(_renderer, _width, _height, true, session);
            _view.LoadUrl("file:///" + HtmlFile);

            bool loaded = false;
            _view.SetFinishLoadingCallback((data, caller, frameId, isMainFrame, url) =>
            {
                loaded = true;
            }, default);

            while (!loaded)
            {
                _renderer.Update();
                _renderer.Render();
            }
        }

        ~BaseUltralightUI()
        {
            _view?.Dispose();
            _renderer?.Dispose();
        }

        protected void InitializeUltralight()
        {
            // setup logging
            LoggerLogMessageCallback cb = LoggerCallback;
            Ultralight.SetLogger(new Logger { LogMessage = cb });

            using var cfg = new Config();

            var cachePath = Path.Combine(AssetDirectory, "Cache");
            cfg.SetCachePath(cachePath);

            var resourcePath = Path.Combine(AssetDirectory, "resources");
            cfg.SetResourcePath(resourcePath);

            cfg.SetUseGpuRenderer(false);
            cfg.SetEnableImages(true);
            cfg.SetEnableJavaScript(true);

            AppCore.EnablePlatformFontLoader();
            AppCore.EnablePlatformFileSystem(AssetDirectory);
            _renderer = new Renderer(cfg);
        }

        protected static void LoggerCallback(LogLevel logLevel, string msg)
        {
            Console.WriteLine($"{logLevel.ToString()}: {msg}");
        }

        public virtual void Update()
        {
            if (_renderer == null)
                return;
            string exception = "";
            _view.EvaluateScript($"console.log('hi')", out exception);

            _renderer.Update();
            _renderer.Render();

            var surface = _view.GetSurface();
            var bitmap = surface.GetBitmap();
            var pixels = bitmap.LockPixels();
            DataPointer dataPointer = new DataPointer((IntPtr)pixels, (int)bitmap.GetHeight() * (int)bitmap.GetWidth() * (int)bitmap.GetBpp());
            _texture.SetData(this.Game.GraphicsContext.CommandList, dataPointer);
            _sprite.Texture = _texture;
            _imageElement.Source = _sprite;

            bitmap.UnlockPixels();
            bitmap.Dispose();
        }

        public void UpdateInputs(ComponentUI ui)
        {
            foreach (var e in ui.MouseEvents)
            {
                _view.FireMouseEvent(e);
            }
            foreach (var e in ui.ScrollEvents)
            {
                _view.FireScrollEvent(e);
            }
            foreach (var e in ui.KeyboardEvents)
            {
                _view.FireKeyEvent(e);
            }
        }

        #endregion Methods

        #region Classes
        public class ComponentUI
        {
            
            public List<MouseEvent> MouseEvents { get; set; } = new List<MouseEvent>();
            public List<KeyEvent> KeyboardEvents { get; set; } = new List<KeyEvent>();
            public List<ScrollEvent> ScrollEvents { get; set; } = new List<ScrollEvent>();
            
        }
        #endregion Classes
    }
}