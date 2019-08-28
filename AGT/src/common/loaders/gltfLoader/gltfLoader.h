#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "managers/materialManager.h"
#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "model.h"
#include "tiny_gltf.h"
#include "vertexBuffer.h"

/*	*****WARNING:*******
 *	This loader is a prototype and should be used with care in production
 */

class GeometryBuffer_base {
public:
  GeometryBuffer_base(const GeometryBuffer_base &) = delete;
  GeometryBuffer_base &operator=(const GeometryBuffer_base &) = delete;
  GeometryBuffer_base(GeometryBuffer_base &&) = default;
  GeometryBuffer_base &operator=(GeometryBuffer_base &&) = default;
  GeometryBuffer_base() = default;
  virtual ~GeometryBuffer_base() = default;

  virtual unsigned char *dataAddress() = 0;
  virtual size_t dataSize() const = 0;
  virtual size_t elementSize() const = 0;
};

template <typename T> class GeometryBuffer : public GeometryBuffer_base {
public:
  GeometryBuffer(const GeometryBuffer &) = delete;
  GeometryBuffer operator=(const GeometryBuffer &) = delete;

  GeometryBuffer(GeometryBuffer &&other) noexcept
      : buffer{other.buffer}, bufferSize{other.bufferSize} {
    other.buffer = nullptr;
  }

  GeometryBuffer &operator=(GeometryBuffer &&other) noexcept {
    buffer = other.buffer;
    bufferSize = other.bufferSize;
    other.buffer = nullptr;
    return *this;
  }

  unsigned char *dataAddress() final {
    return reinterpret_cast<unsigned char *>(data());
  }

  size_t dataSize() const final { return size(); }
  size_t elementSize() const final { return sizeof(T); }
  GeometryBuffer(size_t size)
      : buffer{allocateSimdBuffer(size)}, bufferSize{size} {}
  ~GeometryBuffer() { freeSimdBuffer(buffer); }
  T *data() { return buffer; }
  size_t size() const { return bufferSize; }

private:
  static T *allocateSimdBuffer(size_t size) {
    return reinterpret_cast<T *>(malloc(sizeof(T) * size));
  }
  static void freeSimdBuffer(T *buffer) { free(buffer); }

  T *buffer;
  const size_t bufferSize;
};

struct VertexBufferPart {
  size_t getPartStride() const;

  std::unique_ptr<GeometryBuffer_base> buffer;
  ElementType type;
  ElementSemantic semantic;
  size_t vertexCount;
  size_t perVertex;
};

class GltfLoader {
public:
  GltfLoader(std::shared_ptr<TextureManager> textureManager,
             std::shared_ptr<MaterialManager> materialManager,
             std::shared_ptr<ShaderManager> shaderManager)
      : textureManager(textureManager), materialManager(materialManager),
        shaderManager(shaderManager){};
  Model load(const char *path);

private:
  VertexBuffer
  constructVertexBuffer(const std::vector<VertexBufferPart> &parts);
  void loadMeshes(tinygltf::Model &model, Model &finalModel);

  void iterateNodeHierarchyRecursive(tinygltf::Model &model, int nodeIndex,
                                     Model &finalModel);
  VertexBuffer extractIndexBuffer(tinygltf::Model &model, int acessorID);
  size_t getVertexBufferElementsPerVertex(int type) noexcept;
  ElementSemantic getElementSemantic(const std::string &gltfSemantic);

  VertexBufferPart
  extractVertexBuffer(const tinygltf::Model &model,
                      const std::pair<std::string, int> &attribute);

  void loadTextures(tinygltf::Model &model);
  void loadMaterials(tinygltf::Model &model);

  std::unordered_map<int, std::shared_ptr<Texture>> loadedTextures;
  std::unordered_map<int, std::shared_ptr<PBRMaterial>> loadedMaterials;
  std::shared_ptr<TextureManager> textureManager;
  std::shared_ptr<MaterialManager> materialManager;
  std::shared_ptr<ShaderManager> shaderManager;
};

#endif // GLTF_LOADER_H