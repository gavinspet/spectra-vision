# Quick Command Reference

## Windows (PowerShell/CMD)

```bash
# Build
.\build.bat

# Run
.\run.bat

# Clean
.\clean.bat

# Build + Run (daily workflow)
.\dev.bat

# Full clean rebuild
.\clean.bat && .\build.bat
```

## Linux / macOS

```bash
# Build
./build.sh

# Run
./run.sh

# Clean
./clean.sh

# Build + Run (daily workflow)
./dev.sh

# Full clean rebuild
./clean.sh && ./build.sh

# Make scripts executable (first time)
chmod +x *.sh
```

## What Each Script Does

| Command | Purpose | Output |
|---------|---------|--------|
| `build.bat/sh` | Configure CMake + compile | `build/debug/bin/SpectraVision` |
| `run.bat/sh` | Start backend server | Server running on `localhost:8080` |
| `clean.bat/sh` | Remove all build artifacts | `build/` directory deleted |
| `dev.bat/sh` | Build + Run workflow | Build output + running server |

## Typical Day

```bash
# Morning - Start fresh
./clean.sh
./build.sh
./run.sh

# During day - Code → Build → Test
./dev.sh              # One command build + run

# Before commit - Final check
./clean.sh
./dev.sh              # Full rebuild from scratch
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Build fails | Check error output, run `./clean.sh && ./build.sh` |
| "Executable not found" | Run `./build.sh` first |
| Port 8080 in use | Kill existing process: `lsof -i :8080 && kill -9 <PID>` |
| Permission denied (Unix) | `chmod +x *.sh` |
| CMake not found | Install CMake: `brew install cmake` (macOS) or `sudo apt-get install cmake` (Linux) |

## Environment

- **Project Root:** `spectra-vision/`
- **Build Dir:** `build/debug/`
- **Binary:** `build/debug/bin/SpectraVision` (Unix) or `SpectraVision.exe` (Windows)
- **Port:** `localhost:8080`
- **Config:** `config.json` (auto-created with defaults)

See [SCRIPTS_GUIDE.md](SCRIPTS_GUIDE.md) for detailed documentation.
