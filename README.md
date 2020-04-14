# Simple File Dialog for imGUI library

Under coding. Stay tuned.

Example:

```cpp
#include "FileDialog.h"

using FD = ImGuiFileSystem::FileDialog< ImGuiFileSystem::FileDialogImpl >;
...

FD::GetInstance().Open( [](std::string selected_path) { /* process selected path */ } );

```
![](FileDialog.gif)
