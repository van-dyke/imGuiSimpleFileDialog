#pragma once

#include <iostream>
#include "imgui.h"
#include "imgui_internal.h"
#include "FileSystemElement.h"
#include "Platform.h"
namespace ImGuiFileSystem
{

	template<typename FsImpl>
	class FileDialog
	{
	public:
		FileDialog() : m_pFsImpl(std::make_unique<FsImpl>())
		{}

		FileDialog(FileDialog const&) = delete;
		void operator=(FileDialog const&) = delete;

		void ReScanDrives()
		{
			m_pFsImpl->ReScanDrives();
		}

		static FileDialog& GetInstance()
		{
			static FileDialog< FsImpl > instance;
			return instance;
		}

		template<typename F>
		void Open(F Func)
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(800, 600));

			ImGui::OpenPopup("File");

			if (ImGui::BeginPopupModal("File"))
			{

				bool selected = false;

				std::vector<ImGuiFileSystem::FileSystemElement> tmp1 = ScanDirectory(GetCurrentSelectionPath());

				int idx = 0;

				ImGuiContext& g = *GImGui;
				const float itemsHeight = (g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y * 2.0f) * 4.0f + g.Style.WindowPadding.y * 2.0f;
				ImVec2 size = ImVec2(ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y - itemsHeight);

				ImGui::BeginChild("##FileDialog_FileList", size);

				for (auto const & f : tmp1)
				{
					//if (ImGui::Selectable((m_prefix_map[f.m_dir_element] + f.m_name).c_str(), selected))
					if (ImGui::Selectable((m_pFsImpl->GetPrefix(f.m_dir_element) + f.m_name).c_str(), selected))
					{

						//m_dialog_selection = f.m_name;
						m_pFsImpl->SetDialogSelection(f.m_name);

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

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0, 255, 0, 255 });

				ImGui::Text("Selected: ");
				ImGui::SameLine();
				ImGui::Text(m_pFsImpl->GetDialogSelection().c_str());

				ImGui::PopStyleColor();

				ImGui::Separator();

				if (ImGui::Button("Select", ImVec2(120, 0)))
				{
					Func(GetCurrentSelectionPath());
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
			return m_pFsImpl->ScanDirectory(root);
		}

		void Select(std::vector<FileSystemElement> const & fsys, int idx)
		{
			m_pFsImpl->Select(fsys, idx);
		}

		std::string GetCurrentSelectionPath()
		{
			return m_pFsImpl->GetCurrentSelectionPath();
		}

	private:

		std::unique_ptr< FsImpl > m_pFsImpl;
	};

}

