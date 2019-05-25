// UltralightLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "main.h"


UltralightApp::UltralightApp(const char* assetDir) {
	ultralight::Platform& platform = Platform::instance();
	platform.set_file_system(new FileSystemBasic(assetDir));
	_id = 0;
	_renderer = Renderer::Create();
}

UltralightApp::~UltralightApp() {
	_views.clear();
	_renderer = nullptr;
}

//int UltralightApp::CreateView(int width, int height, const char* htmlPath) {
//
//	auto view = _renderer->CreateView(width, height, true);
//	auto url = ultralight::String::String(htmlPath);
//	view->LoadURL(url);
//	while (view->is_loading() == false)
//		this->Update();
//	_views.insert(make_pair(_id++, view));
//	return _id;
//}
//
//void UltralightApp::ReloadView(int viewId, const char* htmlPath) {
//
//	auto view = _views[viewId];
//	auto url = ultralight::String::String(htmlPath);
//	view->LoadURL(url);
//	while (view->is_loading() == false)
//		this->Update();
//}
//void UltralightApp::ExecuteMouseEvent(int viewId, ultralight::MouseEvent m) {
//
//	_views[viewId]->FireMouseEvent(m);
//}
//void UltralightApp::ExecuteKeyboardEvent(int viewId, ultralight::KeyEvent k) {
//	_views[viewId]->FireKeyEvent(k);
//
//}
//void UltralightApp::ExecuteScrollEvent(int viewId, ultralight::ScrollEvent s) {
//	_views[viewId]->FireScrollEvent(s);
//
//}
//void UltralightApp::Update() {
//	_renderer->Update();
//
//}
//void UltralightApp::Render() {
//	_renderer->Render();
//
//}
//JSValue UltralightApp::JavascriptEval(int viewId, const char* jsFunction) {
//	JSContextRef myContext = _views[viewId]->js_context();
//	SetJSContext(myContext);
//	return JSEval(jsFunction);
//}
//
//void UltralightApp::DisposeView(int viewId) {
//
//	auto view = _views[viewId];
//	view->Release();
//	view = nullptr;
//	_views.erase(viewId);
//}
//RefPtr<Bitmap> UltralightApp::GetViewBitmap(int viewId) {
//	auto view = _views[viewId];
//	return view->bitmap();
//
//}
//
