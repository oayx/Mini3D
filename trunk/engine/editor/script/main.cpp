#include "engine_header.h"
#include "EditorMain.h"

DC_BEGIN_NAMESPACE
class MyMain : public MainComponent
{
	friend class GameObject;

	EditorMain game;
public:
	MyMain() {}
	~MyMain() {}

	virtual void Start()override
	{
		game.Start();
	}
	virtual void GetWindowDesc(Vector<WindowDesc>& descs)override
	{
		WindowDesc window_desc;
		window_desc.name = "Editor";
		window_desc.width = 0;
		window_desc.height = 0;
		window_desc.fps = 60;
		window_desc.antiAlias = 4;
		window_desc.sync = 1;
		window_desc.flags = WindowFlag::Resizable | WindowFlag::MaxSize;
		descs.Add(window_desc);
	}
};
DC_END_NAMESPACE
DLCARE_MAIN(MyMain, true);