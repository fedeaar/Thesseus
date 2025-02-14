#include "io.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

core::code
core::io::gltf::load(std::filesystem::path path, fastgltf::Asset* asset_ptr)
{
  if (!std::filesystem::exists(path)) {
    Logger::err("core::io::gltf::load",
                "gltf load error: failed to find {}",
                path.c_str());
    return core::code::ERROR;
  }
  static constexpr auto ext = fastgltf::Extensions::KHR_mesh_quantization |
                              fastgltf::Extensions::KHR_texture_transform |
                              fastgltf::Extensions::KHR_materials_variants;
  fastgltf::Parser parser(ext);
  constexpr auto options = fastgltf::Options::DontRequireValidAssetMember |
                           fastgltf::Options::AllowDouble |
                           fastgltf::Options::LoadGLBBuffers |
                           fastgltf::Options::LoadExternalBuffers |
                           fastgltf::Options::LoadExternalImages |
                           fastgltf::Options::GenerateMeshIndices;
  auto file = fastgltf::MappedGltfFile::FromPath(path);
  if (!bool(file)) {
    Logger::err("core::io::gltf::load",
                "failed to open glTF file: {}",
                fastgltf::getErrorMessage(file.error()));
    return core::code::ERROR;
  }
  auto asset = parser.loadGltf(file.get(), path.parent_path(), options);
  if (asset.error() != fastgltf::Error::None) {
    Logger::err("core::io::gltf::load",
                "Failed to load glTF: {}",
                fastgltf::getErrorMessage(asset.error()));
    return core::code::ERROR;
  }
  *asset_ptr = std::move(asset.get());
  return core::code::SUCCESS;
}
