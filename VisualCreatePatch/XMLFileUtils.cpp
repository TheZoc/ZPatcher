#include "stdafx.h"
#include <fstream>
#include <string>

#include "XMLFileUtils.h"
#include "LogSystem.h"

#include "../libs/rapidxml-1.13/rapidxml.hpp"
#include "../libs/rapidxml-1.13/rapidxml_print.hpp"

constexpr char CURRENT_VERSION[] = "3.0";

constexpr char ATTRIBUTE_VERSION[]     = "version";
constexpr char ATTRIBUTE_ENCODING[]    = "encoding";

constexpr char NODE_ADD[]              = "add";
constexpr char NODE_DELETE[]           = "delete";
constexpr char NODE_UPDATE[]           = "update";
constexpr char NODE_ZPATCH_FILE_LIST[] = "zPatchFileList";

constexpr int NODE_MAX_LENGTH          = 16; // Increase this value, if there are node names longer than this.

void XMLFileUtils::SaveXMLPatchFile(const ZPatcher::PatchFileList_t * patchFileList, const std::string& outputPatchFilename)
{
	if (!patchFileList)
		return;

	using namespace ZPatcher; // Log utilities
	Log(LOG, "Creating patch file: %s", outputPatchFilename.c_str());

	using namespace rapidxml;
	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute(ATTRIBUTE_VERSION, "1.0"));
	decl->append_attribute(doc.allocate_attribute(ATTRIBUTE_ENCODING, "utf-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, NODE_ZPATCH_FILE_LIST);
	root->append_attribute(doc.allocate_attribute(ATTRIBUTE_VERSION, CURRENT_VERSION));
	doc.append_node(root);

	// NOTE: Pay special attention here - this is a reverse iterator. Directories must be the last ones to be deleted!
	for (std::vector<std::string>::const_reverse_iterator ritr = patchFileList->RemovedFileList.crbegin(); ritr != patchFileList->RemovedFileList.crend(); ++ritr)
	{
		xml_node<>* del = doc.allocate_node(node_element, NODE_DELETE);
		del->value(doc.allocate_string(ritr->c_str()));
		root->append_node(del);
	}
	
	for (std::vector<std::string>::const_iterator itr = patchFileList->AddedFileList.cbegin(); itr < patchFileList->AddedFileList.cend(); ++itr)
	{
		xml_node<>* add = doc.allocate_node(node_element, NODE_ADD);
		add->value(doc.allocate_string(itr->c_str()));
		root->append_node(add);
	}

	for (std::vector<std::string>::const_iterator itr = patchFileList->ModifiedFileList.cbegin(); itr < patchFileList->ModifiedFileList.cend(); ++itr)
	{
		xml_node<>* update = doc.allocate_node(node_element, NODE_UPDATE);
		update->value(doc.allocate_string(itr->c_str()));
		root->append_node(update);
	}

	// Save to file
	std::ofstream xmlFileStream(outputPatchFilename);
	if (!xmlFileStream.is_open())
	{
		Log(LOG_FATAL, "Unable to open file for write: %s", outputPatchFilename.c_str());
		return;
	}

	xmlFileStream << doc;
	xmlFileStream.close();
	doc.clear();

	Log(LOG, "Success writing patch XML file: %s", outputPatchFilename.c_str());
}

bool XMLFileUtils::LoadXMLPatchFile(const std::string & inputPatchFilename, ZPatcher::PatchFileList_t * patchFileList)
{
	if (!patchFileList)
		return false;

	using namespace ZPatcher; // Log utilities
	Log(LOG, "Loading file: %s", inputPatchFilename.c_str());

	std::ifstream xmlFileStream(inputPatchFilename, std::ios::binary);

	if (!xmlFileStream.is_open())
	{
		Log(LOG_FATAL, "Unable to open: %s", inputPatchFilename.c_str());
		return false;
	}

	std::vector<char> xmlFile((std::istreambuf_iterator<char>(xmlFileStream)), std::istreambuf_iterator<char>());
	xmlFile.push_back('\0');

	rapidxml::xml_document<> doc;
	try
	{
		doc.parse<0>(xmlFile.data());
	}
	catch (const rapidxml::parse_error& e)
	{
		Log(LOG_FATAL, "Error parsig XML file: %s\n%s", e.what(), e.where<char>());
		return false;
	}

	rapidxml::xml_node<>* zPatchFileListNode = doc.first_node(NODE_ZPATCH_FILE_LIST);

	if (!zPatchFileListNode)
	{
		Log(LOG_FATAL, "Error: Missing node %s", NODE_ZPATCH_FILE_LIST);
		return false;
	}

	rapidxml::xml_attribute<>* versionNode = zPatchFileListNode->first_attribute(ATTRIBUTE_VERSION);
	if (!versionNode)
	{
		Log(LOG_FATAL, "Error: Missing node %s", ATTRIBUTE_VERSION);
		return false;
	}

	if (strcmp(CURRENT_VERSION, versionNode->value()) != 0)
	{
		Log(LOG_FATAL, "Error: Wrong version: Expected %s - Got %s", CURRENT_VERSION, versionNode->value());
		return false;
	}

	rapidxml::xml_node<>* actionNode = zPatchFileListNode->first_node();
	if (!actionNode)
	{
		Log(LOG_FATAL, "Error: Empty patch list detected");
		return false;
	}

	while (actionNode)
	{
		if (strncmp(actionNode->name(), NODE_DELETE, NODE_MAX_LENGTH) == 0)
		{
			// Remember: This is written and read in the reverse order!
			patchFileList->RemovedFileList.emplace(patchFileList->RemovedFileList.begin(), actionNode->value());
		}
		else if (strncmp(actionNode->name(), NODE_ADD, NODE_MAX_LENGTH) == 0)
		{
			patchFileList->AddedFileList.emplace_back(actionNode->value());
		}
		else if (strncmp(actionNode->name(), NODE_UPDATE, NODE_MAX_LENGTH) == 0)
		{
			patchFileList->ModifiedFileList.emplace_back(actionNode->value());
		}
		else
		{
			Log(LOG_WARNING, "Warning: Unknown action: %s", actionNode->value());
		}
		actionNode = actionNode->next_sibling();
	}

	Log(LOG, "Success importing patch data from: %s", inputPatchFilename.c_str());
	return true;
}
