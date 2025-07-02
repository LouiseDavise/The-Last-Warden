// ===================================================
//  Copyright (c) 2025 Louise Davise & Amartyananda Chang
//  All rights reserved.
//
//  This source code is part of a student project.
//  Unauthorized copying, distribution, or modification
//  of this file is strictly prohibited.
//
//  Created by: Louise Davise & Amartyananda Chang
// ===================================================

#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/ModeSelectScene.hpp"
#include "Scene/SettingScene.hpp"
#include "Scene/LeaderBoardScene.hpp"
#include "Scene/AuthScene.hpp"
#include "Scene/RegisterScene.hpp"
#include "Scene/LoginScene.hpp"
#include "Scene/SelectHeroScene.hpp"
#include "Scene/SelectCompanionScene.hpp"
#include "Scene/ScoreScene.hpp"
#include "Scene/LogoScene.hpp"

int main(int argc, char **argv)
{
	Engine::LOG::SetConfig(true);
	Engine::GameEngine &game = Engine::GameEngine::GetInstance();

	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("mode-select", new ModeSelectScene());
	game.AddNewScene("setting-scene", new SettingScene());
	game.AddNewScene("leaderboard-scene", new LeaderBoardScene());
	game.AddNewScene("auth-scene", new AuthScene());
	game.AddNewScene("register-scene", new RegisterScene());
	game.AddNewScene("login-scene", new LoginScene());
	game.AddNewScene("select-hero", new SelectHeroScene());
	game.AddNewScene("select-companion", new SelectCompanionScene());
	game.AddNewScene("score-scene", new ScoreScene());
	game.AddNewScene("splash-scene", new LogoScene());
	game.Start("splash-scene", 60, 1920, 1280);
	return 0;
}
