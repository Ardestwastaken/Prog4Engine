//#include "CacheTheTrash.h"
//#include "imgui.h"
//#include <chrono>
//#include <numeric>
//#include <algorithm>
//
//namespace dae
//{
//    ThrashComponent::ThrashComponent(GameObject* pGameObject)
//        : Component(pGameObject)
//    {
//    }
//
//    template <typename T, typename Operation>
//    void ThrashComponent::MeasureCacheThrashing(std::vector<float>& outPlotData, int samples, int bufferSize, Operation op) const
//    {
//        outPlotData.clear();
//        std::vector<T> buffer(bufferSize);
//
//        for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
//        {
//            std::vector<long long> timings;
//
//            for (int s = 0; s < samples + 2; ++s) // +2 for outlier removal
//            {
//                auto start = std::chrono::high_resolution_clock::now();
//
//                for (size_t i = 0; i < buffer.size(); i += stepsize)
//                {
//                    op(buffer[i]);
//                }
//
//                auto end = std::chrono::high_resolution_clock::now();
//                timings.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
//            }
//
//
//            // Remove outliers and average
//            std::sort(timings.begin(), timings.end());
//            float avg = static_cast<float>(std::accumulate(timings.begin() + 1, timings.end() - 1, 0LL)) / (timings.size() - 2);
//            outPlotData.push_back(avg);
//        }
//    }
//
//    void ThrashComponent::RenderGui() const
//    {
//        // --- Exercise 1 Window ---
//        ImGui::Begin("exercise one");
//        ImGui::SetNextItemWidth(150);
//        ImGui::InputInt("nr samples", &m_Ex1Samples);
//        if (m_Ex1Samples < 1) m_Ex1Samples = 1;
//
//        if (ImGui::Button("Thrash the cache"))
//        {
//            MeasureCacheThrashing<int>(m_Ex1PlotData, m_Ex1Samples, 1 << 25, [](int& v) { v *= 2; });
//        }
//
//        if (!m_Ex1PlotData.empty())
//        {
//            float maxVal = *std::max_element(m_Ex1PlotData.begin(), m_Ex1PlotData.end());
//            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
//            ImGui::PlotLines("##Ex1Plot", m_Ex1PlotData.data(), (int)m_Ex1PlotData.size(), 0, nullptr, 0.0f, maxVal * 1.1f, ImVec2(200, 100));
//            ImGui::PopStyleColor();
//        }
//        ImGui::End();
//
//        // --- Exercise 2 Window ---
//        ImGui::Begin("exercise two");
//        ImGui::SetNextItemWidth(150);
//        ImGui::InputInt("nr samples", &m_Ex2Samples);
//        if (m_Ex2Samples < 1) m_Ex2Samples = 1;
//
//        if (ImGui::Button("Thrash the cache"))
//        {
//            const int size = 1 << 22; // Reduced size slightly for custom objects
//            MeasureCacheThrashing<GameObject3D>(m_Ex2PlotData, m_Ex2Samples, size, [](GameObject3D& v) { v.ID *= 2; });
//            MeasureCacheThrashing<GameObject3DAlt>(m_Ex2PlotDataAlt, m_Ex2Samples, size, [](GameObject3DAlt& v) { v.ID *= 2; });
//        }
//
//        if (!m_Ex2PlotData.empty())
//        {
//            ImGui::Text("GameObject3D (Inline)");
//            float maxVal = *std::max_element(m_Ex2PlotData.begin(), m_Ex2PlotData.end());
//            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
//            ImGui::PlotLines("##Ex2Plot", m_Ex2PlotData.data(), (int)m_Ex2PlotData.size(), 0, nullptr, 0.0f, maxVal * 1.1f, ImVec2(200, 100));
//            ImGui::PopStyleColor();
//
//            ImGui::Text("GameObject3DAlt (Pointer)");
//            float maxValAlt = *std::max_element(m_Ex2PlotDataAlt.begin(), m_Ex2PlotDataAlt.end());
//            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
//            ImGui::PlotLines("##Ex2PlotAlt", m_Ex2PlotDataAlt.data(), (int)m_Ex2PlotDataAlt.size(), 0, nullptr, 0.0f, maxValAlt * 1.1f, ImVec2(200, 100));
//            ImGui::PopStyleColor();
//        }
//        ImGui::End();
//    }
//}