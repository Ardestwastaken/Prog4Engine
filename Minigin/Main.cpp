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
#include "ServiceLocator.h"
#include "SDLSoundSystem.h"
#include "SoundSystem.h"

// Q*bert game code
#include "QbertPlayerComponent.h"
#include "QbertCommands.h"
#include "QbertHUD.h"
#include "QbertGridComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

enum SoundIds : dae::sound_id { SND_MOVE = 0, SND_DIE = 1, SND_POINTS = 2 };

static dae::GameObject* MakeLabel(dae::Scene& scene,
    const std::string& text, std::shared_ptr<dae::Font> font, float x, float y)
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

    auto bg = std::make_unique<dae::GameObject>();
    bg->AddComponent<dae::TextureComponent>(false)->SetTexture("background.png");
    scene.Add(std::move(bg));

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

    auto fpsObj = std::make_unique<dae::GameObject>();
    fpsObj->SetLocalPosition(5.f, 5.f);
    fpsObj->AddComponent<dae::TextObject>("0 FPS", font);
    fpsObj->AddComponent<dae::FpsComponent>();
    scene.Add(std::move(fpsObj));

    // Controls
    auto ctrl = std::make_unique<dae::GameObject>();
    ctrl->SetLocalPosition(10.f, 170.f);
    ctrl->AddComponent<dae::TextObject>(
        "UP=up-left  RIGHT=up-right  LEFT=down-left  DOWN=down-right  K=die", font);
    scene.Add(std::move(ctrl));

    // Grid
    constexpr float tileW = 64.f;
    constexpr float tileH = 40.f;
    constexpr int numRows = 7;
    const glm::vec2 topCenter = { 350.f, 260.f };

    auto gridObj = std::make_unique<dae::GameObject>();
    auto* pGrid = gridObj->AddComponent<dae::QbertGridComponent>(
        topCenter, tileW, tileH, numRows);
    scene.Add(std::move(gridObj));

    //PLAYER
    auto qbertObj = std::make_unique<dae::GameObject>();
    auto* pQbert = qbertObj->AddComponent<dae::QbertPlayerComponent>(pGrid, 0, 0,3);
    {
        auto* tex = qbertObj->AddComponent<dae::TextureComponent>(true);
        tex->SetTexture("qbrt.png");
        tex->SetScale(0.07f);
    }
    dae::GameObject* pQbertGO = qbertObj.get();
    scene.Add(std::move(qbertObj));

    //HUD
    auto* livesLabel = MakeLabel(scene, "Q*bert Lives: 3", font, 10.f, 55.f);
    auto* scoreLabel = MakeLabel(scene, "Q*bert Score: 0", font, 10.f, 80.f);
    pQbert->AddObserver(livesLabel->AddComponent<dae::QbertLivesDisplay>(0));
    pQbert->AddObserver(scoreLabel->AddComponent<dae::QbertScoreDisplay>(0));

   //INPUT
    auto& input = dae::InputManager::GetInstance();

    // Keyboard
    input.BindKeyboardCommand(SDL_SCANCODE_UP, dae::KeyState::Down, std::make_unique<dae::QbertJumpUpLeftCommand>(pQbertGO));
    input.BindKeyboardCommand(SDL_SCANCODE_RIGHT, dae::KeyState::Down, std::make_unique<dae::QbertJumpUpRightCommand>(pQbertGO));
    input.BindKeyboardCommand(SDL_SCANCODE_LEFT, dae::KeyState::Down, std::make_unique<dae::QbertJumpDownLeftCommand>(pQbertGO));
    input.BindKeyboardCommand(SDL_SCANCODE_DOWN, dae::KeyState::Down, std::make_unique<dae::QbertJumpDownRightCommand>(pQbertGO));
    input.BindKeyboardCommand(SDL_SCANCODE_K, dae::KeyState::Down, std::make_unique<dae::QbertDieCommand>(pQbertGO));

    // Controller
    input.BindControllerCommand(0, dae::Controller::Button::DPadUp, dae::KeyState::Down, std::make_unique<dae::QbertJumpUpLeftCommand>(pQbertGO));
    input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::KeyState::Down, std::make_unique<dae::QbertJumpUpRightCommand>(pQbertGO));
    input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::KeyState::Down, std::make_unique<dae::QbertJumpDownLeftCommand>(pQbertGO));
    input.BindControllerCommand(0, dae::Controller::Button::DPadDown, dae::KeyState::Down, std::make_unique<dae::QbertJumpDownRightCommand>(pQbertGO));
    input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::KeyState::Down, std::make_unique<dae::QbertDieCommand>(pQbertGO));

    input.BindKeyboardCommand(SDL_SCANCODE_M, dae::KeyState::Down,
        std::make_unique<dae::PlaySoundCommand>(SND_MOVE, 0.8f));
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
        ss.RegisterSound(SND_MOVE, (data_location / "move.wav").string());
        ss.RegisterSound(SND_DIE, (data_location / "die.wav").string());
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