#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>
#include <iostream>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"
#include "TextObject.h"
#include "FpsComponent.h"
#include "InputManager.h"
#include "Command.h"

#include "PlayerComponent.h"
#include "PlayerStatsComponents.h"
#include "SteamAchievments.h"

// Sound system
#include "ServiceLocator.h"
#include "SDLSoundSystem.h"
#include "SoundSystem.h"

#include <filesystem>
namespace fs = std::filesystem;

// Sound IDs used in the game
enum SoundIds : dae::sound_id
{
	SND_MOVE   = 0,
	SND_DIE    = 1,
	SND_POINTS = 2,
};

static dae::GameObject* MakeLabel(dae::Scene& scene,
	const std::string& text,
	std::shared_ptr<dae::Font> font,
	float x, float y)
{
	auto obj = std::make_unique<dae::GameObject>();
	obj->SetLocalPosition(x, y);
	obj->AddComponent<dae::TextObject>(text, font);
	dae::GameObject* ptr = obj.get();
	scene.Add(std::move(obj));
	return ptr;
}

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	// background
	auto bg = std::make_unique<dae::GameObject>();
	bg->AddComponent<dae::TextureComponent>()->SetTexture("background.png");
	scene.Add(std::move(bg));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

	// FPS counter
	auto fpsObj = std::make_unique<dae::GameObject>();
	fpsObj->SetLocalPosition(5.f, 5.f);
	fpsObj->AddComponent<dae::TextObject>("0 FPS", font);
	fpsObj->AddComponent<dae::FpsComponent>();
	scene.Add(std::move(fpsObj));

	// Controls info (updated to mention sound keys)
	auto ctrl = std::make_unique<dae::GameObject>();
	ctrl->SetLocalPosition(10.f, 200.f);
	ctrl->AddComponent<dae::TextObject>(
		"P1: WASD=Move  X=Die(snd)  Z=+100pts(snd)  ||  P2: DPad=Move  A=Die(snd)  B=+100pts(snd)",
		font);
	scene.Add(std::move(ctrl));

	auto ctrl2 = std::make_unique<dae::GameObject>();
	ctrl2->SetLocalPosition(10.f, 220.f);
	ctrl2->AddComponent<dae::TextObject>(
		"Press [M] to play a sound directly via ServiceLocator",
		font);
	scene.Add(std::move(ctrl2));

	//make player 1
	auto char1 = std::make_unique<dae::GameObject>();
	char1->SetLocalPosition(180.f, 240.f);
	{
		auto* tex = char1->AddComponent<dae::TextureComponent>();
		tex->SetTexture("qbrt.png");
		tex->SetScale(0.05f);
	}
	auto* pPlayer1 = char1->AddComponent<dae::PlayerComponent>(3);
	dae::GameObject* pChar1 = char1.get();
	scene.Add(std::move(char1));

	//initialize player 2
	auto char2 = std::make_unique<dae::GameObject>();
	char2->SetLocalPosition(360.f, 240.f);
	{
		auto* tex = char2->AddComponent<dae::TextureComponent>();
		tex->SetTexture("qbrt.png");
		tex->SetScale(0.05f);
	}
	auto* pPlayer2 = char2->AddComponent<dae::PlayerComponent>(3);
	dae::GameObject* pChar2 = char2.get();
	scene.Add(std::move(char2));

	//Player 1 stats
	auto* livesDisp1 = MakeLabel(scene, "P1 Lives: 3", font, 10.f, 60.f);
	auto* scoreDisp1 = MakeLabel(scene, "P1 Score: 0", font, 10.f, 85.f);
	pPlayer1->AddObserver(livesDisp1->AddComponent<dae::LivesDisplayComponent>(0));
	pPlayer1->AddObserver(scoreDisp1->AddComponent<dae::ScoreDisplayComponent>(0));

	//Player 2 stats
	auto* livesDisp2 = MakeLabel(scene, "P2 Lives: 3", font, 10.f, 115.f);
	auto* scoreDisp2 = MakeLabel(scene, "P2 Score: 0", font, 10.f, 140.f);
	pPlayer2->AddObserver(livesDisp2->AddComponent<dae::LivesDisplayComponent>(1));
	pPlayer2->AddObserver(scoreDisp2->AddComponent<dae::ScoreDisplayComponent>(1));

	//Input
	auto& input = dae::InputManager::GetInstance();
	constexpr float speed = 100.f;
	constexpr float speed2 = speed * 2.f;

	// --- Player 1 input (keyboard) ---
	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0,-1,0 }, speed));
	input.BindKeyboardCommand(SDL_SCANCODE_S, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0, 1,0 }, speed));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ -1,0,0 }, speed));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 1,0,0 }, speed));
	input.BindKeyboardCommand(SDL_SCANCODE_X, dae::KeyState::Down, std::make_unique<dae::DieCommand>(pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_Z, dae::KeyState::Down, std::make_unique<dae::GainPointsCommand>(pChar1, 100));

	// --- Player 2 input (controller) ---
	input.BindControllerCommand(0, dae::Controller::Button::DPadUp,    dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0,-1,0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadDown,  dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0, 1,0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft,  dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ -1,0,0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 1,0,0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA,   dae::KeyState::Down, std::make_unique<dae::DieCommand>(pChar2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonB,   dae::KeyState::Down, std::make_unique<dae::GainPointsCommand>(pChar2, 100));

	input.BindKeyboardCommand(SDL_SCANCODE_M, dae::KeyState::Down,
		std::make_unique<dae::PlaySoundCommand>(SND_MOVE, 0.8f));
}

int main(int, char* [])
{
#ifdef USE_STEAMWORKS
	if (SteamAPI_Init())
	{
		dae::SteamAchievements::GetInstance().Init();
	}
#endif

#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if (!fs::exists(data_location))
		data_location = "../Data/";
#endif

	try
	{
	
#if _DEBUG
		dae::ServiceLocator::RegisterSoundSystem(
			std::make_unique<dae::LoggingSoundSystem>(
				std::make_unique<dae::SDLSoundSystem>()));
#else
		dae::ServiceLocator::RegisterSoundSystem(
			std::make_unique<dae::SDLSoundSystem>());
#endif

		// Register sound files (paths relative to the Data folder)
		auto& ss = dae::ServiceLocator::GetSoundSystem();
		ss.RegisterSound(SND_MOVE,   (data_location / "move.wav").string());
		ss.RegisterSound(SND_DIE,    (data_location / "die.wav").string());
		ss.RegisterSound(SND_POINTS, (data_location / "points.wav").string());

		dae::Minigin engine(data_location);
		engine.Run(load);

		dae::ServiceLocator::RegisterSoundSystem(nullptr);

	}
	catch (const std::exception& e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Engine Error", e.what(), nullptr);
	}

#ifdef USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif

	return 0;
}
