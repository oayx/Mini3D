
#include "runtime/Application.h"
#include "FighterApp.h"

DC_BEGIN_NAMESPACE
class MyMain : public MainComponent
{
	friend class GameObject;

	FighterApp game;
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
		window_desc.name = "pc";
		window_desc.width = 507;
		window_desc.height = 900;
		window_desc.fps = 60;
		window_desc.antiAlias = 4;
		window_desc.sync = 0;
		window_desc.flags = WindowFlag::Resizable | WindowFlag::MouseClip;
		descs.Add(window_desc);
	}
};
DC_END_NAMESPACE
DLCARE_MAIN(MyMain, false);