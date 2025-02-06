#pragma once

#include "../info/info.h"
#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace pipeline {

struct Pipeline
{
  VkPipeline pipeline;
  VkPipelineLayout layout;
};
class Builder
{
private:
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
  VkPipelineInputAssemblyStateCreateInfo input_assembly_;
  VkPipelineRasterizationStateCreateInfo rasterizer_;
  VkPipelineColorBlendAttachmentState color_blend_att_;
  VkPipelineMultisampleStateCreateInfo multisampling_;
  VkPipelineLayout layout_;
  VkPipelineDepthStencilStateCreateInfo depth_stencil_;
  VkPipelineRenderingCreateInfo rendering_;
  VkFormat color_att_format_;

public:
  Builder();

  void clear();
  void set_layout(VkPipelineLayout layout);
  void set_shaders(VkShaderModule vs, VkShaderModule fs);
  void set_input_topology(VkPrimitiveTopology topology);
  void set_polygon_mode(VkPolygonMode mode);
  void set_cull_mode(VkCullModeFlags flags, VkFrontFace front);
  void set_multisampling_none();
  void set_color_attachment_format(VkFormat format);
  void set_depth_format(VkFormat format);
  void enable_depthtest(bool enable, VkCompareOp op);
  void disable_depthtest();
  void enable_blending_additive();
  void enable_blending_alphablend();
  void disable_blending();
  core::Result<VkPipeline, core::Status> build_pipeline(VkDevice device);
};

core::Result<VkShaderModule, core::Status>
load_shader_module(const char* file_path, VkDevice device);
} // namespace pipeline

} // namespace vulkan
} // namespace mgmt
