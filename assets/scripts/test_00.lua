luaCmptType0 = CmptType.new("Cmpt0")
luaCmptType1 = CmptType.new("Cmpt1")

-- Cmpt0 : 64 bytes
--  0 - 16 str buffer -> (init) 128 bytes string
-- 16 - 24 double {0}
-- 24 - 28 int32  {0}
-- 28 - 32 int32  {0}
-- 32 - 64 str[32] ""

default_ctor = function(ptr)
    print("default ctor")
    local cmpt = LuaBuffer.new(ptr, 64)
    LuaMemory.Set(ptr, 0, 64)
    local str = LuaMemory.Malloc(128)
    local strbuf = LuaBuffer.new(str, 128)
    strbuf:SetCString(0, "hello world!")
    cmpt:SetBuffer(0, strbuf)
end

copy_ctor = function(dst, src)
    print("copy ctor")
    local cmpt_dst = LuaBuffer.new(dst, 64)
    local cmpt_src = LuaBuffer.new(src, 64)
    LuaMemory.Copy(dst, src, 64)

    local strbuf_src = cmpt_src:GetBuffer(0)
    local str_dst = LuaMemory.Malloc(strbuf_src.size)
    local strbuf_dst
    LuaBuffer.new(str_dst, strbuf_src.size)
    LuaMemory.Copy(str_dst, strbuf_src.ptr, strbuf_src.size)

    cmpt_dst:SetBuffer(0, strbuf_dst)
end

move_ctor = function(dst, src)
    print("move ctor")
    local cmpt_dst = LuaBuffer.new(dst, 64)
    local cmpt_src = LuaBuffer.new(src, 64)
    LuaMemory.Copy(dst, src, 64)

    local empty_buffer = LuaBuffer.new()
    cmpt_src:SetBuffer(0, empty_buffer)
end

move_assignment = function(dst, src)
    print("move assignment")
    local cmpt_dst = LuaBuffer.new(dst, 64)
    local cmpt_src = LuaBuffer.new(src, 64)
    local strbuf_dst = cmpt_dst:GetBuffer(0)
    LuaMemory.Free(strbuf_dst.ptr)
    LuaMemory.Copy(dst, src, 64)
    local empty_buffer = LuaBuffer.new()
    cmpt_src:SetBuffer(0, empty_buffer)
end

dtor = function(ptr)
    print("dtor")
    local cmpt = LuaBuffer.new(ptr, 64)
    local strbuf = cmpt:GetBuffer(0)
    LuaMemory.Free(strbuf.ptr)
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

f = function(schedule)
    local g = function(w, singletons, beginIdx, chunk)
        local luaCmptType0 = CmptType.new("Cmpt0")
        local luaCmptType1 = CmptType.new("Cmpt1")
        local entityType = CmptType.new("My::MyECS::Entity")
        local num = chunk:EntityNum()
        local arrayCmpt0 = chunk:GetCmptArray(luaCmptType0)
        local arrayEntity = chunk:GetCmptArray(entityType)
        local entity_buf = LuaBuffer.new(arrayEntity, num)
        local em = w:GetEntityMngr()
        for i = 0, num - 1 do
            local cmpt0 = LuaBuffer.new(LuaMemory.Offset(arrayCmpt0, 64 * i), 64)
            local str = cmpt0:GetBuffer(0)
            print(str:GetCString(0))
            local e = entity_buf:GetEntity(16 * i)
            print(e:Idx())
            print(e:Version())
            if not w.entityMngr:Have(e, luaCmptType1) then
                w:AddCommand(function()
                    w.entityMngr:Attach(e, luaCmptType1, 1)
                end)
            end
        end
    end
    local filter = ArchetypeFilter.new()
    filter.all:add(CmptAccessType.new("Cmpt0", AccessMode.LATEST))
    LuaECSAgency.RegisterChunkJob(schedule, g, "test", filter, SingletonLocator.new(), true)
    world:RunJob(function()
        local num = world.entityMngr:TotalEntityNum()
        print("world's entity num : " .. num)
    end, SingletonLocator.new())
end
LuaSystem001 = world.systemMngr:Register("LuaSystem-001", f)
world.systemMngr:Activate(LuaSystem001)

parent = Parent.new()
e = parent.value
print(parent.value:Idx())
parent.value = Entity.new(1111, 2222)
print(parent.value:Idx())
print(parent.value:Version())
