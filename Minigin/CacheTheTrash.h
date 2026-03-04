#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <algorithm>
#include "imgui.h"
#include "Component.h"
#include "GameObject.h"

namespace dae {
	struct Transform {
		float matrix[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
	};

	class GameObject3D {
	public:
		Transform transform;
		int ID{};
	};

	class GameObject3DAlt {
	public:
		Transform* transform;
		int ID;
	};

	static std::vector<float> g_PlotData;
	static int g_NrSamples = 10;

	static void RenderExerciseOneWindow()
	{
		ImGui::Begin("exercise one");

		ImGui::SetNextItemWidth(150);
		ImGui::InputInt("nr samples", &g_NrSamples);

		if (g_NrSamples < 1) g_NrSamples = 1;

		if (ImGui::Button("Thrash the cache"))
		{
			g_PlotData.clear();

			const int size = 1 << 25;
			int* arr = new int[size];

			for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
			{
				std::vector<long long> timings;

				for (int s = 0; s < g_NrSamples; ++s)
				{
					auto start = std::chrono::high_resolution_clock::now();

					for (int i = 0; i < size; i += stepsize)
					{
						arr[i] *= 2;
					}

					auto end = std::chrono::high_resolution_clock::now();
					auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
					timings.push_back(total);
				}

				float average = (float)std::accumulate(timings.begin(), timings.end(), 0LL) / timings.size();
				g_PlotData.push_back(average);
			}

			delete[] arr;
		}

		if (!g_PlotData.empty())
		{
			float maxVal = *std::max_element(g_PlotData.begin(), g_PlotData.end());

			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));

			ImGui::PlotLines("##CachePlot", g_PlotData.data(), (int)g_PlotData.size(),
				0, nullptr, 0.0f, maxVal * 1.1f, ImVec2(200, 100));

			ImGui::PopStyleColor();
		}

		ImGui::End();
	}

	static std::vector<float> g_PlotDataEx2;
	static std::vector<float> g_PlotDataEx2Alt;
	static int g_NrSamplesEx2 = 10;

	static void RenderExerciseTwoWindow()
	{
		ImGui::Begin("exercise two");

		ImGui::SetNextItemWidth(150);
		ImGui::InputInt("nr samples", &g_NrSamplesEx2);
		if (g_NrSamplesEx2 < 1) g_NrSamplesEx2 = 1;

		if (ImGui::Button("Thrash the cache"))
		{
			g_PlotDataEx2.clear();
			g_PlotDataEx2Alt.clear();

			const int size = 1 << 25;

			GameObject3D* buffer = new GameObject3D[size];
			GameObject3DAlt* bufferAlt = new GameObject3DAlt[size];

			auto RunExperiment = [&](auto* buf, std::vector<float>& dataOut) {
				for (int stepsize = 1; stepsize <= 1024; stepsize *= 2) {
					std::vector<long long> timings;
					for (int s = 0; s < g_NrSamplesEx2 + 2; ++s) {
						auto start = std::chrono::high_resolution_clock::now();
						for (int i = 0; i < size; i += stepsize) {
							buf[i].ID *= 2;
						}
						auto end = std::chrono::high_resolution_clock::now();
						timings.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
					}
					std::sort(timings.begin(), timings.end());
					float average = (float)std::accumulate(timings.begin() + 1, timings.end() - 1, 0LL) / (timings.size() - 2);
					dataOut.push_back(average);
				}
				};

			RunExperiment(buffer, g_PlotDataEx2);
			RunExperiment(bufferAlt, g_PlotDataEx2Alt);

			delete[] buffer;
			delete[] bufferAlt;
		}

		if (!g_PlotDataEx2.empty())
		{
			ImGui::Text("GameObject3D (Inline Transform)");
			float maxVal = *std::max_element(g_PlotDataEx2.begin(), g_PlotDataEx2.end());
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PlotLines("##CachePlot2", g_PlotDataEx2.data(), (int)g_PlotDataEx2.size(), 0, nullptr, 0.0f, maxVal * 1.1f, ImVec2(200, 100));
			ImGui::PopStyleColor();

			ImGui::Text("GameObject3DAlt (Pointer Transform)");
			float maxValAlt = *std::max_element(g_PlotDataEx2Alt.begin(), g_PlotDataEx2Alt.end());
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::PlotLines("##CachePlot2Alt", g_PlotDataEx2Alt.data(), (int)g_PlotDataEx2Alt.size(), 0, nullptr, 0.0f, maxValAlt * 1.1f, ImVec2(200, 100));
			ImGui::PopStyleColor();

		}

		ImGui::End();
	}

}

//class ThrashComponent final : public Component
//{
//public:
//	explicit ThrashComponent(GameObject* pGameObject);
//	virtual ~ThrashComponent() override = default;
//
//	virtual void RenderGui() const override;
//
//private:
//	mutable std::vector<float> m_Ex1PlotData;
//	mutable int m_Ex1Samples = 10;
//
//	mutable std::vector<float> m_Ex2PlotData;
//	mutable std::vector<float> m_Ex2PlotDataAlt;
//	mutable int m_Ex2Samples = 10;
//
//	template <typename T, typename Operation>
//	void MeasureCacheThrashing(std::vector<float>& outPlotData, int samples, int bufferSize, Operation op) const;
//};
//
//}


