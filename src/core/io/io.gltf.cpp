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
  fastgltf::GltfDataBuffer data;
  data.FromPath(path);
  auto type = fastgltf::determineGltfFileType(data);
  if (type == fastgltf::GltfType::glTF) {
    auto asset = parser.loadGltf(data, path.parent_path(), options);
    if (asset.error() != fastgltf::Error::None) {
      Logger::err("core::io::gltf::load",
                  "Failed to load glTF: {}",
                  fastgltf::getErrorMessage(asset.error()));
      return core::code::ERROR;
    }
    *asset_ptr = std::move(asset.get());
    return core::code::SUCCESS;
  }
  if (type == fastgltf::GltfType::GLB) {
    auto asset = parser.loadGltfBinary(data, path.parent_path(), options);
    if (asset.error() != fastgltf::Error::None) {
      Logger::err("core::io::gltf::load",
                  "Failed to load glTF: {}",
                  fastgltf::getErrorMessage(asset.error()));
      return core::code::ERROR;
    }
    *asset_ptr = std::move(asset.get());
    return core::code::SUCCESS;
  }
  Logger::err("core::io::gltf::load", "failed to determine glTF container");
  return core::code::ERROR;
}

//
// vulkan
//

VkFilter
core::io::gltf::to_vulkan_filter(fastgltf::Filter filter)
{
  switch (filter) {
    // nearest
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
      return VK_FILTER_NEAREST;
    // linear
    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
      return VK_FILTER_LINEAR;
  }
}

VkSamplerMipmapMode
core::io::gltf::to_vulkan_mipmap_mode(fastgltf::Filter filter)
{
  switch (filter) {
    // nearest
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
      return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    // linear
    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
      return VK_SAMPLER_MIPMAP_MODE_LINEAR;
  }
}
