#pragma once
#include <windows.h>
#include <filesystem>
#include <bitset>
#include <vector>
#include <map>
#include "FileSystemElement.h"

namespace ImGuiFileSystem
{
	namespace fs = std::filesystem;

	class FileDialogImplWin
	{
	public:
		FileDialogImplWin()
		{
			for (char c = A_ASCII_CODE; c < Z_ASCII_CODE; ++c)
				m_drives_letters.push_back(std::string{ c } +std::string{ ":\\" });

			m_prefix_map.insert(std::make_pair(dir_element::DISK, std::string("[DISK] ")));
			m_prefix_map.insert(std::make_pair(dir_element::DIR, std::string("[DIR] ")));
			m_prefix_map.insert(std::make_pair(dir_element::FILE, std::string("[FILE] ")));

			ReScanDrives();
		}

		FileDialogImplWin(FileDialogImplWin const&) = delete;
		void operator=(FileDialogImplWin const&) = delete;

		void ReScanDrives()
		{
			std::bitset< 8 * sizeof(DWORD) > drives_bitmask{ GetLogicalDrives() };

			for (size_t i = 0; i < drives_bitmask.size(); ++i)
			{
				if (drives_bitmask.test(i))
					m_current_drives_letters.push_back(m_drives_letters[i]);
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

		std::string const & GetPrefix(dir_element const &el)
		{
			return m_prefix_map[el];
		}

		std::string const & GetDialogSelection()
		{
			return m_dialog_selection;
		}

		void SetDialogSelection(std::string s)
		{
			m_dialog_selection = std::move(s);
		}

		void SetCurrentSelection(std::string s)
		{
			current_selection = std::move(s);
		}

	private:

		static constexpr char A_ASCII_CODE = 65;
		static constexpr char Z_ASCII_CODE = 90;

		std::vector<std::string> m_drives_letters;
		std::vector<std::string> m_current_drives_letters;

		std::vector<FileSystemElement> m_cache;
		std::map<dir_element, std::string> m_prefix_map;

		std::string current_selection = {};
		std::string prev_selection = { "xxx" };
		std::string m_dialog_selection = {};
	};
}