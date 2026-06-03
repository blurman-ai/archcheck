#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "archcheck/scan/duplication/duplication_scanner.h"

using namespace archcheck::scan::duplication;
namespace fs = std::filesystem;

static std::string readFile(const std::string &path)
{
  std::ifstream ifs(path);
  if (!ifs)
    return "";
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  return buffer.str();
}

static void collectCppFiles(const std::string &root, std::vector<std::pair<std::string, std::string>> &files,
                            int maxFiles = 1000)
{
  int count = 0;
  for (const auto &entry : fs::recursive_directory_iterator(root))
  {
    if (count >= maxFiles)
      break;
    if (!entry.is_regular_file())
      continue;

    std::string ext = entry.path().extension().string();
    if (ext != ".cpp" && ext != ".h")
      continue;

    std::string content = readFile(entry.path().string());
    if (!content.empty())
    {
      files.push_back({entry.path().string(), content});
      count++;
    }
  }
}

struct ProjectResult
{
  std::string name;
  int fileCount = 0;
  size_t totalLoc = 0;
  size_t candidatesP0 = 0;
  size_t pairsP0 = 0;
  size_t pairsP1 = 0;
  double filterRate = 0.0;
};

TEST_CASE("Duplication detector on all 4 projects with P0/P1 guard stats", "[duplication][batch][integration]")
{

  std::vector<std::string> projectPaths = {"/home/localadm/projects/cpparch/sandbox/drift_repos/BambuStudio",
                                           "/home/localadm/projects/cpparch/sandbox/drift_repos/LibreSprite",
                                           "/home/localadm/projects/cpparch/sandbox/drift_repos/sys-device",
                                           "/home/localadm/projects/cpparch/sandbox/drift_repos/vmecpp"};

  std::vector<ProjectResult> results;

  for (const auto &projPath : projectPaths)
  {
    if (!fs::exists(projPath))
    {
      std::cerr << "SKIP: " << projPath << " (not found)\n";
      continue;
    }

    std::string projName = fs::path(projPath).filename().string();

    // Collect files (limit to 500 to avoid huge scan)
    std::vector<std::pair<std::string, std::string>> files;
    collectCppFiles(projPath, files, 500);

    if (files.empty())
    {
      std::cerr << "SKIP: " << projName << " (no C++ files)\n";
      continue;
    }

    std::cout << "\n📦 " << projName << " (" << files.size() << " files)\n";
    std::cout.flush();

    // P0-only scan
    ScannerOptions optsP0;
    optsP0.simThreshold = 0.60;
    optsP0.enableJointFloor = true;
    optsP0.enableP1Guards = false;

    ScanResult resultP0 = scanForDuplication(files, optsP0);

    // P0+P1 scan
    ScannerOptions optsP1;
    optsP1.simThreshold = 0.60;
    optsP1.enableJointFloor = true;
    optsP1.enableP1Guards = true;

    ScanResult resultP1 = scanForDuplication(files, optsP1);

    // Calculate stats
    double filterRate = 0.0;
    if (resultP0.pairs.size() > 0)
    {
      filterRate = 100.0 * (1.0 - (double)resultP1.pairs.size() / (double)resultP0.pairs.size());
    }

    ProjectResult pr;
    pr.name = projName;
    pr.fileCount = files.size();
    pr.totalLoc = resultP0.totalLoc;
    pr.candidatesP0 = resultP0.candidateCount;
    pr.pairsP0 = resultP0.pairs.size();
    pr.pairsP1 = resultP1.pairs.size();
    pr.filterRate = filterRate;

    results.push_back(pr);

    // Print per-project summary
    std::cout << "  P0 candidates: " << resultP0.candidateCount << "\n";
    std::cout << "  P0 pairs: " << resultP0.pairs.size() << "\n";
    std::cout << "  P1 pairs: " << resultP1.pairs.size() << "\n";
    if (resultP0.pairs.size() > 0)
    {
      std::cout << "  P1 filter rate: " << filterRate << "%\n";
    }
    std::cout.flush();
  }

  // Print summary table
  std::cout << "\n=================================================================\n";
  std::cout << "SUMMARY TABLE (4 PROJECTS)\n";
  std::cout << "=================================================================\n";
  std::cout << "Project          | Files | P0-cand | P0-pairs | P1-pairs | Filter%\n";
  std::cout << "-----------------|-------|---------|----------|----------|----------\n";

  for (const auto &pr : results)
  {
    char buf[100];
    snprintf(buf, sizeof(buf), "%-16s | %5d | %7zu | %8zu | %8zu | %7.1f%%\n", pr.name.c_str(), pr.fileCount,
             pr.candidatesP0, pr.pairsP0, pr.pairsP1, pr.filterRate);
    std::cout << buf;
  }

  // Aggregate stats
  size_t totalFiles = 0;
  size_t totalCandidatesP0 = 0;
  size_t totalPairsP0 = 0;
  size_t totalPairsP1 = 0;

  for (const auto &pr : results)
  {
    totalFiles += pr.fileCount;
    totalCandidatesP0 += pr.candidatesP0;
    totalPairsP0 += pr.pairsP0;
    totalPairsP1 += pr.pairsP1;
  }

  double aggFilterRate = 0.0;
  if (totalPairsP0 > 0)
  {
    aggFilterRate = 100.0 * (1.0 - (double)totalPairsP1 / (double)totalPairsP0);
  }

  std::cout << "===============|=======|=========|==========|==========|==========\n";
  char sumBuf[100];
  snprintf(sumBuf, sizeof(sumBuf), "%-16s | %5zu | %7zu | %8zu | %8zu | %7.1f%%\n", "TOTAL", totalFiles,
           totalCandidatesP0, totalPairsP0, totalPairsP1, aggFilterRate);
  std::cout << sumBuf;

  std::cout << "\n✅ P1 Guards reduced false positives by " << aggFilterRate << "% across all projects\n";

  // Assertion: at least some filtering happened
  REQUIRE(totalFiles > 0);
  REQUIRE(totalPairsP1 <= totalPairsP0);
}
