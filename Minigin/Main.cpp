#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
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

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	// Background
	auto background = std::make_unique<dae::GameObject>();
	background->AddComponent<dae::TextureComponent>()->SetTexture("background.png");
	scene.Add(std::move(background));

	// Logo
	auto logo = std::make_unique<dae::GameObject>();
	logo->SetLocalPosition(358.f, 180.f);
	logo->AddComponent<dae::TextureComponent>()->SetTexture("logo.png");
	scene.Add(std::move(logo));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);

	// Title
	auto titleObj = std::make_unique<dae::GameObject>();
	titleObj->SetLocalPosition(292.f, 20.f);
	titleObj->AddComponent<dae::TextObject>("Programming 4 Assignment", font)
		->SetColor({ 255, 255, 0, 255 });
	scene.Add(std::move(titleObj));

	auto fpsObj = std::make_unique<dae::GameObject>();
	fpsObj->SetLocalPosition(5.f, 5.f);
	fpsObj->AddComponent<dae::TextObject>("0.0 FPS", font);
	fpsObj->AddComponent<dae::FpsComponent>();
	scene.Add(std::move(fpsObj));

	// Character 1 with keyboard
	auto char1 = std::make_unique<dae::GameObject>();
	char1->SetLocalPosition(200.f, 300.f);
	auto* tex1 = char1->AddComponent<dae::TextureComponent>();
	tex1->SetTexture("qbrt.png");
	tex1->SetScale(0.05f);
	dae::GameObject* pChar1 = char1.get();
	scene.Add(std::move(char1));

	// Character 2 with controller
	auto char2 = std::make_unique<dae::GameObject>();
	char2->SetLocalPosition(400.f, 300.f);
	auto* tex2 = char2->AddComponent<dae::TextureComponent>();
	tex2->SetTexture("qbrt.png");
	tex2->SetScale(0.05f);
	dae::GameObject* pChar2 = char2.get();
	scene.Add(std::move(char2));

	constexpr float speed1 = 100.f;
	constexpr float speed2 = speed1 * 2.f;

	auto& input = dae::InputManager::GetInstance();

	// WASD KEYBOARD COMMANDS
	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0, -1, 0 }, speed1));
	input.BindKeyboardCommand(SDL_SCANCODE_S, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0,  1, 0 }, speed1));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ -1,  0, 0 }, speed1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 1,  0, 0 }, speed1));

	//DPAD CONTROLLER COMMANDS
	input.BindControllerCommand(0, dae::Controller::Button::DPadUp, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0, -1, 0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadDown, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0,  1, 0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ -1,  0, 0 }, speed2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 1,  0, 0 }, speed2));
}

int main(int, char* []) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if (!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
	return 0;
}