#pragma once

#include <string>

namespace ImGuiFileSystem
{
	enum class dir_element { DISK, DIR, FILE };

	class FileSystemElement
	{
	public:

		FileSystemElement(std::string s, dir_element el) : m_name(s), m_dir_element(el)
		{ }

		~FileSystemElement() = default;

		FileSystemElement(const FileSystemElement&) = default;

		FileSystemElement(FileSystemElement&& other) noexcept
		{
			swap(*this, other);
		}

		FileSystemElement& operator=(FileSystemElement other)
		{
			swap(*this, other);
			return *this;
		}


		friend void swap(FileSystemElement &A, FileSystemElement &B)
		{
			std::swap(A.m_name, B.m_name);
			std::swap(A.m_dir_element, B.m_dir_element);
		}

		std::string m_name;
		dir_element m_dir_element;
	};
}