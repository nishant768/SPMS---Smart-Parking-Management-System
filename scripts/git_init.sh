#!/bin/bash
# scripts/git_init.sh – Initialize Git repo and make per-stage commits
# Run this ONCE on Ubuntu after cloning/setting up the project

set -e
cd "$(dirname "$0")/.."

echo "=============================================="
echo "  Initializing Git repository for SPMS"
echo "=============================================="

git init
git config user.name  "Your Name"
git config user.email "your.email@example.com"

# ── Stage 1 ────────────────────────────────────────────────────────────────
git add README.md docs/stage1_introduction.md
git commit -m "feat(stage1): Add project introduction and overview

- Project title, objective, and problem statement
- Define project scope (in/out)
- List expected outcomes
- Identify technology stack"

git tag v0.1.0

# ── Stage 2 ────────────────────────────────────────────────────────────────
git add docs/stage2_requirements.md
git commit -m "docs(stage2): Add project requirements and development plan

- Functional requirements (FR-01 to FR-09)
- Non-functional requirements (NFR-01 to NFR-07)
- System constraints
- Module breakdown
- Development timeline (10 weeks)
- Risk analysis"

git tag v0.2.0

# ── Stage 3 ────────────────────────────────────────────────────────────────
git add docs/stage3_design.md
git commit -m "docs(stage3): Add system design, UML diagrams, architecture

- System architecture diagram (kernel + application layers)
- Component descriptions
- Class diagram (UML)
- Sequence diagrams (entry + exit flows)
- State machine diagrams (slot + ticket)
- Data structures
- Git branching strategy"

git tag v0.3.0

# ── Stage 4 ────────────────────────────────────────────────────────────────
git add .gitignore CMakeLists.txt include/ driver/ src/ scripts/setup.sh scripts/load_driver.sh
git commit -m "feat(stage4): Initial implementation and working prototype

- Linux kernel character device driver (parking_sensor.ko)
  - Dynamic major number, cdev, ioctl, mutex
  - /dev/parking_sensor auto-created via device class
- C++17 application modules:
  - common.h (types, constants, helpers)
  - Logger (thread-safe, color-coded, file+stdout)
  - Vehicle, ParkingSlot, ParkingLot (core logic)
  - TicketManager (RAII, map-based storage)
  - BillingSystem (ceil-rounded hourly rates)
  - SensorInterface (device file + FIFO named pipe)
  - SignalHandler (sigaction SIGINT/SIGTERM/SIGHUP)
  - main.cpp (interactive CLI menu)
- CMakeLists.txt build system
- scripts/setup.sh, load_driver.sh"

git tag v0.4.0

# ── Stage 5 ────────────────────────────────────────────────────────────────
git add tests/ scripts/run_tests.sh docs/stage5_testing.md
git commit -m "test(stage5): Add unit tests, integration tests, bug fixes

- test_parking_lot.cpp: 21 test cases (entry, exit, overflow, duplicates)
- test_billing.cpp: 14 test cases (rates, ceil, min charge, 24h)
- test_sensor.cpp: 12 test cases (device, pipe, callback)
- All 47 tests pass
- Fixed race condition in displaySlotMap()
- Fixed double-close ticket bug
- Fixed pipe blocking issue (added O_NONBLOCK)
- Added const correctness and [[nodiscard]]
- Zero compiler warnings (-Wall -Wextra -Wpedantic)"

git tag v0.5.0

# ── Stage 6 ────────────────────────────────────────────────────────────────
git add docs/stage4_implementation.md docs/stage6_final_report.md
git commit -m "docs(stage6): Final implementation, report, and presentation

- Complete stage 4 and stage 6 documentation
- Final feature list (all features implemented)
- Build & run instructions
- Test results summary
- Known limitations and future improvements
- Git tagging strategy documented"

git tag v1.0.0

echo ""
echo "=============================================="
echo "  Git repository initialized!"
echo "  Commits: $(git log --oneline | wc -l)"
echo "  Tags:    $(git tag | wc -l)"
echo ""
echo "  To push to GitHub:"
echo "  git remote add origin https://github.com/YOUR_USERNAME/SmartParkingSystem.git"
echo "  git push -u origin main --tags"
echo "=============================================="
