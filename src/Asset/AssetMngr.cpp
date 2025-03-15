//
// Created by Admin on 15/03/2025.
//

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/ScriptSystem/LuaScript.h>
#include <MyGE/_deps/tinyobjloader/tiny_obj_loader.h>
#include <MySRefl/MySRefl.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <any>
#include <fstream>
#include <functional>
#include <map>
#include <memory>

using namespace My::MyGE;

struct AssetMngr::Impl {
  // asset <-> path <-> guid

  struct Asset {
    xg::Guid localID;
    std::unique_ptr<void, std::function<void(void*)>> ptr;
    const std::type_info& typeinfo;

    template <typename T>
    static std::function<void(void*)> DefaultDeletor() noexcept {
      return [](void* p) { delete (T*)p; };
    }

    template <typename T>
    Asset(T* p)
        : ptr{p, DefaultDeletor<T>()}, typeinfo{typeid(std::decay_t<T>)} {}

    template <typename T>
    Asset(xg::Guid localID, T* p)
        : localID{localID},
          ptr{p, DefaultDeletor<T>()},
          typeinfo{typeid(std::decay_t<T>)} {}
  };

  std::unordered_map<const void*, std::filesystem::path> asset2path;
  std::multimap<std::filesystem::path, Asset> path2assert;

  std::map<std::filesystem::path, xg::Guid> path2guid;
  std::unordered_map<xg::Guid, std::filesystem::path> guid2path;

  static std::string LoadText(const std::filesystem::path& path);
  static rapidjson::Document LoadJSON(const std::filesystem::path& metapath);
  static Mesh* LoadObj(const std::filesystem::path& path);
};

AssetMngr::AssetMngr() : pImpl{new Impl} {}

AssetMngr::~AssetMngr() { delete pImpl; }

void AssetMngr::Clear() {
  pImpl->asset2path.clear();
  pImpl->path2assert.clear();
  pImpl->path2guid.clear();
  pImpl->guid2path.clear();
}

xg::Guid AssetMngr::AssetPathToGUID(const std::filesystem::path& path) const {
  auto target = pImpl->path2guid.find(path);
  return target == pImpl->path2guid.end() ? xg::Guid{} : target->second;
}

bool AssetMngr::Contains(const void* ptr) const {
  return pImpl->asset2path.find(ptr) != pImpl->asset2path.end();
}

const std::filesystem::path& AssetMngr::GetAssetPath(const void* ptr) const {
  static const std::filesystem::path ERROR;
  auto target = pImpl->asset2path.find(ptr);
  return target == pImpl->asset2path.end() ? ERROR : target->second;
}

const std::filesystem::path& AssetMngr::GUIDToAssetPath(
    const xg::Guid& guid) const {
  static const std::filesystem::path ERROR;
  auto target = pImpl->guid2path.find(guid);
  return target == pImpl->guid2path.end() ? ERROR : target->second;
}

void AssetMngr::ImportAsset(const std::filesystem::path& path) {
  assert(!path.empty() && path.is_relative());

  if (pImpl->path2guid.find(path) != pImpl->path2guid.end()) return;
  assert(std::filesystem::exists(path));
  auto metapath = std::filesystem::path{path}.concat(".meta");
  bool existMeta = std::filesystem::exists(metapath);
  assert(!existMeta || !std::filesystem::is_directory(metapath));
  if (path.extension() == ".lua") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else if (path.extension() == ".obj") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else if (path.extension() == ".hlsl") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else if (path.extension() == ".shader") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else if (path.extension() == ".png" || path.extension() == ".jpg" ||
             path.extension() == ".bmp" || path.extension() == ".hdr" ||
             path.extension() == ".tga") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else if (path.extension() == ".tex2d") {
    xg::Guid guid;
    if (!existMeta) {
      // generate meta file

      guid = xg::newGuid();

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("guid");
      writer.String(guid.str());
      writer.EndObject();

      auto dirPath = path.parent_path();
      if (!std::filesystem::is_directory(dirPath))
        std::filesystem::create_directories(dirPath);

      std::ofstream ofs(metapath);
      assert(ofs.is_open());
      ofs << sb.GetString();
      ofs.close();
    } else {
      rapidjson::Document doc = Impl::LoadJSON(metapath);
      guid = xg::Guid{doc["guid"].GetString()};
    }
    pImpl->path2guid.emplace(path, guid);
    pImpl->guid2path.emplace(guid, path);
  } else
    assert("not support" && false);
}

void* AssetMngr::LoadAsset(const std::filesystem::path& path) {
  ImportAsset(path);
  if (path.extension() == ".lua") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();

    auto str = Impl::LoadText(path);
    auto lua = new LuaScript(std::move(str));
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{lua});
    pImpl->asset2path.emplace(lua, path);
    return lua;
  } else if (path.extension() == ".obj") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();
    auto mesh = Impl::LoadObj(path);
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{mesh});
    pImpl->asset2path.emplace(mesh, path);
    return mesh;
  } else if (path.extension() == ".hlsl") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();

    auto str = Impl::LoadText(path);
    auto hlsl = new HLSLFile(std::move(str));
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{hlsl});
    pImpl->asset2path.emplace(hlsl, path);
    return hlsl;
  } else if (path.extension() == ".shader") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();

    auto shaderJSON = Impl::LoadJSON(path);
    auto guidstr = shaderJSON["hlslFile"].GetString();
    xg::Guid guid{guidstr};
    auto hlslTarget = pImpl->guid2path.find(guid);
    auto shader = new Shader;
    shader->hlslFile = hlslTarget != pImpl->guid2path.end()
                           ? LoadAsset<HLSLFile>(hlslTarget->second)
                           : nullptr;
    shader->shaderName = shaderJSON["shaderName"].GetString();
    shader->vertexName = shaderJSON["vertexName"].GetString();
    shader->fragmentName = shaderJSON["fragmentName"].GetString();
    shader->targetName = shaderJSON["targetName"].GetString();
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{shader});
    pImpl->asset2path.emplace(shader, path);
    return shader;
  } else if (path.extension() == ".png" || path.extension() == ".jpg" ||
             path.extension() == ".bmp" || path.extension() == ".hdr" ||
             path.extension() == ".tga") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();
    auto img = new Image(path.string());
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{img});
    pImpl->asset2path.emplace(img, path);
    return img;
  } else if (path.extension() == ".tex2d") {
    auto target = pImpl->path2assert.find(path);
    if (target != pImpl->path2assert.end()) return target->second.ptr.get();

    auto tex2dJSON = Impl::LoadJSON(path);
    auto guidstr = tex2dJSON["image"].GetString();
    xg::Guid guid{guidstr};
    auto imgTarget = pImpl->guid2path.find(guid);
    auto tex2d = new Texture2D;
    tex2d->image = imgTarget != pImpl->guid2path.end()
                       ? LoadAsset<Image>(imgTarget->second)
                       : nullptr;
    tex2d->wrapMode =
        static_cast<Texture2D::WrapMode>(tex2dJSON["wrapMode"].GetUint());
    tex2d->filterMode =
        static_cast<Texture2D::FilterMode>(tex2dJSON["filterMode"].GetUint());
    pImpl->path2assert.emplace_hint(target, path, Impl::Asset{tex2d});
    pImpl->asset2path.emplace(tex2d, path);
    return tex2d;
  } else {
    assert(false);
    return nullptr;
  }
}

void* AssetMngr::LoadAsset(const std::filesystem::path& path,
                           const std::type_info& typeinfo) {
  ImportAsset(path);
  if (path.extension() == ".lua") {
    if (typeinfo != typeid(LuaScript)) return nullptr;

    return LoadAsset(path);
  } else if (path.extension() == ".obj") {
    if (typeinfo != typeid(Mesh)) return nullptr;

    return LoadAsset(path);
  } else if (path.extension() == ".hlsl") {
    if (typeinfo != typeid(HLSLFile)) return nullptr;
    return LoadAsset(path);
  } else if (path.extension() == ".shader") {
    if (typeinfo != typeid(Shader)) return nullptr;
    return LoadAsset(path);
  } else if (path.extension() == ".png" || path.extension() == ".jpg" ||
             path.extension() == ".bmp" || path.extension() == ".hdr" ||
             path.extension() == ".tga") {
    if (typeinfo != typeid(Image)) return nullptr;
    return LoadAsset(path);
  } else if (path.extension() == ".tex2d") {
    if (typeinfo != typeid(Texture2D)) return nullptr;
    return LoadAsset(path);
  } else {
    assert(false);
    return nullptr;
  }
}

void AssetMngr::CreateAsset(const void* ptr,
                            const std::filesystem::path& path) {
  if (path.extension() == ".shader") {
    auto shader = reinterpret_cast<const Shader*>(ptr);
    auto guid = AssetPathToGUID(GetAssetPath(shader->hlslFile));
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("hlslFile");
    writer.String(guid.str());
    writer.Key("shaderName");
    writer.String(shader->shaderName);
    writer.Key("vertexName");
    writer.String(shader->vertexName);
    writer.Key("fragmentName");
    writer.String(shader->fragmentName);
    writer.Key("targetName");
    writer.String(shader->targetName);
    writer.EndObject();

    auto dirPath = path.parent_path();
    if (!std::filesystem::is_directory(dirPath))
      std::filesystem::create_directories(dirPath);

    std::ofstream ofs(path);
    assert(ofs.is_open());
    ofs << sb.GetString();
    ofs.close();

    ImportAsset(path);
  } else if (path.extension() == ".tex2d") {
    auto tex2d = reinterpret_cast<const Texture2D*>(ptr);
    auto guid = AssetPathToGUID(GetAssetPath(tex2d->image));
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("image");
    writer.String(guid.str());
    writer.Key("wrapMode");
    writer.Uint(static_cast<unsigned int>(tex2d->wrapMode));
    writer.Key("filterMode");
    writer.Uint(static_cast<unsigned int>(tex2d->filterMode));
    writer.EndObject();

    auto dirPath = path.parent_path();
    if (!std::filesystem::is_directory(dirPath))
      std::filesystem::create_directories(dirPath);

    std::ofstream ofs(path);
    assert(ofs.is_open());
    ofs << sb.GetString();
    ofs.close();

    ImportAsset(path);
  } else
    assert("not support" && false);
}

// ========================

std::string AssetMngr::Impl::LoadText(const std::filesystem::path& path) {
  std::ifstream t(path);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign(std::istreambuf_iterator<char>(t),
             std::istreambuf_iterator<char>());

  return str;
}

rapidjson::Document AssetMngr::Impl::LoadJSON(
    const std::filesystem::path& metapath) {
  // load guid
  auto str = LoadText(metapath);
  rapidjson::Document doc;
  doc.Parse(str.c_str(), str.size());
  return doc;
}

Mesh* AssetMngr::Impl::LoadObj(const std::filesystem::path& path) {
  tinyobj::ObjReader reader;

  bool success = reader.ParseFromFile(path.string());

  if (!reader.Warning().empty()) std::cout << reader.Warning() << std::endl;

  if (!reader.Error().empty()) std::cerr << reader.Error() << std::endl;

  if (!success) return nullptr;

  const auto& attrib = reader.GetAttrib();
  const auto& shapes = reader.GetShapes();
  const auto& materials = reader.GetMaterials();

  std::vector<pointf3> positions;
  std::vector<rgbf> colors;
  std::vector<normalf> normals;
  std::vector<vecf3> tangents;
  std::vector<uint32_t> indices;
  std::vector<pointf2> uv;
  std::vector<SubMeshDescriptor> submeshes;
  std::map<valu3, size_t> vertexIndexMap;

  positions.reserve(attrib.vertices.size() / 3);
  for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
    auto x = attrib.vertices[i + 0];
    auto y = attrib.vertices[i + 1];
    auto z = attrib.vertices[i + 2];
    positions.emplace_back(x, y, z);
  }

  normals.reserve(attrib.normals.size() / 3);
  for (size_t i = 0; i < attrib.normals.size(); i += 3) {
    auto x = attrib.normals[i + 0];
    auto y = attrib.normals[i + 1];
    auto z = attrib.normals[i + 2];
    normals.emplace_back(x, y, z);
  }

  uv.reserve(attrib.texcoords.size() / 2);
  for (size_t i = 0; i < attrib.texcoords.size(); i += 2) {
    auto x = attrib.texcoords[i + 0];
    auto y = attrib.texcoords[i + 1];
    uv.emplace_back(x, y);
  }

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    submeshes.emplace_back(indices.size(),
                           shapes[s].mesh.num_face_vertices.size() * 3);
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      auto fv = shapes[s].mesh.num_face_vertices[f];
      if (fv != 3) return nullptr;  // only support triangle mesh

      valu3 face;
      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

        valu3 key_idx = {idx.vertex_index, idx.normal_index,
                         idx.texcoord_index};
        auto target = vertexIndexMap.find(key_idx);
        if (target != vertexIndexMap.end()) {
          face[v] = static_cast<unsigned>(target->second);
          continue;
        }

        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
        tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
        tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
        positions.emplace_back(vx, vy, vz);
        if (idx.normal_index != -1) {
          tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
          tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
          tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
          normals.emplace_back(nx, ny, nz);
        }
        if (idx.texcoord_index != -1) {
          tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
          tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
          uv.emplace_back(tx, ty);
        }
        // Optional: vertex colors
        tinyobj::real_t red = attrib.colors[3 * idx.vertex_index + 0];
        tinyobj::real_t green = attrib.colors[3 * idx.vertex_index + 1];
        tinyobj::real_t blue = attrib.colors[3 * idx.vertex_index + 2];
        colors.emplace_back(red, green, blue);

        face[v] = static_cast<unsigned>(positions.size() - 1);
        vertexIndexMap[key_idx] = positions.size() - 1;
      }

      index_offset += fv;
      indices.push_back(face[0]);
      indices.push_back(face[1]);
      indices.push_back(face[2]);

      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }

  auto mesh = new Mesh;
  mesh->SetPositions(std::move(positions));
  mesh->SetColors(std::move(colors));
  mesh->SetNormals(std::move(normals));
  mesh->SetUV(std::move(uv));
  mesh->SetIndices(std::move(indices));
  mesh->SetSubMeshCount(submeshes.size());
  for (size_t i = 0; i < submeshes.size(); i++)
    mesh->SetSubMesh(i, submeshes[i]);

  return mesh;
}
