MoveSystem = function(schedule)
    local MoveChildren = function(w, singletons, e, idx, cmpts)
        local type0 = CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE)

        local type1 = CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST_SINGLETON)

        local cmptPtr = cmpts:GetCmpt(type0)
        local singletonPtr = singletons:GetSingleton(type1)

        local translate = Translation.voidp(cmptPtr:Ptr())
        local worldtime = WorldTime.voidp(singletonPtr:Ptr())

        -- 1 x, 2 y, 3 z
        translate.value[1] = translate.value[1] + worldtime.deltaTime * math.cos(worldtime.elapsedTime)
    end

    local MoveRoot = function(w, singletons, e, idx, cmpts)
        local type0 = CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE)

        local type1 = CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST_SINGLETON)

        local cmptPtr = cmpts:GetCmpt(type0)
        local singletonPtr = singletons:GetSingleton(type1)

        local translate = Translation.voidp(cmptPtr:Ptr())
        local worldtime = WorldTime.voidp(singletonPtr:Ptr())

        -- 1 x, 2 y, 3 z
        translate.value[2] = translate.value[2] - worldtime.deltaTime * math.sin(worldtime.elapsedTime)
    end

    local childFilter = ArchetypeFilter.new();
    childFilter.all:add(CmptAccessType.new("My::MyGE::MeshFilter", AccessMode.LATEST))
    childFilter.all:add(CmptAccessType.new("My::MyGE::Parent", AccessMode.LATEST))
    local cLocator = CmptLocator.new(CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE), 1)
    local sLocator = SingletonLocator.new(CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST_SINGLETON), 1)

    LuaSystem.RegisterEntityJob(schedule, MoveChildren, "MoveChildren", childFilter, cLocator, sLocator, true)

    local rootFilter = ArchetypeFilter.new();
    rootFilter.all:add(CmptAccessType.new("My::MyGE::MeshFilter", AccessMode.LATEST))
    rootFilter.all:add(CmptAccessType.new("My::MyGE::Children", AccessMode.LATEST))
    LuaSystem.RegisterEntityJob(schedule, MoveRoot, "MoveRoot", rootFilter, cLocator, sLocator, true)
end
LuaSystem.RegisterSystem(world, "MoveSystem", MoveSystem)
