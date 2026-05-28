// Copyright 2024 Example Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: long_copyright_guard.h
// -----------------------------------------------------------------------------
//
// This fixture verifies that SF.8 handles headers where the include guard
// appears after a long Apache 2.0 copyright block (abseil-cpp style).
// The guard appears on non-empty line 32 — exceeds old kScanLines=30 limit.
//
// Without the fix (kScanLines=60) archcheck reported a false SF.8 violation
// for every abseil header using this pattern (85 headers affected).
//
// See backlog task #034.
//
// Additional description line to push guard past non-empty line 30.
// Additional description line to push guard past non-empty line 31.

#ifndef ARCHCHECK_FIXTURES_SF8_LONG_COPYRIGHT_GUARD_H_
#define ARCHCHECK_FIXTURES_SF8_LONG_COPYRIGHT_GUARD_H_

#endif // ARCHCHECK_FIXTURES_SF8_LONG_COPYRIGHT_GUARD_H_
