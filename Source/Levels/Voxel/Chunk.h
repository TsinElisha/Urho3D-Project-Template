#pragma once
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/WorkQueue.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Model.h>
#include "VoxelDefs.h"
#include <queue>

const int SIZE_X = 16;
const int SIZE_Y = 16;
const int SIZE_Z = 16;
const int PART_COUNT = 3;

using namespace Urho3D;

struct ChunkVertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uvCoords;
    Vector4 tangent;
    unsigned char light;
};

class Chunk : public Object {
    URHO3D_OBJECT(Chunk, Object);
    Chunk(Context* context);
    virtual ~Chunk();

    static void RegisterObject(Context* context);
public:
    void Init(Scene* scene, const Vector3& position);
    void Load();
    const Vector3& GetPosition();
    Node* GetNode() { return node_; }
    friend void SaveToFile(const WorkItem* item, unsigned threadIndex);
    void Save();
    void MarkForDeletion(bool value);
    bool IsMarkedForDeletion();
    void MarkActive(bool value);
    bool IsActive();
    void SetActive();
    BlockType GetBlockValue(int x, int y, int z);
    bool Render();
    VoxelBlock* GetBlockAt(IntVector3 position);
    int GetSunlight(int x, int y, int z);
    void SetSunlight(int x, int y, int z, int value);
    int GetTorchlight(int x, int y, int z);
    void SetTorchlight(int x, int y, int z, int value = 15);
    unsigned char GetLightValue(int x, int y, int z);
    void SetSunlight(int value);
    bool ShouldRender() { return shouldRender_; }
    bool IsLoaded() { return loaded_; }
    bool IsGeometryCalculated() { return geometryCalculated_; }
    void CalculateLight();
    void CalculateGeometry();
    void MarkForGeometryCalculation();
    Chunk* GetNeighbor(BlockSide side);
    void SetVoxel(int x, int y, int z, BlockType block);
    static int sunlightLevel;
    BlockSide GetNeighborDirection(const IntVector3& position);
    IntVector3 GetNeighborBlockPosition(const IntVector3& position);

private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleWorkItemFinished(StringHash eventType, VariantMap& eventData);
    void HandleHit(StringHash eventType, VariantMap& eventData);
    void HandleAdd(StringHash eventType, VariantMap& eventData);
    Vector2 GetTextureCoord(BlockSide side, BlockType blockType, Vector2 position);
    IntVector3 GetChunkBlock(Vector3 position);
    bool IsBlockInsideChunk(IntVector3 position);
    void CreateNode();
    void RemoveNode();
    bool BlockHaveNeighbor(BlockSide side, int x, int y, int z);
    BlockType GetBlockNeighbor(BlockSide side, int x, int y, int z);
    unsigned char NeighborLightValue(BlockSide side, int x, int y, int z);
    int GetPartIndex(int x, int y, int z);

    Vector<SharedPtr<Node>> parts_;
    SharedPtr<Node> node_;
    SharedPtr<Node> label_;
    Scene* scene_;
    Vector3 position_;
    SharedPtr<WorkItem> saveWorkItem_;
    VoxelBlock data_[SIZE_X][SIZE_Y][SIZE_Z];
    unsigned char lightMap_[SIZE_X][SIZE_Y][SIZE_Z];
    bool shouldDelete_{false};
    bool isActive_{true};
    Mutex mutex_;

    bool loaded_{false};
    bool geometryCalculated_{false};
    bool shouldRender_{false};
    bool notified_{false};
    int renderIndex_{0};
    Timer saveTimer_;
    int renderCounter_{0};
};
