#!/bin/sh

# Test script for VSS to SVG conversion
# This script downloads a test VSS file and verifies that vss2svg-conv can convert it

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Determine script directory and project root
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== VSS Conversion Test ==="
echo "Script directory: ${SCRIPT_DIR}"
echo "Project root: ${PROJECT_ROOT}"
echo

# Create test directory
TEST_DIR="${SCRIPT_DIR}/test_output"
echo "Creating test directory: ${TEST_DIR}"
rm -rf "${TEST_DIR}"
mkdir -p "${TEST_DIR}"
cd "${TEST_DIR}"

echo "Step 1: Downloading test VSS file..."
if ! wget -q "http://www.visiocafe.com/downloads/hp/HPE-Recent_2025-10-14.zip"; then
    echo -e "${RED}ERROR: Failed to download test file${NC}"
    exit 1
fi

echo "Step 2: Extracting files..."
if ! unzip -q HPE-Recent_2025-10-14.zip; then
    echo -e "${RED}ERROR: Failed to extract test file${NC}"
    exit 1
fi

echo "Step 3: Creating output directory..."
mkdir -p output

echo "Step 4: Finding VSS files..."
VSS_FILES=$(find . -name "*.vss")
if [ -z "$VSS_FILES" ]; then
    echo -e "${RED}ERROR: No VSS files found in extracted archive${NC}"
    exit 1
fi

echo "Found VSS files:"
echo "$VSS_FILES"
echo

echo "Step 5: Running VSS to SVG conversion..."
# Run conversion with path adjusted for tests/ directory
if find . -name "*.vss" -exec "${PROJECT_ROOT}/build/vss2svg-conv" -o output/ -s 50 -i {} \;; then
    CONVERSION_EXIT_CODE=$?
    echo "Conversion command completed with exit code: ${CONVERSION_EXIT_CODE}"
else
    CONVERSION_EXIT_CODE=$?
    echo -e "${RED}Conversion command failed with exit code: ${CONVERSION_EXIT_CODE}${NC}"
fi

echo
echo "Step 6: Validating results..."

# Check if SVG files were generated
SVG_COUNT=$(ls -A output/*.svg 2>/dev/null | wc -l)
SVG_FILES=$(ls -la output/*.svg 2>/dev/null)

if [ $CONVERSION_EXIT_CODE -eq 0 ] && [ $SVG_COUNT -gt 0 ]; then
    echo -e "${GREEN}✅ SUCCESS: VSS conversion test passed!${NC}"
    echo "SVG files generated: ${SVG_COUNT}"
    echo "Generated files:"
    echo "$SVG_FILES"
    echo
    echo -e "${GREEN}Test completed successfully!${NC}"
    exit 0
else
    echo -e "${RED}❌ FAIL: VSS conversion test failed${NC}"
    echo "Exit code: ${CONVERSION_EXIT_CODE}"
    echo "SVG files found: ${SVG_COUNT}"
    echo
    echo "Output directory contents:"
    ls -la output/ || echo "No output directory found"
    echo
    echo -e "${YELLOW}Possible issues:${NC}"
    echo "1. vss2svg-conv executable not found (check if project was built)"
    echo "2. Missing dependencies (fontconfig, etc.)"
    echo "3. Build failed or incomplete"
    echo
    echo "To debug further, check:"
    echo "- Build status: ls -la ${PROJECT_ROOT}/build/"
    echo "- Executable: ls -la ${PROJECT_ROOT}/build/vss2svg-conv"
    echo "- Dependencies: run ${SCRIPT_DIR}/setup-dependencies.sh"
    exit 1
fi
