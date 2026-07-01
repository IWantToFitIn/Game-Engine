import xml.etree.ElementTree as ET
import re
import itertools
import sys
import os 

class Parser:
	def __init__(self, tokens):
		self.tokens = tokens
		self.pos = 0
	def peek(self):
			return self.tokens[self.pos] if self.pos < len(self.tokens) else None
	def consume(self):
		token = self.peek()
		self.pos += 1
		return token
	def parse(self):
		if not self.tokens:
			return None
		return self.parseOr()
	def parseOr(self):
		nodes = [self.parseAnd()]
		while self.peek() == ',':
			self.consume()
			nodes.append(self.parseAnd())
		return {"anyOf": nodes} if len(nodes) > 1 else nodes[0]
	def parseAnd(self):
		nodes = [self.parseFactor()]
		while self.peek() == '+':
			self.consume()
			nodes.append(self.parseFactor())
		return {"allOf": nodes} if len(nodes) > 1 else nodes[0]
	def parseFactor(self):
		token = self.peek()
		if token == '(':
			self.consume()
			node = self.parseOr()
			if(self.peek() == ')'):
				self.consume()
			return node
		else:
			return self.consume()


def tokenize(expr):
	return re.findall(r'[A-Za-z0-9_]+|[\+\,\(\)]', expr)

def astToDnf(node):
	if node is None:
		return []
	if isinstance(node, str):
		return [[node]]
	if "anyOf" in node:
		result = []
		for child in node["anyOf"]:
			result.extend(astToDnf(child))
		return result
	if "allOf" in node:
		children = [astToDnf(child) for child in node["allOf"]]
		result = []
		for combo in itertools.product(*children):
			path = []
			for subPath in combo:
				path.extend(subPath)
			result.append(path)
		return result
	return []

def generateMap(xmlPath):
	tree = ET.parse(xmlPath)
	root = tree.getroot()

	dependencyMap = {}
	for ext in root.findall('.//extensions/extension'):
		name = ext.get("name")
		if not ("vulkan" in ext.get("supported").split(',')):
			continue
		
		extType = ext.get("type", "unknown")
		depends = ext.get("depends")
		requires = ext.get("requires")

		paths = []
		if depends:
			ast = Parser(tokenize(depends)).parse()
			paths = astToDnf(ast)
		elif requires:
			reqList = [r.strip() for r in requires.split(",") if r.strip()]
			paths = [reqList] if reqList else []
		
		dependencyMap[name] = {
			"type": extType,
			"paths": paths
		}
	return dependencyMap

def generateHpp(xmlPath, hppPath):
	map = generateMap(xmlPath)
	names = sorted(map.keys())

	with open(hppPath, "w") as f:
		f.write("#pragma once\n")
		f.write("#include<string_view>\n")
		f.write("#include<vector>\n\n")

		f.write("enum class ExtensionType {\n")
		f.write("	Unknown, Device, Instance\n")
		f.write("};\n\n")

		f.write("struct Mapping{\n")
		f.write("	std::string_view extensionName;\n")
		f.write("	ExtensionType exType;\n")
		f.write("	std::vector<std::vector<std::string_view>> depPaths;\n")
		f.write("};\n\n")

		f.write("inline const std::vector<Mapping> gDependencyMap = {\n")
		for ext in names:
			extType = map[ext]["type"]
			paths = map[ext]["paths"]

			if extType == "instance":
				enumVal = "ExtensionType::Instance"
			elif extType == "device":
				enumVal = "ExtensionType::Device"
			else:
				enumVal = "ExtensionType::Unknown"
			
			f.write(f'	{{ "{ext}", {enumVal}, {{')

			cppPaths = []
			for path in paths:
				cppItems = ", ".join(f'"{item}"' for item in path)
				cppPaths.append(f'{{ {cppItems} }}')
			f.write(", ".join(cppPaths) + "} },\n")
		f.write("};\n\n")
		
if __name__ == "__main__":
	if len(sys.argv) > 2:
		xmlPath = sys.argv[1]
		hppPath = sys.argv[2]
		outDir = os.path.dirname(hppPath)
		if outDir:
			os.makedirs(outDir, exist_ok=True)
		generateHpp(xmlPath, hppPath)
	else:
		print("proper arguments not provided")