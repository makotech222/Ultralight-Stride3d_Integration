// UltralightLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Ultralight/Ultralight.h>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include "FileSystemBasic.h"
#include <vector>
#include <map>

using namespace ultralight;
using namespace std;

class UltralightApp {
	RefPtr<Renderer> _renderer;
	map<int, RefPtr<View>> _views;
	map<int, unsigned char*> _rgbaTextures;
	int _id;

public:
	UltralightApp(const char* assetDir) {
		ultralight::Platform& platform = Platform::instance();
		auto cfg = platform.config();
		cfg.use_bgra_for_offscreen_rendering = true;
		platform.set_file_system(new FileSystemBasic(assetDir));
		platform.set_config(cfg);
		_id = 0;
		_renderer = Renderer::Create();
	}

	virtual ~UltralightApp() {
		_views.clear();
		_renderer = nullptr;
	}

	int CreateView(int width, int height, const char* htmlPath) {
		auto view = _renderer->CreateView(width, height, true);
		auto url = ultralight::String::String(htmlPath);
		view->LoadURL(url);
		while (view->is_loading() == false)
			this->Update();
		_views.insert(make_pair(_id++, view));
		return _id;
	}

	void ReloadView(int viewId, const char* htmlPath) {
		auto view = _views[viewId];
		auto url = ultralight::String::String(htmlPath);
		view->LoadURL(url);
		while (view->is_loading() == false)
			this->Update();
	}

	void ExecuteMouseEvent(int viewId, int button, int type, int x, int y) {
		auto m = ultralight::MouseEvent();
		m.button = static_cast<ultralight::MouseEvent::Button>(button);
		m.type = static_cast<ultralight::MouseEvent::Type>(type);
		m.x = x;
		m.y = y;
		_views[viewId]->FireMouseEvent(m);
	}
	void ExecuteKeyboardEvent(int viewId, uintptr_t wParam, intptr_t lParam, int type) {
		auto m = ultralight::KeyEvent(static_cast<ultralight::KeyEvent::Type>(type), wParam, lParam, false);
		_views[viewId]->FireKeyEvent(m);
	}
	void ExecuteScrollEvent(int viewId, int type, int deltax, int deltay) {
		auto m = ultralight::ScrollEvent();
		m.type = static_cast<ultralight::ScrollEvent::Type>(type);
		m.delta_x = deltax;
		m.delta_y = deltay;
		_views[viewId]->FireScrollEvent(m);
	}
	void Update() {
		_renderer->Update();
	}
	void Render() {
		_renderer->Render();
	}
	unsigned char* GetRGBATexture(int viewId) {
		auto view = _views[viewId];
		auto bitmap = view->bitmap();
		if (_rgbaTextures.find(viewId) == _rgbaTextures.end()) {
			_rgbaTextures.insert(make_pair(viewId, new unsigned char[bitmap->size()]));
		}
		auto input = static_cast<unsigned char*>(bitmap->raw_pixels());
		ConvertTextureToXenkoFormat(input, bitmap->width(), bitmap->height(), _rgbaTextures[viewId]);
		return _rgbaTextures[viewId];
	}
	void DisposeView(int viewId) {
		auto view = _views[viewId];
		view->Release();
		view = nullptr;
		_views.erase(viewId);
	}

	RefPtr<Bitmap> GetViewBitmap(int viewId) {

		auto view = _views[viewId];
		return view->bitmap();
	}
private:
	void ConvertTextureToXenkoFormat(unsigned char* input, int pixel_width,
		int pixel_height, unsigned char* output)
	{
		int offset = 0;

		for (int y = 0; y < pixel_height; y++) {
			for (int x = 0; x < pixel_width; x++) {
				output[offset] = input[offset + 2];
				output[offset + 1] = input[offset + 1];
				output[offset + 2] = input[offset + 0];
				output[offset + 3] = input[offset + 3];

				offset += 4;
			}
		}
	}
};
extern "C"
{
	unique_ptr<UltralightApp> _app;
	__declspec(dllexport) void InitializeApp(const char* assetDir)
	{
		if (_app != nullptr)
			return;
		_app = make_unique<UltralightApp>(assetDir);
	}

	__declspec(dllexport) int CreateView(int width, int height, const char* htmlPath)
	{
		return _app->CreateView(width, height, htmlPath);
	}

	__declspec(dllexport) void Update()
	{
		_app->Update();
	}

	__declspec(dllexport) void Render()
	{
		_app->Render();
	}

	__declspec(dllexport) void ReloadView(int viewId, const char* htmlPath)
	{
		_app->ReloadView(viewId, htmlPath);
	}
	
	__declspec(dllexport) void GetViewBitmap(int viewId, unsigned char** data, int* size)
	{
		auto bitmap = _app->GetViewBitmap(viewId);
		*size = bitmap->size();
		*data = _app->GetRGBATexture(viewId);
	}

	__declspec(dllexport) void ExecuteMouseEvent(int viewId, int button, int type, int x, int y)
	{
		_app->ExecuteMouseEvent(viewId, button, type, x, y);
	}
	__declspec(dllexport) void ExecuteKeyboardEvent(int viewId, uintptr_t wParam, intptr_t lParam, int type)
	{
		_app->ExecuteKeyboardEvent(viewId, wParam, lParam, type);
	}
	__declspec(dllexport) void ExecuteScrollEvent(int viewId, int type, int deltax, int deltay)
	{
		_app->ExecuteScrollEvent(viewId, type, deltax, deltay);
	}
	__declspec(dllexport) void DisposeView(int viewId)
	{
		_app->DisposeView(viewId);
	}
	__declspec(dllexport) void Dispose()
	{
		_app = nullptr;
	}
}