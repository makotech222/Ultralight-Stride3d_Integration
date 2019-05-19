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
	void Update() {
		_renderer->Update();
	}
	void Render() {
		_renderer->Render();
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
		*data = static_cast<unsigned char*>(bitmap->raw_pixels());
	}

	__declspec(dllexport) void ExecuteMouseEvent(int viewId, int button, int type, int x, int y)
	{
		_app->ExecuteMouseEvent(viewId, button, type, x, y);
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