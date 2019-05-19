using System;
using System.Linq;
using Xenko.Engine;
using Xenko.UI.Controls;
using Xenko.Graphics;
using Xenko.Rendering.Sprites;
using System.Runtime.InteropServices;

namespace UltralightUI
{
    public class BaseUltralightUI : SyncScript
    {
        #region Fields
        /// <summary>
        /// Should be only one renderer per Game.
        /// </summary>
        private static bool _rendererInitialized = false;
        private Texture _texture;
        private SpriteFromTexture _sprite;
        private ImageElement _imageElement;
        private int _width;
        private int _height;
        /// <summary>
        /// ID of view created by Ultralight.
        /// </summary>
        private int _viewId { get; set; }

        #endregion Fields

        #region Properties
        /// <summary>
        /// Full path to directory containing html files.
        /// </summary>
        public string AssetDirectory { get; set; } = "\\assets\\";

        /// <summary>
        /// File path to main html file. Should be inside the AssetDirectory folder.
        /// </summary>
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
            _width = (int)uiComponent.Resolution.X;
            _height = (int)uiComponent.Resolution.Y;
            _texture = Texture.New2D(this.GraphicsDevice, _width, _height, Xenko.Graphics.PixelFormat.R8G8B8A8_UNorm, TextureFlags.ShaderResource | TextureFlags.RenderTarget);
            _sprite = new SpriteFromTexture();
            if (!_rendererInitialized)
            {
                UltralightDll.InitializeApp(this.AssetDirectory);
                _rendererInitialized = true;
            }
            UltralightDll.CreateView(_width, _height, "file:///" + HtmlFile);
        }

        public override void Update()
        {
            UltralightDll.Update();
            UltralightDll.Render();
            IntPtr data;
            int size;
            UltralightDll.GetViewBitmap(_viewId, out data, out size);
            DataPointer dataPointer = new DataPointer(data, size);
            _texture.SetData(this.Game.GraphicsContext.CommandList, dataPointer);
            _sprite.Texture = _texture;
            _imageElement.Source = _sprite;
        }

        #endregion Methods
    }
    public class UltralightDll
    {
        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitializeApp(string assetDir);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateView(int width, int height, string htmlPath);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReloadView(int viewId, string htmlPath);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Update();

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Render();

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetViewBitmap(int viewId, out IntPtr data, out int size);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ExecuteMouseEvent(int viewId, int button, int type, int x, int y);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DisposeView(int viewId);

        [DllImport(@"\dlls\UltralightDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Dispose();
    }

    public struct UltralightMouseEvent
    {
        public int x;
        public int y;
        public UltralightButton button;
        public UltralightButtonEventType type;
        public UltralightMouseEvent(UltralightButton button, UltralightButtonEventType type, int x, int y)
        {
            this.x = x;
            this.y = y;
            this.button = button;
            this.type = type;
        }
    }

    public enum UltralightButton { None, Left, Middle, Right }

    public enum UltralightButtonEventType { MouseMove, MouseDown, MouseUp }
}