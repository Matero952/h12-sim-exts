-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)
    links {}
-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
    { "omni", ext.target_dir.."/omni" },
}

-- Build the C++ plugin that will be loaded by the extension.
-- The plugin must implement the omni::ext::IExt interface to
-- be automatically loaded by the extension system at startup.
project_ext_plugin(ext, "h12_assets_ext.cpp.plugin")
    local plugin_name = "h12_assets_ext.cpp"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name }
    exceptionhandling "On"
    rtti "On"
