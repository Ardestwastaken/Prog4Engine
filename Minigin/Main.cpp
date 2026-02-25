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
#include "RotatorComponent.h"

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

	// Title text
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

	// First character
	auto qbert1 = std::make_unique<dae::GameObject>();
	auto* tex1 = qbert1->AddComponent<dae::TextureComponent>();
	tex1->SetTexture("qbrt.png");
	tex1->SetScale(0.05f);
	qbert1->AddComponent<dae::RotatorComponent>(glm::vec2{ 512, 288 }, 100.f, 1.f);

	dae::GameObject* pQbert1 = qbert1.get();
	scene.Add(std::move(qbert1));

	// Second character
	auto qbert2 = std::make_unique<dae::GameObject>();
	auto* tex2 = qbert2->AddComponent<dae::TextureComponent>();
	tex2->SetTexture("qbrt.png");
	tex2->SetScale(0.05f);
	qbert2->AddComponent<dae::RotatorComponent>(glm::vec2{ 0, 0 }, 60.f, -2.f);
	qbert2->SetParent(pQbert1, false);
	scene.Add(std::move(qbert2));
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