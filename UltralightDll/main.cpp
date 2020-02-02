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
#include <JavaScriptCore/JavaScript.h>
#include <AppCore/JSHelpers.h>

using namespace ultralight;
using namespace std;
typedef void(__stdcall* CSharpCallBack)(const char*, const char*);

class UltralightApp : public LoadListener {
private:
	RefPtr<Renderer> _renderer;
	map<int, RefPtr<View>> _views;
	bool _isReady = false;
	map<std::string, CSharpCallBack> _callbacks;
	int _id;

public:
	UltralightApp(const char* assetDir) {
		ultralight::Platform& platform = Platform::instance();
		auto cfg = platform.config();
		platform.set_file_system(new FileSystemBasic(assetDir));
		platform.set_config(cfg);
		_id = 0;
		_renderer = Renderer::Create();
	}

	virtual ~UltralightApp() {
		_views.clear();
		_callbacks.clear();
		_renderer = nullptr;
	}

	virtual void OnDOMReady(View* caller) override
	{
		_isReady = true;
	}

	int CreateView(int width, int height, const char* htmlPath) {
		_isReady = false;
		auto view = _renderer->CreateView(width, height, true);
		view->set_load_listener(this);
		auto url = ultralight::String::String(htmlPath);
		view->LoadURL(url);
		while (_isReady == false) {
			this->Update();
		}
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
	void JavascriptEval(int viewId, const char* jsFunction) {
		JSContextRef myContext = _views[viewId]->js_context();
		SetJSContext(myContext);
		auto a = JSEval(jsFunction);
	}

	void ExecuteJSFunction(int viewId, const char* function, const char* json) {
		JSContextRef myContext = _views[viewId]->js_context();
		SetJSContext(myContext);

		JSObject global = JSGlobalObject();
		JSFunction func = global[function];
		if (func.IsValid()) {
			JSValue result = func({ json });
		}
	}

	void WriteToFile(std::string text) {
		ofstream myfile;
		myfile.open("example.txt");
		myfile << text;
		myfile.close();
	}
	void JSCallbackFunction(const JSObject& thisObject, const JSArgs& args, std::string funcName)
	{
		auto callback = _callbacks[funcName];
		std::string argJson = "{";
		for (size_t i = 0; i < args.size(); i++) {
			const char* val = ultralight::String(args[i].ToString()).utf8().data();
			char name = (char)('a' + i);
			argJson += '"';
			argJson += name;
			argJson += '"';
			argJson += ":";
			argJson += '"';
			argJson += val;
			argJson += '"';
			argJson += ',';
		}
		argJson += '}';
		callback(funcName.c_str(), argJson.c_str());
	}

	void RegisterEventHandler(int viewId, const char* funcName, CSharpCallBack func) {
		JSContextRef myContext = _views[viewId]->js_context();
		SetJSContext(myContext);
		JSObject global = JSGlobalObject();
		_callbacks.insert(make_pair(std::string(funcName), func));

		global[funcName] = (JSCallback)std::bind(&UltralightApp::JSCallbackFunction, this, std::placeholders::_1, std::placeholders::_2, std::string(funcName));
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
	__declspec(dllexport) void ExecuteKeyboardEvent(int viewId, uintptr_t wParam, intptr_t lParam, int type)
	{
		_app->ExecuteKeyboardEvent(viewId, wParam, lParam, type);
	}
	__declspec(dllexport) void ExecuteScrollEvent(int viewId, int type, int deltax, int deltay)
	{
		_app->ExecuteScrollEvent(viewId, type, deltax, deltay);
	}
	__declspec(dllexport) void JavascriptEval(int viewId, const char* jsFunction)
	{
		_app->JavascriptEval(viewId, jsFunction);
	}
	__declspec(dllexport) void ExecuteJSFunction(int viewId, const char* function, const char* json)
	{
		_app->ExecuteJSFunction(viewId, function, json);
	}
	__declspec(dllexport) void RegisterEventHandler(int viewId, const char* funcName, CSharpCallBack func)
	{
		_app->RegisterEventHandler(viewId, funcName, func);
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