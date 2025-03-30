MoveRootSystem_OnUpdate = function(schedule)
    local MoveRoot = function(w, singletons, e, idx, cmpts)
        local type0 = My.MyECS.CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE)

        local type1 = My.MyECS.CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST)

        local cmptPtr = cmpts:GetCmpt(type0)
        local singletonPtr = singletons:GetSingleton(type1)

        local translate = My.MyGE.Translation.voidp(cmptPtr:Ptr())
        local worldtime = My.MyGE.WorldTime.voidp(singletonPtr:Ptr())

        -- 1 x, 2 y, 3 z
        translate.value[2] = translate.value[2] - worldtime.deltaTime * math.sin(worldtime.elapsedTime)
    end

    local cLocatorTypes = LuaArray_CmptAccessType.new()
    cLocatorTypes:PushBack(My.MyECS.CmptAccessType.new("My::MyGE::Translation", My.MyECS.AccessMode.WRITE))
    local cLocator = My.MyECS.CmptLocator.new(cLocatorTypes:ToConstSpan())

    local sLocatorTypes = LuaArray_CmptAccessType.new()
    sLocatorTypes:PushBack(My.MyECS.CmptAccessType.new("My::MyGE::WorldTime", My.MyECS.AccessMode.LATEST))
    local sLocator = My.MyECS.SingletonLocator.new(sLocatorTypes:ToConstSpan())

    local rootFilter = My.MyECS.ArchetypeFilter.new();
    rootFilter.all:add(My.MyECS.CmptAccessType.new("My::MyGE::MeshFilter", My.MyECS.AccessMode.LATEST))
    rootFilter.all:add(My.MyECS.CmptAccessType.new("My::MyGE::Children", My.MyECS.AccessMode.LATEST))

    My.MyGE.LuaECSAgency.RegisterEntityJob(schedule, MoveRoot, "MoveRoot", true, rootFilter, cLocator, sLocator)
end

MoveRootSystemID = world.systemMngr.systemTraits:Register("MoveRootSystem")
world.systemMngr.systemTraits:RegisterOnUpdate(MoveRootSystemID,
    My.MyGE.LuaECSAgency.SafeOnUpdate(MoveRootSystem_OnUpdate))
world.systemMngr:Activate(MoveRootSystemID)
