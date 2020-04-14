# imGuiSimpleFileDialog
Simple file dialog for ImGui library 

Under coding. Stay tuned.

Example:

```cpp

using fd = ImGuiFileSystem::FileDialogImpl;
...
ImGuiFileSystem::FileDialog< fd >::GetInstance().Open( [](std::string selected_path) { /* process selected path */ } );

```
![](FileDialog.gif)
