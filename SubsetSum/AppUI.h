//#pragma once
//
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//#include <GLFW/glfw3.h>
//
//#include "Dataset.h"
//#include "DataLoader.h"
//#include "Solution.h"
//#include "SubsetSumSolver.h"
//#include "BruteForceSolver.h"
//#include "BacktrackingSolver.h"
//#include "DynamicProgrammingSolver.h"
//#include "MeetInTheMiddleSolver.h"
//#include "PerformanceAnalyzer.h"
//
//#include <memory>
//#include <vector>
//#include <string>
//#include <sstream>
//#include <algorithm>
//#include <cstring>
//#include <cmath>
//
//// ============================================================
//// Class: AppUI
//// Giao diện đồ họa ImGui với 3 tab:
////   Tab 1 - Chạy một thuật toán tự chọn
////   Tab 2 - Chạy và so sánh tất cả thuật toán
////   Tab 3 - Xem data input
//// ============================================================
//class AppUI {
//private:
//    GLFWwindow* window_ = nullptr;
//    int winW_ = 960, winH_ = 640;
//
//    // ── Solvers & Analyzer ───────────────────────────────
//    std::vector<std::shared_ptr<SubsetSumSolver>> solvers_;
//    PerformanceAnalyzer analyzer_;
//
//    // ── Shared input state ────────────────────────────────
//    char bufElements_[512] = "3 1 4 1 5 9 2 6";
//    char bufTarget_[64] = "10";
//
//    // ── Tab 1 state ───────────────────────────────────────
//    int  selectedSolver_ = 3; // MeetInTheMiddle by default
//    Solution singleResult_;
//    bool singleRan_ = false;
//
//    // ── Tab 2 state ───────────────────────────────────────
//    std::vector<Solution> compareResults_;
//    bool compareRan_ = false;
//
//    // ── Tab 3 state ───────────────────────────────────────
//    // (computed on-the-fly from shared input)
//
//    // ── Colors ────────────────────────────────────────────
//    const ImVec4 COL_GREEN{ 0.11f, 0.62f, 0.46f, 1.f };
//    const ImVec4 COL_AMBER{ 0.73f, 0.46f, 0.09f, 1.f };
//    const ImVec4 COL_RED{ 0.64f, 0.18f, 0.18f, 1.f };
//    const ImVec4 COL_MUTED{ 0.55f, 0.55f, 0.55f, 1.f };
//    const ImVec4 COL_BG2{ 0.13f, 0.14f, 0.15f, 1.f };
//    const ImVec4 COL_ACCENT{ 0.11f, 0.62f, 0.46f, 0.18f };
//
//    // ── Helpers ───────────────────────────────────────────
//
//    // Parse bufElements_ → vector<long long>
//    std::vector<long long> parseElements() const {
//        std::vector<long long> v;
//        std::istringstream ss(bufElements_);
//        long long x;
//        while (ss >> x) v.push_back(x);
//        return v;
//    }
//
//    long long parseTarget() const {
//        try { return std::stoll(bufTarget_); }
//        catch (...) { return 0; }
//    }
//
//    Dataset makeDataset() const {
//        auto elems = parseElements();
//        if (elems.empty()) elems = { 0 };
//        return Dataset(elems, parseTarget(), "manual");
//    }
//
//    // Thanh màu nhỏ dùng trong bảng so sánh
//    void drawBar(double value, double maxVal, float width, ImVec4 color) {
//        float ratio = (maxVal > 0) ? (float)(value / maxVal) : 0.f;
//        ratio = std::max(0.f, std::min(1.f, ratio));
//        ImVec2 pos = ImGui::GetCursorScreenPos();
//        float h = ImGui::GetTextLineHeight() * 0.7f;
//        ImDrawList* dl = ImGui::GetWindowDrawList();
//        // background
//        dl->AddRectFilled(pos, { pos.x + width, pos.y + h },
//            IM_COL32(60, 60, 60, 180), 3.f);
//        // fill
//        dl->AddRectFilled(pos, { pos.x + width * ratio, pos.y + h },
//            ImGui::ColorConvertFloat4ToU32(color), 3.f);
//        ImGui::Dummy({ width, h });
//    }
//
//    // Badge text "FOUND" / "NOT FOUND"
//    void drawBadge(bool found) {
//        if (found) {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            ImGui::Text("FOUND");
//        }
//        else {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_AMBER);
//            ImGui::Text("NOT FOUND");
//        }
//        ImGui::PopStyleColor();
//    }
//
//    // Stat card: label trên, value lớn dưới
//    void drawStatCard(const char* label, const std::string& value,
//        float width = 120.f) {
//        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.16f, 0.17f, 1.f));
//        ImGui::BeginChild(label, { width, 56.f }, true);
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::SetWindowFontScale(0.82f);
//        ImGui::Text("%s", label);
//        ImGui::SetWindowFontScale(1.f);
//        ImGui::PopStyleColor();
//        ImGui::Text("%s", value.c_str());
//        ImGui::EndChild();
//        ImGui::PopStyleColor();
//    }
//
//    // ── Shared input panel (dùng ở cả 3 tab) ─────────────
//    void drawInputPanel() {
//        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.13f, 0.14f, 1.f));
//        ImGui::BeginChild("##input_panel", { 0, 80.f }, true);
//
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Dataset");
//        ImGui::PopStyleColor();
//        ImGui::Separator();
//        ImGui::Spacing();
//
//        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 160.f);
//        ImGui::InputText("Elements", bufElements_, sizeof(bufElements_));
//        ImGui::SameLine();
//        ImGui::SetNextItemWidth(100.f);
//        ImGui::InputText("Target", bufTarget_, sizeof(bufTarget_));
//
//        ImGui::EndChild();
//        ImGui::PopStyleColor();
//        ImGui::Spacing();
//    }
//
//    // ── PRESETS nhanh ─────────────────────────────────────
//    void drawPresets() {
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Quick presets:");
//        ImGui::PopStyleColor();
//        ImGui::SameLine();
//
//        struct Preset { const char* label; const char* elems; const char* target; };
//        static const Preset presets[] = {
//            {"small·found",    "3 1 4 1 5 9 2 6",                     "10"},
//            {"medium·n=16",    "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16", "57"},
//            {"negatives",      "-5 -3 2 7 -1 4 6",                    "3"},
//            {"no solution",    "1 2 4 8 16",                           "100"},
//        };
//        for (auto& p : presets) {
//            if (ImGui::SmallButton(p.label)) {
//                strncpy(bufElements_, p.elems, sizeof(bufElements_) - 1);
//                strncpy(bufTarget_, p.target, sizeof(bufTarget_) - 1);
//                singleRan_ = false;
//                compareRan_ = false;
//            }
//            ImGui::SameLine();
//        }
//        ImGui::NewLine();
//        ImGui::Spacing();
//    }
//
//    // ============================================================
//    // TAB 1: Chạy một thuật toán
//    // ============================================================
//    void drawTab1() {
//        drawInputPanel();
//        drawPresets();
//
//        // Solver selector cards (2 cột)
//        static const char* names[] = {
//            "BruteForce", "Backtracking", "DynamicProgramming", "MeetInTheMiddle"
//        };
//        static const char* complexities[] = {
//            "O(2^n * n)  |  bitmask duyệt hết",
//            "O(2^n) + pruning suffix-sum",
//            "O(n * |target|)  |  hash map",
//            "O(2^(n/2) * n)  |  chia đôi + hash"
//        };
//
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Chon thuat toan:");
//        ImGui::PopStyleColor();
//        ImGui::Spacing();
//
//        float cardW = (ImGui::GetContentRegionAvail().x - 12.f) / 2.f;
//        for (int i = 0; i < 4; i++) {
//            bool sel = (selectedSolver_ == i);
//
//            if (sel) ImGui::PushStyleColor(ImGuiCol_ChildBg,
//                ImVec4(0.07f, 0.30f, 0.22f, 0.35f));
//            else     ImGui::PushStyleColor(ImGuiCol_ChildBg,
//                ImVec4(0.12f, 0.13f, 0.14f, 1.f));
//
//            ImGui::BeginChild(names[i], { cardW, 58.f }, true);
//
//            // Radio dot
//            ImGui::PushStyleColor(ImGuiCol_Text, sel ? COL_GREEN : COL_MUTED);
//            ImGui::Text(sel ? "[*]" : "[ ]");
//            ImGui::PopStyleColor();
//            ImGui::SameLine();
//
//            ImGui::BeginGroup();
//            if (sel) ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            ImGui::Text("%s", names[i]);
//            if (sel) ImGui::PopStyleColor();
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//            ImGui::SetWindowFontScale(0.82f);
//            ImGui::TextWrapped("%s", complexities[i]);
//            ImGui::SetWindowFontScale(1.f);
//            ImGui::PopStyleColor();
//            ImGui::EndGroup();
//
//            ImGui::EndChild();
//            ImGui::PopStyleColor();
//
//            // Click vào card để chọn
//            if (ImGui::IsItemClicked()) {
//                selectedSolver_ = i;
//                singleRan_ = false;
//            }
//
//            if (i % 2 == 0) ImGui::SameLine(0, 12.f);
//            else ImGui::Spacing();
//        }
//
//        ImGui::Spacing();
//
//        // Nút RUN
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.11f, 0.45f, 0.34f, 1.f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.13f, 0.55f, 0.40f, 1.f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.09f, 0.36f, 0.27f, 1.f));
//        if (ImGui::Button("  Run  ", { 120.f, 32.f })) {
//            Dataset ds = makeDataset();
//            singleResult_ = solvers_[selectedSolver_]->solve(ds);
//            singleRan_ = true;
//        }
//        ImGui::PopStyleColor(3);
//
//        if (!singleRan_) return;
//
//        // ── Kết quả ──
//        ImGui::Spacing();
//        ImGui::Separator();
//        ImGui::Spacing();
//
//        const Solution& sol = singleResult_;
//
//        // Header
//        ImGui::Text("Solver: ");
//        ImGui::SameLine();
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//        ImGui::Text("%s", sol.solverName().c_str());
//        ImGui::PopStyleColor();
//        ImGui::SameLine(0, 20.f);
//        ImGui::Text("Time: ");
//        ImGui::SameLine();
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//        ImGui::Text("%.4f ms", sol.elapsedMs());
//        ImGui::PopStyleColor();
//
//        ImGui::Spacing();
//
//        if (!sol.found()) {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_AMBER);
//            ImGui::Text("Khong co tap con nao co tong = %lld", sol.target());
//            ImGui::PopStyleColor();
//            return;
//        }
//
//        // Stat cards
//        std::string sTarget = std::to_string(sol.target());
//        std::string sSum = std::to_string(sol.actualSum());
//        std::string sN = std::to_string((int)sol.elements().size());
//        drawStatCard("Target", sTarget);  ImGui::SameLine(0, 8);
//        drawStatCard("Sum", sSum);     ImGui::SameLine(0, 8);
//        drawStatCard("Subset n", sN);
//
//        ImGui::Spacing();
//
//        // Tập con - hiển thị từng pill
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Tap con:");
//        ImGui::PopStyleColor();
//        ImGui::SameLine();
//        for (auto x : sol.elements()) {
//            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07f, 0.30f, 0.22f, 0.55f));
//            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.30f, 0.22f, 0.55f));
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            std::string lbl = std::to_string(x);
//            ImGui::SmallButton(lbl.c_str());
//            ImGui::PopStyleColor(3);
//            ImGui::SameLine();
//        }
//        ImGui::NewLine();
//
//        // Indices
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Indices: ");
//        ImGui::PopStyleColor();
//        ImGui::SameLine();
//        for (auto idx : sol.indices()) {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//            ImGui::Text("[%d]", idx);
//            ImGui::PopStyleColor();
//            ImGui::SameLine();
//        }
//        ImGui::NewLine();
//
//        // Validate
//        ImGui::Spacing();
//        if (sol.validate()) {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            ImGui::Text("Ket qua hop le: %lld = %lld", sol.actualSum(), sol.target());
//            ImGui::PopStyleColor();
//        }
//        else {
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_RED);
//            ImGui::Text("[!] Ket qua SAI!");
//            ImGui::PopStyleColor();
//        }
//    }
//
//    // ============================================================
//    // TAB 2: So sánh tất cả
//    // ============================================================
//    void drawTab2() {
//        drawInputPanel();
//        drawPresets();
//
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.11f, 0.45f, 0.34f, 1.f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.13f, 0.55f, 0.40f, 1.f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.09f, 0.36f, 0.27f, 1.f));
//        if (ImGui::Button("  Run All  ", { 140.f, 32.f })) {
//            Dataset ds = makeDataset();
//            analyzer_.clearRecords();
//            compareResults_.clear();
//            for (auto& s : solvers_)
//                compareResults_.push_back(s->solve(ds));
//            compareRan_ = true;
//        }
//        ImGui::PopStyleColor(3);
//
//        if (!compareRan_ || compareResults_.empty()) return;
//
//        ImGui::Spacing();
//        ImGui::Separator();
//        ImGui::Spacing();
//
//        // Tìm fastest & maxTime để vẽ bar
//        double maxMs = 0;
//        int fastestIdx = 0;
//        for (int i = 0; i < (int)compareResults_.size(); i++) {
//            double t = compareResults_[i].elapsedMs();
//            if (t > maxMs) maxMs = t;
//            if (t < compareResults_[fastestIdx].elapsedMs()) fastestIdx = i;
//        }
//
//        // Bảng so sánh
//        if (ImGui::BeginTable("##compare_table", 5,
//            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
//            ImGuiTableFlags_SizingFixedFit))
//        {
//            ImGui::TableSetupColumn("Solver", ImGuiTableColumnFlags_WidthStretch);
//            ImGui::TableSetupColumn("Ket qua", ImGuiTableColumnFlags_WidthFixed, 90.f);
//            ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthFixed, 90.f);
//            ImGui::TableSetupColumn("Toc do", ImGuiTableColumnFlags_WidthFixed, 120.f);
//            ImGui::TableSetupColumn("Valid", ImGuiTableColumnFlags_WidthFixed, 50.f);
//            ImGui::TableHeadersRow();
//
//            for (int i = 0; i < (int)compareResults_.size(); i++) {
//                const Solution& sol = compareResults_[i];
//                bool fastest = (i == fastestIdx);
//
//                ImGui::TableNextRow();
//
//                // Col 0: Solver name
//                ImGui::TableSetColumnIndex(0);
//                if (fastest) {
//                    ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//                    ImGui::Text("%s  *fastest*", sol.solverName().c_str());
//                    ImGui::PopStyleColor();
//                }
//                else {
//                    ImGui::Text("%s", sol.solverName().c_str());
//                }
//
//                // Col 1: Badge
//                ImGui::TableSetColumnIndex(1);
//                drawBadge(sol.found());
//
//                // Col 2: Time
//                ImGui::TableSetColumnIndex(2);
//                ImGui::Text("%.4f", sol.elapsedMs());
//
//                // Col 3: Bar
//                ImGui::TableSetColumnIndex(3);
//                ImVec4 barColor = fastest ? COL_GREEN : ImVec4(0.37f, 0.37f, 0.37f, 1.f);
//                drawBar(sol.elapsedMs(), maxMs, 110.f, barColor);
//
//                // Col 4: Valid
//                ImGui::TableSetColumnIndex(4);
//                if (sol.validate()) {
//                    ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//                    ImGui::Text("OK");
//                }
//                else {
//                    ImGui::PushStyleColor(ImGuiCol_Text, COL_RED);
//                    ImGui::Text("FAIL");
//                }
//                ImGui::PopStyleColor();
//            }
//            ImGui::EndTable();
//        }
//
//        ImGui::Spacing();
//
//        // Tập con từ solver đầu tiên tìm được
//        for (const auto& sol : compareResults_) {
//            if (!sol.found()) continue;
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//            ImGui::Text("Tap con tim duoc (solver dau tien):");
//            ImGui::PopStyleColor();
//            ImGui::SameLine();
//            for (auto x : sol.elements()) {
//                std::string lbl = std::to_string(x);
//                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07f, 0.30f, 0.22f, 0.45f));
//                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.30f, 0.22f, 0.45f));
//                ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//                ImGui::SmallButton(lbl.c_str());
//                ImGui::PopStyleColor(3);
//                ImGui::SameLine();
//            }
//            ImGui::NewLine();
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//            ImGui::Text("Tong = %lld  ==  target %lld",
//                sol.actualSum(), sol.target());
//            ImGui::PopStyleColor();
//            break;
//        }
//    }
//
//    // ============================================================
//    // TAB 3: Xem Data Input
//    // ============================================================
//    void drawTab3() {
//        drawInputPanel();
//        drawPresets();
//
//        auto elems = parseElements();
//        long long t = parseTarget();
//
//        if (elems.empty()) {
//            ImGui::TextColored(COL_AMBER, "Khong co phan tu nao.");
//            return;
//        }
//
//        int n = (int)elems.size();
//        long long total = 0, mn = elems[0], mx = elems[0];
//        int pos = 0, neg = 0, zer = 0;
//        for (auto x : elems) {
//            total += x;
//            mn = std::min(mn, x);
//            mx = std::max(mx, x);
//            if (x > 0) pos++; else if (x < 0) neg++; else zer++;
//        }
//        double avg = (double)total / n;
//
//        // ── Stat cards ──
//        float cw = 100.f;
//        drawStatCard("n", std::to_string(n), cw); ImGui::SameLine(0, 6);
//        drawStatCard("target", std::to_string(t), cw); ImGui::SameLine(0, 6);
//        drawStatCard("sum", std::to_string(total), cw); ImGui::SameLine(0, 6);
//        char avgBuf[32]; snprintf(avgBuf, sizeof(avgBuf), "%.2f", avg);
//        drawStatCard("avg", avgBuf, cw); ImGui::SameLine(0, 6);
//        drawStatCard("min", std::to_string(mn), cw); ImGui::SameLine(0, 6);
//        drawStatCard("max", std::to_string(mx), cw);
//
//        ImGui::Spacing();
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("positive: %d   negative: %d   zero: %d", pos, neg, zer);
//        ImGui::PopStyleColor();
//
//        ImGui::Spacing();
//        ImGui::Separator();
//        ImGui::Spacing();
//
//        // ── Hiển thị từng phần tử, highlight nếu thuộc subset ──
//        // Tính subset nhanh bằng MITM
//        MeetInTheMiddleSolver mitm;
//        Dataset ds(elems, t);
//        Solution chk = mitm.solve(ds);
//        std::vector<bool> inSubset(n, false);
//        if (chk.found())
//            for (int idx : chk.indices()) inSubset[idx] = true;
//
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Cac phan tu %s:",
//            chk.found() ? "(xanh = thuoc tap con hop le)" : "(khong co tap con)");
//        ImGui::PopStyleColor();
//        ImGui::Spacing();
//
//        for (int i = 0; i < n; i++) {
//            std::string lbl = std::to_string(elems[i]) + "##e" + std::to_string(i);
//            if (inSubset[i]) {
//                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07f, 0.35f, 0.25f, 0.7f));
//                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.35f, 0.25f, 0.7f));
//                ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            }
//            else {
//                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.19f, 0.20f, 1.f));
//                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.23f, 0.24f, 1.f));
//                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.f));
//            }
//            // Tooltip index
//            ImGui::SmallButton(lbl.c_str());
//            if (ImGui::IsItemHovered())
//                ImGui::SetTooltip("index [%d] = %lld", i, elems[i]);
//            ImGui::PopStyleColor(3);
//            ImGui::SameLine();
//        }
//        ImGui::NewLine();
//        ImGui::Spacing();
//        ImGui::Separator();
//        ImGui::Spacing();
//
//        // ── Histogram ──
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::Text("Histogram phan bo gia tri:");
//        ImGui::PopStyleColor();
//        ImGui::Spacing();
//
//        const int bins = 12;
//        long long range = mx - mn;
//        std::vector<int> counts(bins, 0);
//        for (auto x : elems) {
//            int b = (range == 0) ? 0
//                : (int)std::min((long long)(bins - 1), (x - mn) * bins / (range + 1));
//            counts[b]++;
//        }
//        int cmax = *std::max_element(counts.begin(), counts.end());
//
//        float barW = std::max(20.f, (ImGui::GetContentRegionAvail().x - 20.f) / bins - 4.f);
//        float maxBarH = 80.f;
//        ImVec2 origin = ImGui::GetCursorScreenPos();
//        ImDrawList* dl = ImGui::GetWindowDrawList();
//
//        for (int b = 0; b < bins; b++) {
//            float h = (cmax > 0) ? (float)counts[b] / cmax * maxBarH : 0.f;
//            float x0 = origin.x + b * (barW + 4.f);
//            float y0 = origin.y + maxBarH - h;
//            float alpha = 0.35f + 0.65f * (cmax > 0 ? (float)counts[b] / cmax : 0.f);
//
//            dl->AddRectFilled({ x0, y0 }, { x0 + barW, origin.y + maxBarH },
//                IM_COL32(28, 159, 117, (int)(alpha * 255)), 2.f);
//
//            // Label count
//            if (counts[b] > 0) {
//                char buf[8]; snprintf(buf, sizeof(buf), "%d", counts[b]);
//                dl->AddText({ x0 + 2.f, y0 - 14.f },
//                    IM_COL32(120, 200, 160, 220), buf);
//            }
//        }
//        ImGui::Dummy({ 0, maxBarH + 10.f });
//
//        // X-axis labels
//        ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//        ImGui::SetWindowFontScale(0.75f);
//        char lo_buf[16], hi_buf[16];
//        snprintf(lo_buf, sizeof(lo_buf), "%lld", mn);
//        snprintf(hi_buf, sizeof(hi_buf), "%lld", mx);
//        ImGui::Text("%s", lo_buf);
//        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20.f);
//        ImGui::Text("%s", hi_buf);
//        ImGui::SetWindowFontScale(1.f);
//        ImGui::PopStyleColor();
//    }
//
//    // ── GLFW / ImGui setup ────────────────────────────────
//
//    bool initWindow() {
//        if (!glfwInit()) return false;
//
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//
//        window_ = glfwCreateWindow(winW_, winH_,
//            "Subset Sum Solver", nullptr, nullptr);
//        if (!window_) { glfwTerminate(); return false; }
//
//        glfwMakeContextCurrent(window_);
//        glfwSwapInterval(1);
//        return true;
//    }
//
//    void initImGui() {
//        IMGUI_CHECKVERSION();
//        ImGui::CreateContext();
//        ImGuiIO& io = ImGui::GetIO();
//        io.IniFilename = nullptr; // Không lưu layout
//
//        // Dark theme + tùy chỉnh
//        ImGui::StyleColorsDark();
//        ImGuiStyle& s = ImGui::GetStyle();
//        s.WindowRounding = 6.f;
//        s.FrameRounding = 5.f;
//        s.GrabRounding = 4.f;
//        s.ChildRounding = 5.f;
//        s.PopupRounding = 5.f;
//        s.TabRounding = 5.f;
//        s.FramePadding = { 8.f, 5.f };
//        s.ItemSpacing = { 8.f, 6.f };
//        s.WindowPadding = { 14.f, 14.f };
//        s.ScrollbarSize = 10.f;
//
//        // Override một số màu
//        auto& c = s.Colors;
//        c[ImGuiCol_WindowBg] = { 0.10f, 0.11f, 0.12f, 1.f };
//        c[ImGuiCol_FrameBg] = { 0.16f, 0.17f, 0.18f, 1.f };
//        c[ImGuiCol_FrameBgHovered] = { 0.20f, 0.21f, 0.22f, 1.f };
//        c[ImGuiCol_FrameBgActive] = { 0.22f, 0.23f, 0.25f, 1.f };
//        c[ImGuiCol_TitleBgActive] = { 0.08f, 0.25f, 0.18f, 1.f };
//        c[ImGuiCol_Tab] = { 0.13f, 0.14f, 0.15f, 1.f };
//        c[ImGuiCol_TabHovered] = { 0.10f, 0.40f, 0.28f, 0.8f };
//        c[ImGuiCol_TabActive] = { 0.09f, 0.35f, 0.24f, 1.f };
//        c[ImGuiCol_Header] = { 0.09f, 0.35f, 0.24f, 0.6f };
//        c[ImGuiCol_HeaderHovered] = { 0.09f, 0.42f, 0.28f, 0.7f };
//        c[ImGuiCol_Separator] = { 0.25f, 0.27f, 0.28f, 1.f };
//        c[ImGuiCol_TableBorderLight] = { 0.20f, 0.22f, 0.24f, 1.f };
//        c[ImGuiCol_TableBorderStrong] = { 0.28f, 0.30f, 0.32f, 1.f };
//        c[ImGuiCol_TableRowBg] = { 0.11f, 0.12f, 0.13f, 1.f };
//        c[ImGuiCol_TableRowBgAlt] = { 0.14f, 0.15f, 0.16f, 1.f };
//        c[ImGuiCol_CheckMark] = { 0.11f, 0.72f, 0.50f, 1.f };
//        c[ImGuiCol_SliderGrab] = { 0.11f, 0.60f, 0.42f, 1.f };
//        c[ImGuiCol_Button] = { 0.16f, 0.17f, 0.19f, 1.f };
//        c[ImGuiCol_ButtonHovered] = { 0.20f, 0.21f, 0.23f, 1.f };
//        c[ImGuiCol_ButtonActive] = { 0.08f, 0.40f, 0.28f, 1.f };
//        c[ImGuiCol_ScrollbarBg] = { 0.08f, 0.09f, 0.10f, 1.f };
//        c[ImGuiCol_ScrollbarGrab] = { 0.25f, 0.27f, 0.28f, 1.f };
//
//        ImGui_ImplGlfw_InitForOpenGL(window_, true);
//        ImGui_ImplOpenGL3_Init("#version 330");
//    }
//
//    void shutdown() {
//        ImGui_ImplOpenGL3_Shutdown();
//        ImGui_ImplGlfw_Shutdown();
//        ImGui::DestroyContext();
//        glfwDestroyWindow(window_);
//        glfwTerminate();
//    }
//
//public:
//    AppUI() {
//        solvers_.push_back(std::make_shared<BruteForceSolver>());
//        solvers_.push_back(std::make_shared<BacktrackingSolver>());
//        solvers_.push_back(std::make_shared<DynamicProgrammingSolver>());
//        solvers_.push_back(std::make_shared<MeetInTheMiddleSolver>());
//        for (auto& s : solvers_) analyzer_.addSolver(s);
//    }
//
//    int run() {
//        if (!initWindow()) return -1;
//        initImGui();
//
//        while (!glfwWindowShouldClose(window_)) {
//            glfwPollEvents();
//            glfwGetFramebufferSize(window_, &winW_, &winH_);
//
//            ImGui_ImplOpenGL3_NewFrame();
//            ImGui_ImplGlfw_NewFrame();
//            ImGui::NewFrame();
//
//            // Full-screen window
//            ImGui::SetNextWindowPos({ 0, 0 });
//            ImGui::SetNextWindowSize({ (float)winW_, (float)winH_ });
//            ImGui::Begin("##main", nullptr,
//                ImGuiWindowFlags_NoTitleBar |
//                ImGuiWindowFlags_NoResize |
//                ImGuiWindowFlags_NoMove |
//                ImGuiWindowFlags_NoBringToFrontOnFocus);
//
//            // Title bar giả
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN);
//            ImGui::Text("SUBSET SUM SOLVER");
//            ImGui::PopStyleColor();
//            ImGui::SameLine();
//            ImGui::PushStyleColor(ImGuiCol_Text, COL_MUTED);
//            ImGui::Text("| BruteForce  Backtracking  DP  MeetInTheMiddle");
//            ImGui::PopStyleColor();
//            ImGui::Separator();
//            ImGui::Spacing();
//
//            // 3 Tabs
//            if (ImGui::BeginTabBar("##tabs")) {
//                if (ImGui::BeginTabItem("1  Chay mot thuat toan")) {
//                    drawTab1();
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("2  So sanh tat ca")) {
//                    drawTab2();
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("3  Xem data input")) {
//                    drawTab3();
//                    ImGui::EndTabItem();
//                }
//                ImGui::EndTabBar();
//            }
//
//            ImGui::End();
//
//            // Render
//            ImGui::Render();
//            glViewport(0, 0, winW_, winH_);
//            glClearColor(0.10f, 0.11f, 0.12f, 1.f);
//            glClear(GL_COLOR_BUFFER_BIT);
//            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//            glfwSwapBuffers(window_);
//        }
//
//        shutdown();
//        return 0;
//    }
//};