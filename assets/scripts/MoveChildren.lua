MoveChildrenSystem_OnUpdate = function(schedule)
    local MoveChildren = function(w, singletons, e, idx, cmpts)
        local type0 = My.MyECS.CmptAccessType.new("My::MyGE::Translation", AccessMode.WRITE)

        local type1 = My.MyECS.CmptAccessType.new("My::MyGE::WorldTime", AccessMode.LATEST)

        local cmptPtr = cmpts:GetCmpt(type0)
        local singletonPtr = singletons:GetSingleton(type1)

        local translate = My.MyGE.Translation.voidp(cmptPtr:Ptr())
        local worldtime = My.MyGE.WorldTime.voidp(singletonPtr:Ptr())

        -- 1 x, 2 y, 3 z
        translate.value[1] = translate.value[1] + worldtime.deltaTime * math.cos(worldtime.elapsedTime)
    end

    local cLocatorTypes = LuaArray_CmptAccessType.new()
    cLocatorTypes:PushBack(My.MyECS.CmptAccessType.new("My::MyGE::Translation", My.MyECS.AccessMode.WRITE))
    local cLocator = My.MyECS.CmptLocator.new(cLocatorTypes:ToConstSpan())

    local sLocatorTypes = LuaArray_CmptAccessType.new()
    sLocatorTypes:PushBack(My.MyECS.CmptAccessType.new("My::MyGE::WorldTime", My.MyECS.AccessMode.LATEST))
    local sLocator = My.MyECS.SingletonLocator.new(sLocatorTypes:ToConstSpan())

    local childFilter = My.MyECS.ArchetypeFilter.new();
    childFilter.all:add(My.MyECS.CmptAccessType.new("My::MyGE::MeshFilter", My.MyECS.AccessMode.LATEST))
    childFilter.all:add(My.MyECS.CmptAccessType.new("My::MyGE::Parent", My.MyECS.AccessMode.LATEST))

    My.MyGE.LuaECSAgency
        .RegisterEntityJob(schedule, MoveChildren, "MoveChildren", true, childFilter, cLocator, sLocator)
end

MoveChildrenSystemID = world.systemMngr.systemTraits:Register("MoveChildrenSystem")
world.systemMngr.systemTraits:RegisterOnUpdate(MoveChildrenSystemID,
    My.MyGE.LuaECSAgency.SafeOnUpdate(MoveChildrenSystem_OnUpdate))
world.systemMngr:Activate(MoveChildrenSystemID)
