#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#include "SteamAchievments.h"
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
#include "ServiceLocator.h"
#include "SDLSoundSystem.h"
#include "SoundSystem.h"
#include "TextureAtlasComponent.h"
#include "GridComponent.h"
#include "PlayerComponent.h"
#include "PlayerCommands.h"
#include "LevelObserver.h"
#include "MenuComponent.h"
#include "TimedSceneTransition.h"
#include "CoilyComponent.h"
#include "CoilyCommands.h"
#include "PauseComponent.h"
#include "GlobalCommands.h"
#include "HUDComponent.h"
#include "GameOverComponent.h"
#include "GameSession.h"
#include "Controller.h"

#include <filesystem>
namespace fs = std::filesystem;

enum SoundIds : dae::sound_id
{
	SND_JUMP = 0,
	SND_SWEAR = 1,
	SND_ROUND_COMPLETE = 2,
	SND_CHANGE_SELECTION = 3,
};

const glm::ivec2 WinDim{ 1024, 576 };
const glm::vec2  WinCenter{ 512.f, 288.f };

static constexpr int   k_NumRows = 7;
static constexpr float k_TileW = 64.f;
static constexpr float k_TileH = 48.f;
static constexpr float k_PlayerW = 50.f;
static constexpr float k_PlayerH = 48.f;
static constexpr float k_CoilyW = 48.f;
static constexpr float k_CoilyH = 50.f;

enum SceneIndex : int
{
	SCENE_MENU = 0,

	SCENE_SOLO_INSTR = 1,
	SCENE_SOLO_LV1_INTRO = 2, SCENE_SOLO_LV1_PLAY = 3,
	SCENE_SOLO_LV2_INTRO = 4, SCENE_SOLO_LV2_PLAY = 5,
	SCENE_SOLO_LV3_INTRO = 6, SCENE_SOLO_LV3_PLAY = 7,
	SCENE_SOLO_VICTORY = 8,

	SCENE_COOP_INSTR = 9,
	SCENE_COOP_LV1_INTRO = 10, SCENE_COOP_LV1_PLAY = 11,
	SCENE_COOP_LV2_INTRO = 12, SCENE_COOP_LV2_PLAY = 13,
	SCENE_COOP_LV3_INTRO = 14, SCENE_COOP_LV3_PLAY = 15,
	SCENE_COOP_VICTORY = 16,

	SCENE_VERSUS_INSTR = 17,
	SCENE_VERSUS_LV1_INTRO = 18, SCENE_VERSUS_LV1_PLAY = 19,
	SCENE_VERSUS_LV2_INTRO = 20, SCENE_VERSUS_LV2_PLAY = 21,
	SCENE_VERSUS_LV3_INTRO = 22, SCENE_VERSUS_LV3_PLAY = 23,
	SCENE_VERSUS_VICTORY = 24,
};

using Btn = dae::Controller::Button;

class SkipRoundCommand final : public dae::Command
{
public:
	explicit SkipRoundCommand(dae::LevelObserverComponent* obs) : m_obs(obs) {}
	void Execute() override { m_obs->ForceComplete(); }
private:
	dae::LevelObserverComponent* m_obs;
};


// Bind jump commands for one player to both a keyboard layout and a controller.
static void BindPlayerJumps(dae::InputManager& input,
	dae::GameObject* go,
	SDL_Scancode kUpLeft, SDL_Scancode kUpRight,
	SDL_Scancode kDownLeft, SDL_Scancode kDownRight,
	unsigned int ctrlIdx)
{
	using KS = dae::KeyState;
	input.BindKeyboardCommand(kUpLeft, KS::Down, std::make_unique<dae::JumpUpLeftCommand>(go));
	input.BindKeyboardCommand(kUpRight, KS::Down, std::make_unique<dae::JumpUpRightCommand>(go));
	input.BindKeyboardCommand(kDownLeft, KS::Down, std::make_unique<dae::JumpDownLeftCommand>(go));
	input.BindKeyboardCommand(kDownRight, KS::Down, std::make_unique<dae::JumpDownRightCommand>(go));

	input.BindControllerCommand(ctrlIdx, Btn::DPadLeft, KS::Down, std::make_unique<dae::JumpUpLeftCommand>(go));
	input.BindControllerCommand(ctrlIdx, Btn::DPadUp, KS::Down, std::make_unique<dae::JumpUpRightCommand>(go));
	input.BindControllerCommand(ctrlIdx, Btn::DPadDown, KS::Down, std::make_unique<dae::JumpDownLeftCommand>(go));
	input.BindControllerCommand(ctrlIdx, Btn::DPadRight, KS::Down, std::make_unique<dae::JumpDownRightCommand>(go));
}

static void BindCommonPlay(dae::InputManager& input)
{
	input.BindKeyboardCommand(SDL_SCANCODE_F2, dae::KeyState::Down,
		std::make_unique<dae::MuteToggleCommand>());
}

static dae::GridComponent* MakeGrid(dae::Scene& scene,
	int numCubeTypes, int requiredState,
	dae::GridComponent::ColorMode colorMode)
{
	const float gridH = (k_NumRows - 1) * k_TileH;
	auto go = std::make_unique<dae::GameObject>();
	auto* grid = go->AddComponent<dae::GridComponent>(
		glm::vec2{ WinCenter.x, WinCenter.y - gridH * 0.5f },
		k_TileW, k_TileH,
		k_NumRows, 0, numCubeTypes, requiredState, colorMode
	);
	scene.Add(std::move(go));
	return grid;
}

static dae::PlayerComponent* MakePlayer(dae::Scene& scene,
	dae::GridComponent* grid,
	int startRow, int startCol,
	const std::string& spritePath)
{
	auto go = std::make_unique<dae::GameObject>();
	auto* atlas = go->AddComponent<dae::TextureAtlasComponent>(spritePath, 4, 1);
	auto p = grid->TileWorldPos(startRow, startCol);
	atlas->SetDestRect(p.x - k_PlayerW * 0.5f, p.y - k_PlayerH, k_PlayerW, k_PlayerH);
	atlas->SetFrame(2, 0);
	auto* player = go->AddComponent<dae::PlayerComponent>(
		grid, &scene, k_PlayerW, k_PlayerH, startRow, startCol, 4
	);
	scene.Add(std::move(go));
	return player;
}

static dae::CoilyComponent* MakeCoily(dae::Scene& scene,
	dae::GridComponent* grid,
	dae::PlayerComponent* targetPlayer)
{
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::TextureAtlasComponent>("qbert/Coily_Spritesheet.png", 10, 1);
	auto* coily = go->AddComponent<dae::CoilyComponent>(grid, targetPlayer, k_CoilyW, k_CoilyH);
	scene.Add(std::move(go));
	return coily;
}

static void MakeHUD(dae::Scene& scene, dae::PlayerComponent* player,
	float heartX, float scoreX)
{
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::HUDComponent>(player, &scene,
		heartX, 200.f, 44.f, scoreX, 140.f);
	scene.Add(std::move(go));
}

static void MakeOverlaysAndPause(dae::Scene& scene,
	dae::PlayerComponent* player, int menuScene)
{
	auto gameOverGO = std::make_unique<dae::GameObject>();
	gameOverGO->AddComponent<dae::GameOverComponent>(player, menuScene, WinDim.x, WinDim.y);
	scene.Add(std::move(gameOverGO));

	auto pauseGO = std::make_unique<dae::GameObject>();
	pauseGO->AddComponent<dae::PauseOverlayComponent>("qbert/Pause_Screen.png", WinDim.x, WinDim.y);
	pauseGO->AddComponent<dae::PauseManagerComponent>(menuScene);
	scene.Add(std::move(pauseGO));
}


static void MakeMainMenu()
{
	dae::GameSession::Get().Reset();
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto logoGO = std::make_unique<dae::GameObject>();
	logoGO->SetLocalPosition(WinCenter.x, 160.f);
	logoGO->AddComponent<dae::TextureComponent>(true)->SetTexture("qbert/Game_Title.png");
	scene.Add(std::move(logoGO));

	auto font = dae::ResourceManager::GetInstance().LoadFont("qbert/Minecraft.ttf", 40);
	SDL_Color yellow{ 255, 215, 0, 255 };

	const float itemX = WinCenter.x, item0Y = 370.f, spacing = 60.f;

	auto makeLabel = [&](const std::string& text, float y)
		{
			auto go = std::make_unique<dae::GameObject>();
			go->SetLocalPosition(itemX, y);
			go->AddComponent<dae::TextObject>(text, font, true, yellow);
			scene.Add(std::move(go));
		};
	makeLabel("Solo Mode", item0Y);
	makeLabel("Co-op Mode", item0Y + spacing);
	makeLabel("Versus Mode", item0Y + spacing * 2.f);

	auto arrowGO = std::make_unique<dae::GameObject>();
	auto* arrowRaw = arrowGO.get();
	arrowGO->AddComponent<dae::TextureComponent>(true)->SetTexture("qbert/Selection_Arrow.png");
	arrowGO->GetComponent<dae::TextureComponent>()->SetScale(3.f);
	scene.Add(std::move(arrowGO));

	auto menuGO = std::make_unique<dae::GameObject>();
	auto* menu = menuGO->AddComponent<dae::MenuComponent>(
		SCENE_SOLO_INSTR, SCENE_COOP_INSTR, SCENE_VERSUS_INSTR);
	menu->SetItemPositions({
		{ itemX, item0Y },
		{ itemX, item0Y + spacing },
		{ itemX, item0Y + spacing * 2.f }
		});
	menu->SetArrow(arrowRaw);
	auto* menuRaw = menu;
	scene.Add(std::move(menuGO));

	using KS = dae::KeyState;
	auto& input = dae::InputManager::GetInstance();

	// Keyboard
	input.BindKeyboardCommand(SDL_SCANCODE_UP, KS::Down, std::make_unique<dae::MenuUpCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_DOWN, KS::Down, std::make_unique<dae::MenuDownCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_W, KS::Down, std::make_unique<dae::MenuUpCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_S, KS::Down, std::make_unique<dae::MenuDownCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_RETURN, KS::Down, std::make_unique<dae::MenuConfirmCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_SPACE, KS::Down, std::make_unique<dae::MenuConfirmCommand>(menuRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_F2, KS::Down, std::make_unique<dae::MuteToggleCommand>());
	input.BindKeyboardCommand(SDL_SCANCODE_ESCAPE, KS::Down, std::make_unique<dae::QuitGameCommand>());

	// Controller 0
	input.BindControllerCommand(0, Btn::DPadUp, KS::Down, std::make_unique<dae::MenuUpCommand>(menuRaw));
	input.BindControllerCommand(0, Btn::DPadDown, KS::Down, std::make_unique<dae::MenuDownCommand>(menuRaw));
	input.BindControllerCommand(0, Btn::ButtonA, KS::Down, std::make_unique<dae::MenuConfirmCommand>(menuRaw));
	input.BindControllerCommand(0, Btn::ButtonB, KS::Down, std::make_unique<dae::QuitGameCommand>());
}

static void MakeInstructions(const std::string& title, int nextScene)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto font = dae::ResourceManager::GetInstance().LoadFont("qbert/Minecraft.ttf", 22);
	auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("qbert/Minecraft.ttf", 16);
	SDL_Color white{ 255, 255, 255, 255 };

	auto makeText = [&](const std::string& t, float x, float y, bool small = false)
		{
			auto go = std::make_unique<dae::GameObject>();
			go->SetLocalPosition(x, y);
			go->AddComponent<dae::TextObject>(t, small ? fontSmall : font, true, white);
			scene.Add(std::move(go));
		};

	makeText(title, WinCenter.x, 55.f);
	makeText("Turn all cubes of the pyramid into the same color!", WinCenter.x, 130.f);
	makeText("Step on them to make them change, but be careful", WinCenter.x, 158.f);
	makeText("to not fall or get hit by enemies while doing so.", WinCenter.x, 186.f);

	auto ctrlGO = std::make_unique<dae::GameObject>();
	ctrlGO->SetLocalPosition(260.f, 310.f);
	ctrlGO->AddComponent<dae::TextureComponent>(true)->SetTexture("qbert/P1_Controls.png");
	scene.Add(std::move(ctrlGO));

	makeText("OR", WinCenter.x, 310.f);
	makeText("Move up/right -> DPAD UP", 700.f, 265.f, true);
	makeText("Move up/left -> DPAD LEFT", 700.f, 290.f, true);
	makeText("Move down/left -> DPAD DOWN", 700.f, 315.f, true);
	makeText("Move down/right -> DPAD RIGHT", 700.f, 340.f, true);
	makeText("Pause game -> START", 700.f, 365.f, true);
	makeText("And pause with the P key", 260.f, 410.f, true);
	makeText("Press ENTER / A button to begin", WinCenter.x, 510.f, true);

	using KS = dae::KeyState;
	auto& input = dae::InputManager::GetInstance();

	input.BindKeyboardCommand(SDL_SCANCODE_RETURN, KS::Down, std::make_unique<dae::LoadSceneCommand>(nextScene));
	input.BindKeyboardCommand(SDL_SCANCODE_SPACE, KS::Down, std::make_unique<dae::LoadSceneCommand>(nextScene));
	input.BindKeyboardCommand(SDL_SCANCODE_F2, KS::Down, std::make_unique<dae::MuteToggleCommand>());
	input.BindKeyboardCommand(SDL_SCANCODE_ESCAPE, KS::Down, std::make_unique<dae::LoadSceneCommand>(SCENE_MENU));

	input.BindControllerCommand(0, Btn::ButtonA, KS::Down, std::make_unique<dae::LoadSceneCommand>(nextScene));
	input.BindControllerCommand(0, Btn::ButtonB, KS::Down, std::make_unique<dae::LoadSceneCommand>(SCENE_MENU));
}

static void MakeLevelIntro(const std::string& imagePath, int nextScene)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto imgGO = std::make_unique<dae::GameObject>();
	imgGO->SetLocalPosition(WinCenter.x, WinCenter.y);
	imgGO->AddComponent<dae::TextureComponent>(true)->SetTexture(imagePath);
	scene.Add(std::move(imgGO));
	auto timerGO = std::make_unique<dae::GameObject>();
	timerGO->AddComponent<dae::TimedSceneTransitionComponent>(nextScene, 2.f);
	scene.Add(std::move(timerGO));
}

static void MakeVictory(int menuScene)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto imgGO = std::make_unique<dae::GameObject>();
	imgGO->SetLocalPosition(WinCenter.x, WinCenter.y - 60.f);
	imgGO->AddComponent<dae::TextureComponent>(true)->SetTexture("qbert/Victory_Title.png");
	scene.Add(std::move(imgGO));

	auto font = dae::ResourceManager::GetInstance().LoadFont("qbert/Minecraft.ttf", 22);
	SDL_Color yellow{ 255, 215, 0, 255 };
	auto go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(WinCenter.x, WinCenter.y + 100.f);
	go->AddComponent<dae::TextObject>("Press ENTER to return to the main menu", font, true, yellow);
	scene.Add(std::move(go));

	using KS = dae::KeyState;
	auto& input = dae::InputManager::GetInstance();

	input.BindKeyboardCommand(SDL_SCANCODE_RETURN, KS::Down, std::make_unique<dae::LoadSceneCommand>(menuScene));
	input.BindKeyboardCommand(SDL_SCANCODE_SPACE, KS::Down, std::make_unique<dae::LoadSceneCommand>(menuScene));
	input.BindKeyboardCommand(SDL_SCANCODE_F2, KS::Down, std::make_unique<dae::MuteToggleCommand>());
	input.BindKeyboardCommand(SDL_SCANCODE_ESCAPE, KS::Down, std::make_unique<dae::QuitGameCommand>());

	input.BindControllerCommand(0, Btn::ButtonA, KS::Down, std::make_unique<dae::LoadSceneCommand>(menuScene));
	input.BindControllerCommand(0, Btn::ButtonB, KS::Down, std::make_unique<dae::LoadSceneCommand>(menuScene));
}


static void MakeSoloPlay(int nextScene,
	int numCubeTypes, int requiredState,
	dae::GridComponent::ColorMode colorMode)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto* grid = MakeGrid(scene, numCubeTypes, requiredState, colorMode);
	auto* player = MakePlayer(scene, grid, 0, 0, "qbert/Qbert_P1_Spritesheet.png");
	auto* coily = MakeCoily(scene, grid, player);

	auto obsGO = std::make_unique<dae::GameObject>();
	auto* obs = obsGO->AddComponent<dae::LevelObserverComponent>(player, grid, coily, nextScene, 2.f);
	scene.Add(std::move(obsGO));

	MakeHUD(scene, player, 40.f, 10.f);
	MakeOverlaysAndPause(scene, player, SCENE_MENU);

	auto& input = dae::InputManager::GetInstance();
	auto* g = player->GetGO();

	// Keyboard: arrow keys + WASD both work
	BindPlayerJumps(input, g,
		SDL_SCANCODE_LEFT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT, 0);
	BindPlayerJumps(input, g,
		SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, 0);

	input.BindKeyboardCommand(SDL_SCANCODE_F1, dae::KeyState::Down, std::make_unique<SkipRoundCommand>(obs));
	BindCommonPlay(input);
}


static void MakeCoopPlay(int nextScene,
	int numCubeTypes, int requiredState,
	dae::GridComponent::ColorMode colorMode)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto* grid = MakeGrid(scene, numCubeTypes, requiredState, colorMode);

	// P1: WASD + controller 0, bottom-left
	auto* p1 = MakePlayer(scene, grid, k_NumRows - 1, 0, "qbert/Qbert_P1_Spritesheet.png");
	// P2: arrow keys + controller 1, bottom-right
	auto* p2 = MakePlayer(scene, grid, k_NumRows - 1, k_NumRows - 1, "qbert/Qbert_P1_Spritesheet.png");

	auto* coily = MakeCoily(scene, grid, p1);

	auto obsGO = std::make_unique<dae::GameObject>();
	auto* obs = obsGO->AddComponent<dae::LevelObserverComponent>(p1, grid, coily, nextScene, 2.f);
	p2->AddObserver(obs);
	scene.Add(std::move(obsGO));

	MakeHUD(scene, p1, 40.f, 10.f);
	MakeHUD(scene, p2, WinDim.x - 60.f, WinDim.x - 140.f);
	MakeOverlaysAndPause(scene, p1, SCENE_MENU);

	auto& input = dae::InputManager::GetInstance();

	// P1: WASD + controller 0 dpad
	BindPlayerJumps(input, p1->GetGO(),
		SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, 0);

	// P2: arrow keys + controller 1 dpad
	BindPlayerJumps(input, p2->GetGO(),
		SDL_SCANCODE_LEFT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT, 1);

	input.BindKeyboardCommand(SDL_SCANCODE_F1, dae::KeyState::Down, std::make_unique<SkipRoundCommand>(obs));
	BindCommonPlay(input);
}


static void MakeVersusPlay(int nextScene,
	int numCubeTypes, int requiredState,
	dae::GridComponent::ColorMode colorMode)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto* grid = MakeGrid(scene, numCubeTypes, requiredState, colorMode);

	// P1: WASD + controller 0
	auto* p1 = MakePlayer(scene, grid, 0, 0, "qbert/Qbert_P1_Spritesheet.png");

	auto* coily = MakeCoily(scene, grid, p1);
	coily->SetPlayerControlled();

	auto obsGO = std::make_unique<dae::GameObject>();
	auto* obs = obsGO->AddComponent<dae::LevelObserverComponent>(p1, grid, coily, nextScene, 2.f);
	scene.Add(std::move(obsGO));

	MakeHUD(scene, p1, 40.f, 10.f);
	MakeOverlaysAndPause(scene, p1, SCENE_MENU);

	auto& input = dae::InputManager::GetInstance();

	// P1: WASD + controller 0
	BindPlayerJumps(input, p1->GetGO(),
		SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, 0);

	// P2: arrow keys + controller 1 → control Coily
	using KS = dae::KeyState;
	input.BindKeyboardCommand(SDL_SCANCODE_LEFT, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, -1, -1));
	input.BindKeyboardCommand(SDL_SCANCODE_UP, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, -1, 0));
	input.BindKeyboardCommand(SDL_SCANCODE_DOWN, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, +1, 0));
	input.BindKeyboardCommand(SDL_SCANCODE_RIGHT, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, +1, +1));

	input.BindControllerCommand(1, Btn::DPadLeft, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, -1, -1));
	input.BindControllerCommand(1, Btn::DPadUp, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, -1, 0));
	input.BindControllerCommand(1, Btn::DPadDown, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, +1, 0));
	input.BindControllerCommand(1, Btn::DPadRight, KS::Down, std::make_unique<dae::CoilyJumpCommand>(coily, +1, +1));

	input.BindKeyboardCommand(SDL_SCANCODE_F1, KS::Down, std::make_unique<SkipRoundCommand>(obs));
	BindCommonPlay(input);
}

int main(int, char* [])
{
#ifdef USE_STEAMWORKS
	if (SteamAPI_Init()) dae::SteamAchievements::GetInstance().Init();
#endif

#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if (!fs::exists(data_location)) data_location = "../Data/";
#endif

	try
	{
#if _DEBUG
		dae::ServiceLocator::RegisterSoundSystem(
			std::make_unique<dae::LoggingSoundSystem>(std::make_unique<dae::SDLSoundSystem>()));
#else
		dae::ServiceLocator::RegisterSoundSystem(std::make_unique<dae::SDLSoundSystem>());
#endif

		auto& ss = dae::ServiceLocator::GetSoundSystem();
		ss.RegisterSound(SND_JUMP, (data_location / "qbert/Sounds/QBert_Jump.wav").string());
		ss.RegisterSound(SND_SWEAR, (data_location / "qbert/Sounds/Swearing.wav").string());
		ss.RegisterSound(SND_ROUND_COMPLETE, (data_location / "qbert/Sounds/Round_Complete_Tune.wav").string());
		ss.RegisterSound(SND_CHANGE_SELECTION, (data_location / "qbert/Sounds/Change_Selection.wav").string());

		using CM = dae::GridComponent::ColorMode;
		auto& sm = dae::SceneManager::GetInstance();

		sm.RegisterScene(SCENE_MENU, MakeMainMenu);

		// Solo
		sm.RegisterScene(SCENE_SOLO_INSTR, [] { MakeInstructions("INSTRUCTIONS - SOLO MODE", SCENE_SOLO_LV1_INTRO); });
		sm.RegisterScene(SCENE_SOLO_LV1_INTRO, [] { MakeLevelIntro("qbert/Level_01_Title.png", SCENE_SOLO_LV1_PLAY); });
		sm.RegisterScene(SCENE_SOLO_LV1_PLAY, [] { MakeSoloPlay(SCENE_SOLO_LV2_INTRO, 6, 1, CM::Normal); });
		sm.RegisterScene(SCENE_SOLO_LV2_INTRO, [] { MakeLevelIntro("qbert/Level_02_Title.png", SCENE_SOLO_LV2_PLAY); });
		sm.RegisterScene(SCENE_SOLO_LV2_PLAY, [] { MakeSoloPlay(SCENE_SOLO_LV3_INTRO, 6, 2, CM::Normal); });
		sm.RegisterScene(SCENE_SOLO_LV3_INTRO, [] { MakeLevelIntro("qbert/Level_03_Title.png", SCENE_SOLO_LV3_PLAY); });
		sm.RegisterScene(SCENE_SOLO_LV3_PLAY, [] { MakeSoloPlay(SCENE_SOLO_VICTORY, 6, 1, CM::AlternatingAfterFirst); });
		sm.RegisterScene(SCENE_SOLO_VICTORY, [] { MakeVictory(SCENE_MENU); });

		// Co-op
		sm.RegisterScene(SCENE_COOP_INSTR, [] { MakeInstructions("INSTRUCTIONS - CO-OP MODE", SCENE_COOP_LV1_INTRO); });
		sm.RegisterScene(SCENE_COOP_LV1_INTRO, [] { MakeLevelIntro("qbert/Level_01_Title.png", SCENE_COOP_LV1_PLAY); });
		sm.RegisterScene(SCENE_COOP_LV1_PLAY, [] { MakeCoopPlay(SCENE_COOP_LV2_INTRO, 6, 1, CM::Normal); });
		sm.RegisterScene(SCENE_COOP_LV2_INTRO, [] { MakeLevelIntro("qbert/Level_02_Title.png", SCENE_COOP_LV2_PLAY); });
		sm.RegisterScene(SCENE_COOP_LV2_PLAY, [] { MakeCoopPlay(SCENE_COOP_LV3_INTRO, 6, 2, CM::Normal); });
		sm.RegisterScene(SCENE_COOP_LV3_INTRO, [] { MakeLevelIntro("qbert/Level_03_Title.png", SCENE_COOP_LV3_PLAY); });
		sm.RegisterScene(SCENE_COOP_LV3_PLAY, [] { MakeCoopPlay(SCENE_COOP_VICTORY, 6, 1, CM::AlternatingAfterFirst); });
		sm.RegisterScene(SCENE_COOP_VICTORY, [] { MakeVictory(SCENE_MENU); });

		// Versus
		sm.RegisterScene(SCENE_VERSUS_INSTR, [] { MakeInstructions("INSTRUCTIONS - VERSUS MODE", SCENE_VERSUS_LV1_INTRO); });
		sm.RegisterScene(SCENE_VERSUS_LV1_INTRO, [] { MakeLevelIntro("qbert/Level_01_Title.png", SCENE_VERSUS_LV1_PLAY); });
		sm.RegisterScene(SCENE_VERSUS_LV1_PLAY, [] { MakeVersusPlay(SCENE_VERSUS_LV2_INTRO, 6, 1, CM::Normal); });
		sm.RegisterScene(SCENE_VERSUS_LV2_INTRO, [] { MakeLevelIntro("qbert/Level_02_Title.png", SCENE_VERSUS_LV2_PLAY); });
		sm.RegisterScene(SCENE_VERSUS_LV2_PLAY, [] { MakeVersusPlay(SCENE_VERSUS_LV3_INTRO, 6, 2, CM::Normal); });
		sm.RegisterScene(SCENE_VERSUS_LV3_INTRO, [] { MakeLevelIntro("qbert/Level_03_Title.png", SCENE_VERSUS_LV3_PLAY); });
		sm.RegisterScene(SCENE_VERSUS_LV3_PLAY, [] { MakeVersusPlay(SCENE_VERSUS_VICTORY, 6, 1, CM::AlternatingAfterFirst); });
		sm.RegisterScene(SCENE_VERSUS_VICTORY, [] { MakeVictory(SCENE_MENU); });

		dae::Minigin engine(data_location, WinDim);
		engine.Run(MakeMainMenu);
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