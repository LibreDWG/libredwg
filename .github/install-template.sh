#!/bin/sh
set -euo pipefail

# LibreDWG Installation Script
# Platform: PLATFORM_PLACEHOLDER
# Version: VERSION_PLACEHOLDER

VERSION="VERSION_PLACEHOLDER"
PLATFORM="PLATFORM_PLACEHOLDER"
REPO="rite-build/libredwg"
PREFIX="$HOME/.local"

# Parse arguments
while [ $# -gt 0 ]; do
  case "$1" in
    --prefix)
      PREFIX="$2"
      shift 2
      ;;
    --system)
      PREFIX="/usr/local"
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [OPTIONS]"
      echo ""
      echo "Options:"
      echo "  --prefix PATH    Install to custom location (default: ~/.local)"
      echo "  --system         Install to /usr/local (requires sudo)"
      echo "  --help           Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Run '$0 --help' for usage information"
      exit 1
      ;;
  esac
done

echo "==> LibreDWG Installer"
echo "    Version:  $VERSION"
echo "    Platform: $PLATFORM"
echo "    Prefix:   $PREFIX"
echo ""

# Check if we need sudo
NEEDS_SUDO=0
if [ ! -w "$PREFIX" ] && [ -d "$PREFIX" ]; then
  NEEDS_SUDO=1
elif [ ! -d "$PREFIX" ]; then
  PARENT_DIR=$(dirname "$PREFIX")
  if [ ! -w "$PARENT_DIR" ]; then
    NEEDS_SUDO=1
  fi
fi

if [ $NEEDS_SUDO -eq 1 ]; then
  echo "⚠️  Installation requires sudo access for $PREFIX"
  SUDO="sudo"
else
  SUDO=""
fi

# Create temporary directory
TMP_DIR=$(mktemp -d)
trap "rm -rf $TMP_DIR" EXIT

cd "$TMP_DIR"

# Download files
TARBALL="libredwg-${VERSION}-${PLATFORM}.tar.gz"
CHECKSUM="libredwg-${VERSION}-${PLATFORM}.tar.gz.sha256"
BASE_URL="https://github.com/${REPO}/releases/download/${VERSION}"

# Determine which download tool to use
if command -v curl >/dev/null 2>&1; then
  DOWNLOAD_CMD="curl"
elif command -v wget >/dev/null 2>&1; then
  DOWNLOAD_CMD="wget"
else
  echo "❌ Error: curl or wget is required"
  exit 1
fi

# Retry download function with exponential backoff
download_with_retry() {
  local url="$1"
  local output="$2"
  local max_attempts=3
  local attempt=1
  local delay=1

  while [ $attempt -le $max_attempts ]; do
    if [ "$DOWNLOAD_CMD" = "curl" ]; then
      if curl -fsSL "$url" -o "$output"; then
        return 0
      fi
    elif [ "$DOWNLOAD_CMD" = "wget" ]; then
      if wget -q "$url" -O "$output"; then
        return 0
      fi
    fi

    if [ $attempt -lt $max_attempts ]; then
      echo "    Download failed (attempt $attempt/$max_attempts), retrying in ${delay}s..."
      sleep "$delay"
      delay=$((delay * 2))
    fi
    attempt=$((attempt + 1))
  done

  echo "❌ Error: Failed to download after $max_attempts attempts"
  return 1
}

echo "==> Downloading $TARBALL..."
download_with_retry "${BASE_URL}/${TARBALL}" "$TARBALL"
download_with_retry "${BASE_URL}/${CHECKSUM}" "$CHECKSUM"

# Verify checksum
echo "==> Verifying checksum..."
if command -v sha256sum >/dev/null 2>&1; then
  sha256sum -c "$CHECKSUM" || {
    echo "❌ Checksum verification failed!"
    exit 1
  }
elif command -v shasum >/dev/null 2>&1; then
  shasum -a 256 -c "$CHECKSUM" || {
    echo "❌ Checksum verification failed!"
    exit 1
  }
else
  echo "⚠️  Warning: sha256sum not found, skipping verification"
fi

# Extract tarball
echo "==> Extracting files..."
tar -xzf "$TARBALL"

# Install
echo "==> Installing to $PREFIX..."
$SUDO mkdir -p "$PREFIX"
$SUDO cp -r "libredwg-${VERSION}-${PLATFORM}/usr/local/"* "$PREFIX/"

echo ""
echo "✅ LibreDWG $VERSION installed successfully!"
echo ""

# Check if binaries are in PATH
if echo "$PATH" | grep -q "$PREFIX/bin"; then
  echo "LibreDWG binaries are available in your PATH."
else
  echo "⚠️  Add $PREFIX/bin to your PATH:"
  echo ""
  echo "    export PATH=\"$PREFIX/bin:\$PATH\""
  echo ""
  echo "Add this to your ~/.bashrc, ~/.zshrc, or ~/.profile to make it permanent."
fi

# Platform-specific: Add library path instructions for non-standard installations
if [ "$PREFIX" != "/usr/local" ]; then
  if [ "$(uname -s)" = "Darwin" ]; then
    echo ""
    echo "⚠️  On macOS, if you get library loading errors, also export:"
    echo ""
    echo "    export DYLD_LIBRARY_PATH=\"$PREFIX/lib:\$DYLD_LIBRARY_PATH\""
    echo ""
  elif [ "$(uname -s)" = "Linux" ]; then
    echo ""
    echo "⚠️  On Linux, if you get library loading errors, also export:"
    echo ""
    echo "    export LD_LIBRARY_PATH=\"$PREFIX/lib:\$LD_LIBRARY_PATH\""
    echo ""
  fi
fi

echo ""
echo "Available commands:"
echo "  - dwg2dxf, dxf2dwg, dwg2SVG"
echo "  - dwgread, dwgwrite, dwgadd"
echo "  - dwggrep, dwglayers, dwgrewrite"
echo ""
