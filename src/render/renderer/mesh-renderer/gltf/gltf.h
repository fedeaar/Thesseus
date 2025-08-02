#pragma once

#include "../asset/asset.h"
#include "../material/material.h"
#include "../mesh/mesh.h"
#include "../renderer.h"

namespace render {
namespace gltf {

core::code
load_gltf(char* ip_path,
          mgmt::vulkan::Manager& ir_vkMgr,
          render::asset::DefaultResources& ir_defaultRes,
          mgmt::vulkan::descriptor::Writer& ir_writer,
          render::asset::material::MaterialPipelines& ir_materialPipes,
          std::shared_ptr<render::asset::LoadedGLTF>& ir_scene);

} // gltf
} // render
