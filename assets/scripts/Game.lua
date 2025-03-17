move = function(schedule)
    local sys = function(w, singletons, e, idx, cmpts)
        local type0 = CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE)

        local type1 = CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST_SINGLETON)

        local cmptPtr = cmpts:GetCmpt(type0)
        local singletonPtr = singletons:GetSingleton(type1)

        local translate = Translation.voidp(cmptPtr:Ptr())
        local worldtime = WorldTime.voidp(singletonPtr:Ptr())

        -- 1 x, 2 y, 3 z
        translate.value[2] = math.sin(worldtime.elapsedTime)
    end

    local filter = ArchetypeFilter.new();
    filter.all:add(CmptAccessType.new("My::MyGE::MeshFilter", AccessMode.LATEST))
    local cLocator = CmptLocator.new(CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE), 1)
    local sLocator = SingletonLocator.new(CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST_SINGLETON), 1)
    LuaSystem.RegisterEntityJob(schedule, sys, "move", filter, cLocator, sLocator, true)
end
LuaSystem.RegisterSystem(world, "move", move)
