// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingScene.hpp"
#include "Scene/LeaderBoardScene.hpp"
#include "Scene/AuthScene.hpp"
#include "Scene/NewPlayerScene.hpp"
#include "Scene/OldPlayerScene.hpp"
#include "Scene/WelcomeScene.hpp"

int main(int argc, char **argv)
{
	Engine::LOG::SetConfig(true);
	Engine::GameEngine &game = Engine::GameEngine::GetInstance();

	// TODO HACKATHON-2 (2/3): Register Scenes here
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win-scene", new WinScene());
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("setting-scene", new SettingScene());
	game.AddNewScene("leaderboard-scene", new LeaderBoardScene());
	game.AddNewScene("auth-scene", new AuthScene());
	game.AddNewScene("new-player-scene", new NewPlayerScene());
	game.AddNewScene("old-player-scene", new OldPlayerScene());
	game.AddNewScene("welcome-scene", new WelcomeScene());

	// TODO HACKATHON-1 (1/1): Change the start scene
	game.Start("auth-scene", 60, 1600, 832);
	return 0;
}
