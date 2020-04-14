#pragma once

#if defined(_WIN32) ||  defined(_WIN64)
#include "FileDialogImplWin.h"
namespace ImGuiFileSystem
{
	using FileDialogImpl = ImGuiFileSystem::FileDialogImplWin;
}
#elif defined(__linux__)
#include "FileDialogImplLin.h"
namespace ImGuiFileSystem
{
	using FileDialogImpl = ImGuiFileSystem::FileDialogImplLin
}
#endif