workspace "SchoolProject"
    architecture "x64"
    configurations {"Debug", "Release"}
    systemversion "latest"

    project "SchoolProject"
        location "SchoolProject"
        files {"%{prj.location}/**.cpp", "%{prj.location}/**.h","%{prj.location}/**.hlsl", "%{prj.location}/**.hlsli"}
        kind "WindowedApp"
        language "C++"
        cppdialect "C++14"

        targetdir "bin/%{cfg.buildcfg}"
        objdir "objects/%{cfg.buildcfg}"

        includedirs{"DirectXTK/include/"}

        links{
            "d3d11",
        }
	vpaths{["Shaders"] = "**.hlsl", ["Headers"] = "**.h", ["Sources"] = {"**.c", "**.cpp"}, ["Buffers"] = {"**Buffer"}, ["PreCompiledHeaders"] = "**pch", ["Main"] = "**main.cpp", ["ECS"] = {"**Component", "**System","**Factory","**ECS"}, ["Core"] = {"**Render","**Core"},["imGUI"] = {"**imgui","**imstb","**imconfig"}}
        filter "configurations:Debug"
            libdirs{"DirectXTK/bin/x64/Debug_lib/"}
            links{"DirectXTK"}

            defines{"_DEBUG", "_UNICODE", "UNICODE"}
            symbols "On"
        filter "configurations:Release"
            libdirs{"DirectXTK/bin/x64/Release_lib/"}
            links{"DirectXTK"}
            defines{"NDEBUG", "_UNICODE", "UNICODE"}
            optimize "On"

    filter "*"
        local ws = "$(ProjectDir)%%(Filename).cso"
        files("*.hlsl")
            shadermodel("5.0")
            shaderobjectfileoutput(ws)

        filter("files:**_vs.hlsl")
            shadertype("Vertex")

        filter("files:**_ps.hlsl")
            shadertype("Pixel")
        
        filter("files:**_gs.hlsl")
            shadertype("Geometry")

        filter("files:**_cs.hlsl")
            shadertype("Compute")