#include "pipeline.h"

#include <fstream>

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

core::Result<VkShaderModule, Status>
RenderEngine::VulkanManager::Pipeline::load_shader_module(const char* file_path,
                                                          VkDevice device)
{
  std::ifstream file(file_path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    logger.error(fmt::format(
      "load_shader_module failed, could not open file {}", file_path));
    return Status::ERROR;
  }
  size_t size = (size_t)file.tellg();
  std::vector<u32> buffer(size / sizeof(u32));
  file.seekg(0);
  file.read((char*)buffer.data(), size);
  file.close();
  VkShaderModuleCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.codeSize = buffer.size() * sizeof(u32);
  info.pCode = buffer.data();
  VkShaderModule shader_module;
  auto status =
    check(vkCreateShaderModule(device, &info, nullptr, &shader_module));
  if (status != Status::SUCCESS) {
    logger.error("load_shader_module failed, vkCreateShaderModule error");
    return status;
  }
  return shader_module;
}
