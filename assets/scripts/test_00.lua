luaCmptType0 = My.MyECS.CmptType.new("Cmpt0")
luaCmptType1 = My.MyECS.CmptType.new("Cmpt1")

-- Cmpt0 : 64 bytes
--  0 - 16 str buffer -> (init) 128 bytes string
-- 16 - 24 double {0}
-- 24 - 28 int32  {0}
-- 28 - 32 int32  {0}
-- 32 - 64 str[32] ""

default_ctor = function(ptr)
    print("default ctor")
    local cmpt = My.MyGE.LuaBuffer.new(ptr, 64)
    My.MyGE.LuaMemory.Set(ptr, 0, 64)
    local str = My.MyGE.LuaMemory.Malloc(128)
    local strbuf = My.MyGE.LuaBuffer.new(str, 128)
    strbuf:SetCString(0, "hello world!")
    cmpt:SetBuffer(0, strbuf)
end

copy_ctor = function(dst, src)
    print("copy ctor")
    local cmpt_dst = My.MyGE.LuaBuffer.new(dst, 64)
    local cmpt_src = My.MyGE.LuaBuffer.new(src, 64)
    My.MyGE.LuaMemory.Copy(dst, src, 64)

    local strbuf_src = cmpt_src:GetBuffer(0)
    local str_dst = My.MyGE.LuaMemory.Malloc(strbuf_src.size)
    local strbuf_dst = My.MyGE.LuaBuffer.new(str_dst, strbuf_src.size)
    My.MyGE.LuaMemory.Copy(str_dst, strbuf_src.ptr, strbuf_src.size)

    cmpt_dst:SetBuffer(0, strbuf_dst)
end

move_ctor = function(dst, src)
    print("move ctor")
    local cmpt_dst = My.MyGE.LuaBuffer.new(dst, 64)
    local cmpt_src = My.MyGE.LuaBuffer.new(src, 64)
    My.MyGE.LuaMemory.Copy(dst, src, 64)

    local empty_buffer = My.MyGE.LuaBuffer.new()
    cmpt_src:SetBuffer(0, empty_buffer)
end

move_assignment = function(dst, src)
    print("move assignment")
    local cmpt_dst = My.MyGE.LuaBuffer.new(dst, 64)
    local cmpt_src = My.MyGE.LuaBuffer.new(src, 64)
    local strbuf_dst = cmpt_dst:GetBuffer(0)
    My.MyGE.LuaMemory.Free(strbuf_dst.ptr)
    My.MyGE.LuaMemory.Copy(dst, src, 64)
    local empty_buffer = My.MyGE.LuaBuffer.new()
    cmpt_src:SetBuffer(0, empty_buffer)
end

dtor = function(ptr)
    print("dtor")
    local cmpt = My.MyGE.LuaBuffer.new(ptr, 64)
    local strbuf = cmpt:GetBuffer(0)
    My.MyGE.LuaMemory.Free(strbuf.ptr)
end

--------------------------------------------------------------------

world.entityMngr.cmptTraits:RegisterSize(luaCmptType0, 64)
world.entityMngr.cmptTraits:RegisterName(luaCmptType0, "Cmpt0")

world.entityMngr.cmptTraits:RegisterDefaultConstructor(luaCmptType0, default_ctor)
world.entityMngr.cmptTraits:RegisterCopyConstructor(luaCmptType0, copy_ctor)
world.entityMngr.cmptTraits:RegisterMoveConstructor(luaCmptType0, move_ctor)
world.entityMngr.cmptTraits:RegisterMoveAssignment(luaCmptType0, move_assignment)
world.entityMngr.cmptTraits:RegisterDestructor(luaCmptType0, dtor)

world.entityMngr.cmptTraits:RegisterSize(luaCmptType1, 8)
world.entityMngr.cmptTraits:RegisterName(luaCmptType1, "Cmpt1")

onUpdate = function(schedule)
    local g = function(w, singletons, beginIdx, chunk)
        local luaCmptType0 = My.MyECS.CmptType.new("Cmpt0")
        local luaCmptType1 = My.MyECS.CmptType.new("Cmpt1")
        local entityType = My.MyECS.CmptType.new("My::MyECS::Entity")
        local num = chunk:EntityNum()
        local arrayCmpt0 = chunk:GetCmptArray(luaCmptType0)
        local arrayEntity = chunk:GetCmptArray(entityType)
        local entity_buf = My.MyGE.LuaBuffer.new(arrayEntity, num)
        local em = w:GetEntityMngr()
        for i = 0, num - 1 do
            local cmpt0 = My.MyGE.LuaBuffer.new(My.MyGE.LuaMemory.Offset(arrayCmpt0, 64 * i), 64)
            local str = cmpt0:GetBuffer(0)
            print(str:GetCString(0))
            local e = entity_buf:GetEntity(16 * i)
            print(e:Idx())
            print(e:Version())
            if not w.entityMngr:Have(e, luaCmptType1) then
                w:AddCommand(function()
                    w.entityMngr:Attach(e, luaCmptType1)
                end)
            end
        end
    end
    local filter = My.MyECS.ArchetypeFilter.new()
    filter.all:add(My.MyECS.CmptAccessType.new("Cmpt0", My.MyECS.AccessMode.LATEST))
    My.MyGE.LuaECSAgency.RegisterChunkJob(schedule, g, "test", filter, true)
    schedule:RegisterCommand(function(w)
        local num = w.entityMngr:TotalEntityNum()
        print("world's entity num : " .. num)
    end)
end
LuaSystem001ID = world.systemMngr.systemTraits:Register("LuaSystem-001")
world.systemMngr.systemTraits:RegisterOnUpdate(LuaSystem001ID, My.MyGE.LuaECSAgency.SafeOnUpdate(onUpdate))
world.systemMngr:Activate(LuaSystem001ID)
