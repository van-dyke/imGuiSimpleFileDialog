#pragma once

#include <iostream>
#include <bitset>
#include <vector>
#include <map>
#include <windows.h>
#include <filesystem>
#include "imgui.h"

namespace ImGuiFileSystem
{

	namespace fs = std::filesystem;

	enum class dir_element { DISK, DIR, FILE };

	struct FileSystemElement
	{
		std::string m_name;
		dir_element m_dir_element;
	};

	class FileDialog
	{
	public:
		FileDialog()
		{
			for (char c = A_ASCII_CODE; c < Z_ASCII_CODE; ++c)
				m_drives_letters.push_back(std::string{ c } +std::string{ ":\\" });

			m_prefix_map.insert(std::make_pair(dir_element::DISK,	std::string("[DISK] ")));
			m_prefix_map.insert(std::make_pair(dir_element::DIR,	std::string("[DIR] ")));
			m_prefix_map.insert(std::make_pair(dir_element::FILE,	std::string("[FILE] ")));

			ReScanDrives();
		}

		FileDialog(FileDialog const&)		= delete;
		void operator=(FileDialog const&)	= delete;

		void ReScanDrives()
		{
			std::bitset< 8 * sizeof(DWORD) > drives_bitmask{ GetLogicalDrives() };

			for (size_t i = 0; i < drives_bitmask.size(); ++i)
			{
				if (drives_bitmask.test(i))
					m_current_drives_letters.push_back(m_drives_letters[i]);
			}
		}

		static FileDialog& GetInstance()
		{
			static FileDialog instance; 
			return instance;
		}

		template<typename F>
		void Open( F Func )
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(800, 600));


			ImGui::OpenPopup("File");


			if (ImGui::BeginPopupModal("File"))
			{

				bool selected = false;

				std::vector<ImGuiFileSystem::FileSystemElement> tmp1 = ScanDirectory( GetCurrentSelectionPath() );

				int idx = 0;

				ImGuiContext& g = *GImGui;
				const float itemsHeight = (g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y * 2.0f) * 4.0f + g.Style.WindowPadding.y * 2.0f;
				ImVec2 size = ImVec2(ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y - itemsHeight);

				ImGui::BeginChild("##FileDialog_FileList", size);

				for (auto const & f : tmp1)
				{

					if (ImGui::Selectable((m_prefix_map[f.m_dir_element] + f.m_name).c_str(), selected))
					{
						
						m_dialog_selection = f.m_name;

						if (f.m_dir_element == ImGuiFileSystem::dir_element::FILE)
						{

						}
						else
						{
							Select(tmp1, idx);
						}
					}

					idx++;
				}

				ImGui::EndChild();

				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0, 255, 0, 255});

				ImGui::Text("Selected: ");
				ImGui::SameLine();
				ImGui::Text(m_dialog_selection.c_str());

				ImGui::PopStyleColor();

				ImGui::Separator();

				if (ImGui::Button("Select", ImVec2(120, 0))) 
				{
					Func( GetCurrentSelectionPath() );
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				
				if (ImGui::Button("Cancel", ImVec2(120, 0))) 
				{ 
					ImGui::CloseCurrentPopup(); 
				}

				ImGui::EndPopup();
			}
		}

		std::vector<FileSystemElement> ScanDirectory(std::string root = "")
		{
			if (prev_selection == root)
				return m_cache;

			std::error_code ec;

			std::vector<FileSystemElement> tmp;
			std::vector<fs::path> paths;

			if (root.empty())
			{
				for (auto const& p : m_current_drives_letters)
					tmp.push_back(FileSystemElement{ p, dir_element::DISK });

				return tmp;
			}


			tmp.push_back(FileSystemElement{ "..", dir_element::DIR });


			std::filesystem::directory_iterator dirpos{ root, std::filesystem::directory_options::skip_permission_denied, ec };

			for (std::filesystem::path const & p : dirpos)
			{

				if (fs::is_regular_file(p, ec))
				{
					auto filename = p.filename();
					auto found = filename.string().find_last_of("\\");

					tmp.push_back(FileSystemElement{ filename.string().substr(found + 1), dir_element::FILE });
				}

				if (fs::is_directory(p, ec))
				{
					auto found = p.string().find_last_of("\\");
					tmp.push_back(FileSystemElement{ p.string().substr(found + 1), dir_element::DIR });
				}

			}
			
			prev_selection = root;

			m_cache = tmp;

			return tmp;

		}

		void Select(std::vector<FileSystemElement> const & fsys, int idx)
		{
			if (current_selection.empty())
			{
				current_selection = fsys[idx].m_name;
				return;
			}

			if (idx == 0)
			{
				auto found = current_selection.find_last_of("\\");
				current_selection = current_selection.substr(0, found);

				if (current_selection.length() == 2 && current_selection.find(":"))
					current_selection = "";
			}
			else
			{
				current_selection += "\\" + fsys[idx].m_name;
			}
		}

		std::string GetCurrentSelectionPath()
		{
			return current_selection;
		}

	private:
		static constexpr char A_ASCII_CODE = 65;
		static constexpr char Z_ASCII_CODE = 90;

		std::vector<std::string> m_drives_letters;
		std::vector<std::string> m_current_drives_letters;

		std::vector<FileSystemElement> m_cache;
		std::map<dir_element, std::string> m_prefix_map;

		std::string current_selection = {};
		std::string prev_selection = {"xxx"};
		std::string m_dialog_selection = {};
	};

}

