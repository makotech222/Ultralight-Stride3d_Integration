
#include "FileSystemBasic.h"
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <Ultralight/Ultralight.h>
#include <vector>

using namespace ultralight;

class UltralightApp {

private:
	RefPtr<Renderer> _renderer;
	std::map<int, RefPtr<View>> _views;
	int _id;
public:
	UltralightApp(const char* assetDir);

	~UltralightApp();

	//int CreateView(int width, int height, const char* htmlPath);

	//void ReloadView(int viewId, const char* htmlPath);

	//void ExecuteMouseEvent(int viewId, ultralight::MouseEvent m);
	//void ExecuteKeyboardEvent(int viewId, ultralight::KeyEvent k);
	//void ExecuteScrollEvent(int viewId, ultralight::ScrollEvent s);
	//void Update();
	//void Render();
	//JSValue JavascriptEval(int viewId, const char* jsFunction);
	//void DisposeView(int viewId);
	//RefPtr<Bitmap> GetViewBitmap(int viewId);
};