#include "../../Common/Window.h"
#include "GameManager.h"
#include "AudioEngine.h"

using namespace NCL;
using namespace CSC8503;

void InitialiseAudioFiles() {
	AudioEngine::Instance()->Create2DStream(MENU_MUSIC, "menumusic.mp3");
	AudioEngine::Instance()->Create2DStream(GAME_MUSIC, "gamemusic.mp3");
	AudioEngine::Instance()->Create2DStream(SHOOT_SOUND, "paintFill.wav");
	AudioEngine::Instance()->Create2DStream(VICTORY_SOUND, "victorySound.wav");
}

int main() {
	
	InitialiseAudioFiles();
	Window* w = Window::CreateGameWindow("T2_Splatoon", 1280, 720);
	if (!w->HasInitialised()) { return -1; }

	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	GameManager* g = new GameManager();
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	while (w->UpdateWindow() && !g->IsQuit()) 
	{
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		
		if (dt > 0.1f) 
		{
			std::cout << "Skipping large time delta" << std::endl;
			continue;
		}
		
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR))	w->ShowConsole(true);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT))	w->ShowConsole(false);

		//w->SetTitle("Frame time:" + std::to_string(1000.0f * dt));
		w->SetTitle("T2_Splatoon");

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}