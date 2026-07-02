import argparse
import os
import struct
import sys
from pathlib import Path

def main():
	parser = argparse.ArgumentParser(description="Convert a compiled SPIRV shader into a C++ header")
	parser.add_argument("input", help="Path to compiled .spv file")
	parser.add_argument("output", help="Path to output .hpp file")

	args = parser.parse_args()

	inPath = args.input
	outPath = args.output
	baseName = Path(os.path.basename(inPath)).with_suffix('')
	varName = "g" + f"{baseName}".title()

	if not os.path.exists(inPath):
		print(f"Error: input file '{inPath}' not found", file=sys.stderr)
		sys.exit(1)
	
	with open(inPath, "rb") as f:
		data = f.read()
	
	if len(data) % 4 != 0:
		padding = 4 - (len(data) % 4)
		data += b'\x00' * padding
	
	wordCount = len(data) // 4
	words = struct.unpack(f"<{wordCount}I", data)
	wordsPerLine = 8

	header = [
		"#pragma once",
		"#include<cstdint>",
		"#include<vector>",
		"",
		f"inline const std::vector<uint32_t> {varName} = {{"
	]
	for i in range(0, len(words), wordsPerLine):
		chunk = words[i : i + wordsPerLine]
		line = "	" + ", ".join([f"0x{w:08x}" for w in chunk])
		if i + wordsPerLine < len(words):
			line += ","
		header.append(line)
	header.extend([
		"};",
		""
	])

	with open(outPath, "w") as f:
		f.write("\n".join(header))

if __name__ == "__main__":
	main()