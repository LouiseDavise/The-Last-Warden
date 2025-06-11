// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/ModeSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.hpp"
#include "Scene/SettingScene.hpp"
#include "Scene/LeaderBoardScene.hpp"
#include "Scene/AuthScene.hpp"
#include "Scene/NewPlayerScene.hpp"
#include "Scene/OldPlayerScene.hpp"

int main(int argc, char **argv)
{
	Engine::LOG::SetConfig(true);
	Engine::GameEngine &game = Engine::GameEngine::GetInstance();

	game.AddNewScene("start", new StartScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose-scene", new LoseScene());
	game.AddNewScene("win-scene", new WinScene());
	game.AddNewScene("mode-select", new ModeSelectScene());
	game.AddNewScene("setting-scene", new SettingScene());
	game.AddNewScene("leaderboard-scene", new LeaderBoardScene());
	game.AddNewScene("auth-scene", new AuthScene());
	game.AddNewScene("new-player-scene", new NewPlayerScene());
	game.AddNewScene("old-player-scene", new OldPlayerScene());

	game.Start("play", 60, 1920, 1280);
	return 0;
}
