#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "archcheck/scan/duplication/duplication_scanner.h"

using namespace archcheck::scan::duplication;

// Helper to read file
static std::string readFile(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs)
        return "";
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

TEST_CASE("Duplication detector on vmecpp generated code (fftx)", "[duplication][integration]") {
    // Two almost-identical generated files from vmecpp
    std::string f1Path = "/home/localadm/projects/cpparch/sandbox/drift_repos/vmecpp/"
        "src/vmecpp/cpp/third_party/fftx_codelets/lib_fftx_prdftbat_cpu_srcs/"
        "fftx_prdftbat_32_bat_144_APar_APar_CPU.cpp";
    
    std::string f2Path = "/home/localadm/projects/cpparch/sandbox/drift_repos/vmecpp/"
        "src/vmecpp/cpp/third_party/fftx_codelets/lib_fftx_prdftbat_cpu_srcs/"
        "fftx_prdftbat_32_bat_168_APar_APar_CPU.cpp";
    
    std::string f1Content = readFile(f1Path);
    std::string f2Content = readFile(f2Path);
    
    REQUIRE(!f1Content.empty());
    REQUIRE(!f2Content.empty());
    
    // Run detector with P0 guards only (production configuration)
    ScannerOptions opts;
    opts.simThreshold = 0.60;       // Standard threshold
    opts.enableJointFloor = true;   // P0.6: enforce both metrics
    opts.enableP1Guards = false;    // Disable P1 for baseline
    
    std::vector<std::pair<std::string, std::string>> files = {
        {f1Path, f1Content},
        {f2Path, f2Content}
    };
    
    ScanResult resultP0 = scanForDuplication(files, opts);
    
    SECTION("P0-only baseline") {
        // With P0 guards, expect some pair or filtered pair
        // P0.6 (joint floor) should filter unless BOTH weighted AND line pass
        // These files have very high similarity, should pass P0 guards but may be filtered by P0.6
        std::cout << "\n=== P0-ONLY BASELINE ===\n";
        std::cout << "Fragments found: " << resultP0.fragments.size() << "\n";
        std::cout << "Candidates before filters: " << resultP0.candidateCount << "\n";
        std::cout << "Pairs after P0 guards: " << resultP0.pairs.size() << "\n";
        
        if (!resultP0.pairs.empty()) {
            for (const auto &p : resultP0.pairs) {
                std::cout << "  Pair [" << p.a << ", " << p.b << "]:\n";
                std::cout << "    weighted=" << p.weighted << ", line=" << p.line << "\n";
            }
        }
    }
    
    // Run detector with P0 + P1 guards
    opts.enableP1Guards = true;
    ScanResult resultP1 = scanForDuplication(files, opts);
    
    SECTION("P0+P1 guards") {
        std::cout << "\n=== P0+P1 GUARDS ===\n";
        std::cout << "Pairs after P0+P1 guards: " << resultP1.pairs.size() << "\n";
        
        if (!resultP1.pairs.empty()) {
            for (const auto &p : resultP1.pairs) {
                std::cout << "  Pair [" << p.a << ", " << p.b << "]:\n";
                std::cout << "    weighted=" << p.weighted << ", line=" << p.line << "\n";
            }
        }
    }
    
    // Both should detect the high similarity
    REQUIRE(resultP0.fragments.size() > 0);
    REQUIRE(resultP1.fragments.size() > 0);
}
