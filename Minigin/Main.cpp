#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"

//Add components
#include "TextureComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto Background = std::make_unique<dae::GameObject>();
	auto BackTexture = Background->AddComponent<dae::TextureComponent>();
	BackTexture->SetTexture("background.png");
	scene.Add(std::move(Background));

	auto Logo = std::make_unique<dae::GameObject>();
	auto LogoTexture = Logo->AddComponent<dae::TextureComponent>();
	LogoTexture->SetTexture("logo.png");
	LogoTexture->SetPosition(358, 180);
	scene.Add(std::move(Logo));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<dae::GameObject>();
	to->SetPosition(292, 20);

	auto Text = to->AddComponent<dae::TextObject>("Programming 4 Assignment", font);
	Text->SetColor({ 255, 255, 0, 255 });
	scene.Add(std::move(to));


}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
