#pragma once

#include <filesystem>

namespace archcheck::cli
{

// #199: opt-in flags for --duplication. windowFree selects the window-free core (ADR-004);
// minLines is a mode-agnostic minimum-clone-size output filter in lines (#200, 0 = off).
struct DuplicationOptions
{
  bool windowFree = false;
  int minLines = 0;
  int rareAnchors = 0;     // window-free rarity idiom guard (advanced sweep knob; 0 = off)
  bool controlFlow = true; // window-free structural idiom guard (drop non-executable spans)
};

// Preview/advisory single-path modes. All print to stdout; none gate CI
// except runGraph (exit 1 when the include graph has cycles).
int runScan(const std::filesystem::path &root);
int runGraph(const std::filesystem::path &root);
int runDuplication(const std::filesystem::path &root, const DuplicationOptions &opts = {});
int runHistory(const std::filesystem::path &root);

} // namespace archcheck::cli
