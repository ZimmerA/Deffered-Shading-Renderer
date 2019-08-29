#include "gltfLoader.h"

template <typename bufferType, typename sourceType>
void loadIndexBuffer(bufferType *dest, sourceType *source, size_t indexCount,
                     size_t offset, size_t stride) {
  for (size_t i = 0; i < indexCount; ++i) {
    dest[i] = *(reinterpret_cast<sourceType *>(
        reinterpret_cast<unsigned char *>(source) + (offset + i * stride)));
  }
}

size_t VertexBufferPart::getPartStride() const {
  return buffer->elementSize() * perVertex;
}

Model GltfLoader::load(const char *path) {

  tinygltf::Model model;
  tinygltf::TinyGLTF loader;

  std::string err;
  std::string warn;
  // Todo: make these 2 local instead of member
  loadedTextures.clear();
  loadedMaterials.clear();

  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

  Model finalModel;

  if (!warn.empty()) {
    printf("Warn: %s\n", warn.c_str());
  }

  if (!err.empty()) {
    printf("Err: %s\n", err.c_str());
    // TODO: Throw Exception
  }

  if (!ret) {
    printf("Failed to parse glTF\n");
    // TODO: Throw Exception
  }

  tinygltf::Scene *defaultScene = &model.scenes.at(0);
  loadTextures(model);
  loadMaterials(model);
  loadMeshes(model, finalModel);

  /*
  // TODO: load transform nodes
  for (auto &nodeIndex : defaultScene->nodes) {
    iterateNodeHierarchyRecursive(model, nodeIndex, finalModel);
  }
  */

  return finalModel;
}

VertexBuffer
GltfLoader::constructVertexBuffer(const std::vector<VertexBufferPart> &parts) {
  size_t stride{0}, strideInElements{0};
  size_t vertexCount{0};
  // Needed for VertexAttribpointers
  VertexElementDescriptorVec elementdescriptor;

  for (const auto &part : parts) {
    strideInElements += part.perVertex;
    stride += part.buffer->elementSize() * part.perVertex;
    vertexCount = part.vertexCount;
    elementdescriptor.push_back({part.semantic, part.type});
  }

  GeometryBuffer<float> finalBuffer(vertexCount * strideInElements);

  // Copy elements in final buffer
  for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
    size_t bytesWrittenInCurrentStride = 0;
    for (const auto &part : parts) {
      memcpy(
          finalBuffer.dataAddress() +
              (bytesWrittenInCurrentStride + vertexIndex * stride),
          (part.buffer->dataAddress() + (vertexIndex * part.getPartStride())),
          part.getPartStride());
      bytesWrittenInCurrentStride += part.getPartStride();
    }
  }

  VertexBuffer finalVbo(vertexCount, stride, elementdescriptor);
  finalVbo.buffer(GL_ARRAY_BUFFER, stride * vertexCount, finalBuffer.data(),
                  GL_STATIC_DRAW);
  return finalVbo;
}

void GltfLoader::loadMeshes(tinygltf::Model &model, Model &finalModel) {
  for (auto &mesh : model.meshes) {
    for (const auto &primitive : mesh.primitives) {
      // TODO: Not every mesh has an index buffer
      VertexBuffer ebo = extractIndexBuffer(model, primitive.indices);

      // Load attributes
      std::vector<VertexBufferPart> parts;

      for (const auto &attribute : primitive.attributes) {
        parts.push_back(std::move(extractVertexBuffer(model, attribute)));
      }

      // Stitch together submeshes into single vbo
      VertexBuffer vbo = constructVertexBuffer(parts);
      VertexArray vao(std::move(vbo), std::move(ebo), GL_TRIANGLES);
      glBindVertexArray(0);

      finalModel.meshes.push_back(
          std::move(Mesh(std::move(vao), loadedMaterials[primitive.material])));
    }
  }
}

void GltfLoader::iterateNodeHierarchyRecursive(tinygltf::Model &model,
                                               int nodeIndex,
                                               Model &finalModel) {
  const int meshIndex = model.nodes.at(nodeIndex).mesh;

  for (auto &childNodeIndex : model.nodes.at(nodeIndex).children) {
    iterateNodeHierarchyRecursive(model, childNodeIndex, finalModel);
  }
}

VertexBuffer GltfLoader::extractIndexBuffer(tinygltf::Model &model,
                                            int acessorID) {
  const auto &accessor = model.accessors.at(acessorID);
  const auto &bufferView = model.bufferViews.at(accessor.bufferView);
  auto &buffer = model.buffers.at(bufferView.buffer);

  const auto byteStride = accessor.ByteStride(bufferView);
  const auto indexCount = accessor.count;

  auto convertTo16Bit{false};
  VertexBuffer ebo(indexCount);

  switch (accessor.componentType) {
  case TINYGLTF_COMPONENT_TYPE_BYTE:
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    convertTo16Bit = true;
  case TINYGLTF_COMPONENT_TYPE_SHORT:
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
    auto geomBuffer = GeometryBuffer<uint16_t>(indexCount);
    if (convertTo16Bit)
      loadIndexBuffer(geomBuffer.data(), buffer.data.data(), indexCount,
                      bufferView.byteOffset + accessor.byteOffset, byteStride);
    else
      loadIndexBuffer(
          geomBuffer.data(), reinterpret_cast<uint16_t *>(buffer.data.data()),
          indexCount, bufferView.byteOffset + accessor.byteOffset, byteStride);
    ebo.buffer(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint16_t),
               geomBuffer.data(), GL_STATIC_DRAW);

  } break;
  case TINYGLTF_COMPONENT_TYPE_INT:;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
    auto geomBuffer = GeometryBuffer<uint32_t>(indexCount);
    loadIndexBuffer(
        geomBuffer.data(), reinterpret_cast<uint32_t *>(buffer.data.data()),
        indexCount, bufferView.byteOffset + accessor.byteOffset, byteStride);
    ebo.buffer(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t),
               geomBuffer.data(), GL_STATIC_DRAW);
  } break;
  }

  return ebo;
}

size_t GltfLoader::getVertexBufferElementsPerVertex(int type) noexcept {
  switch (type) {
  case TINYGLTF_TYPE_VEC2:
    return 2;
  case TINYGLTF_TYPE_VEC3:
    return 3;
  case TINYGLTF_TYPE_VEC4:
    return 4;
  default:
    return 0;
  }
}

ElementSemantic
GltfLoader::getElementSemantic(const std::string &gltfSemantic) {
  if (gltfSemantic == "POSITION")
    return POSITION;
  if (gltfSemantic == "NORMAL")
    return NORMAL;
  if (gltfSemantic == "TANGENT")
    return TANGENT;
  if (gltfSemantic == "TEXCOORD_0")
    return TEXCOORD;
  if (gltfSemantic == "TEXCOORD_1")
    return TEXCOORD;
  if (gltfSemantic == "COLOR_0")
    return DIFFUSE;
  return SEMANTIC_COUNT;
}

VertexBufferPart
GltfLoader::extractVertexBuffer(const tinygltf::Model &model,
                                const std::pair<std::string, int> &attribute) {
  const auto &accessor = model.accessors.at(attribute.second);
  const auto &bufferView = model.bufferViews.at(accessor.bufferView);
  const auto &buffer = model.buffers.at(bufferView.buffer);
  const auto vertexBufferByteLen = bufferView.byteLength;
  const auto elementOffsetInBuffer =
      bufferView.byteOffset + accessor.byteOffset;
  const auto numberOfElementsPerVertex =
      getVertexBufferElementsPerVertex(accessor.type);
  const auto elementSemantic = getElementSemantic(attribute.first);

  ElementType elementType{};
  assert(numberOfElementsPerVertex != 0);

  size_t bufferLenghtInBufferBasicType{0};
  std::unique_ptr<GeometryBuffer_base> geometryBuffer{nullptr};

  // Create geometry buffer
  switch (accessor.componentType) {
  case TINYGLTF_COMPONENT_TYPE_FLOAT:
    bufferLenghtInBufferBasicType = (vertexBufferByteLen / sizeof(float));
    geometryBuffer =
        std::make_unique<GeometryBuffer<float>>(bufferLenghtInBufferBasicType);
    if (numberOfElementsPerVertex == 2)
      elementType = ET_FLOAT2;
    if (numberOfElementsPerVertex == 3)
      elementType = ET_FLOAT3;
    if (numberOfElementsPerVertex == 4)
      elementType = ET_FLOAT4;
    break;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    bufferLenghtInBufferBasicType =
        (vertexBufferByteLen / sizeof(unsigned short));
    geometryBuffer = std::make_unique<GeometryBuffer<unsigned short>>(
        bufferLenghtInBufferBasicType);
    if (numberOfElementsPerVertex == 2)
      elementType = ET_USHORT2;
    if (numberOfElementsPerVertex == 4)
      elementType = ET_USHORT4;
    break;
  default:
    assert(false);
    break;
  }

  const auto byteStride = accessor.ByteStride(bufferView);
  const auto vertexCount = accessor.count;
  const auto vertexElementLengthInBytes =
      geometryBuffer->elementSize() * numberOfElementsPerVertex;

  for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
    const auto destOffset = vertexIndex * vertexElementLengthInBytes;
    const auto sourceOffset = elementOffsetInBuffer + vertexIndex * byteStride;
    memcpy(geometryBuffer->dataAddress() + destOffset,
           buffer.data.data() + sourceOffset, vertexElementLengthInBytes);
  }

  return {std::move(geometryBuffer), elementType, elementSemantic, vertexCount,
          numberOfElementsPerVertex};
}

void GltfLoader::loadTextures(tinygltf::Model &model) {
  for (const auto &texture : model.textures) {
    const auto &image = model.images.at(texture.source);
    // TODO: Better Texture naming system
    std::shared_ptr<Texture> loadedTex =
        textureManager->create("gltf_texture_" + std::to_string(texture.source),
                               const_cast<unsigned char *>(image.image.data()),
                               image.width, image.height, image.component);
    loadedTextures[texture.source] = loadedTex;
  }
}

void GltfLoader::loadMaterials(tinygltf::Model &model) {
  int i = 0;
  for (auto &material : model.materials) {
    // TODO: Better material naming system
    std::shared_ptr<PBRMaterial> mat =
        materialManager->createEmpty("sponza" + std::to_string(i));
    // TODO: find more elegent way to set shaders
	mat->shader = shaderManager->getProgramByName("gProgram");
    loadedMaterials[i++] = mat;

    for (const auto &content : material.values) {
      if (content.first == "baseColorTexture") {
        mat->textures[PBRMaterial::TextureType::ALBEDO] =
            loadedTextures[content.second.TextureIndex()];
      }

      if (content.first == "metallicRoughnessTexture") {
        mat->textures[PBRMaterial::TextureType::METALROUGHNESS] =
            loadedTextures[content.second.TextureIndex()];
      }

      if (content.first == "baseColorFactor") {
        mat->baseColorFactor = glm::vec3(content.second.ColorFactor().at(0),
                                         content.second.ColorFactor().at(1),
                                         content.second.ColorFactor().at(2));
      }

      if (content.first == "metallicFactor") {
        mat->metallicFactor = static_cast<float>(content.second.Factor());
      }

      if (content.first == "roughnessFactor") {
        mat->metallicFactor = static_cast<float>(content.second.Factor());
      }
    }

    for (const auto &content : material.additionalValues) {
      if (content.first == "normalTexture") {
        mat->textures[PBRMaterial::TextureType::NORMAL] =
            loadedTextures[content.second.TextureIndex()];
      }

      if (content.first == "occlusionTexture") {
        mat->textures[PBRMaterial::TextureType::OCCLUSION] =
            loadedTextures[content.second.TextureIndex()];
      }

      if (content.first == "emissiveTexture") {
        mat->textures[PBRMaterial::TextureType::EMISSIVE] =
            loadedTextures[content.second.TextureIndex()];
      }

      if (content.first == "emissiveFactor") {
        mat->emissiveFactor = glm::vec3(content.second.ColorFactor().at(0),
                                        content.second.ColorFactor().at(1),
                                        content.second.ColorFactor().at(2));
      }

      if (content.first == "alphaMode") {
        if (content.second.string_value == "MASK") {
			mat->shader = shaderManager->getProgramByName("gAlphaProgram");
          mat->mode = M_MASK;
        } else if (content.second.string_value == "BLEND") {
          mat->mode = M_BLEND;
        }
	  }
    }
  }
}
