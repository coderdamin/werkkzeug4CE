/*+**************************************************************************/
/***                                                                      ***/
/***   This file is distributed under a BSD license.                      ***/
/***   See LICENSE.txt for details.                                       ***/
/***                                                                      ***/
/**************************************************************************+*/

#ifndef FILE_WZ4FRLIB_PHYSX_HPP
#define FILE_WZ4FRLIB_PHYSX_HPP

#include "wz4frlib/wz4_demo2_ops.hpp"
#include "wz4frlib/wz4_physx_ops.hpp"

/****************************************************************************/
/****************************************************************************/

//#define COMPIL_WITH_PVD

#ifdef _DEBUG
#undef _DEBUG
#define _DEBUG_WAS_DEFINED
#endif

#undef new
#include "C:/library/PhysX-3.2.3_PC_SDK_Core/Include/PxPhysicsAPI.h"
#define new sDEFINE_NEW

#ifdef _DEBUG_WAS_DEFINED
#undef _DEBUG_WAS_DEFINED
#define _DEBUG
#endif

#ifdef _M_X64
// 64 bits
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win64/PhysX3CHECKED_x64.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win64/PhysX3CommonCHECKED_x64.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win64/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win64/PhysX3CookingCHECKED_x64.lib")
#ifdef COMPIL_WITH_PVD
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win64/PhysXVisualDebuggerSDKCHECKED.lib")
#endif
#else
// 32 bits
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win32/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win32/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win32/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win32/PhysX3CookingCHECKED_x86.lib")
#ifdef COMPIL_WITH_PVD
#pragma comment(lib, "C:/library/PhysX-3.2.3_PC_SDK_Core/Lib/win32/PhysXVisualDebuggerSDKCHECKED.lib")
#endif
#endif

#ifdef _DEBUG
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
#endif

using namespace physx;

/****************************************************************************/
/****************************************************************************/

void PhysXInitEngine();

/****************************************************************************/
/****************************************************************************/

struct sActor
{
  PxRigidActor * actor;
  sMatrix34 * matrix;
};

/****************************************************************************/
/****************************************************************************/

// A template tree scene for physx object
// Transform and Render objects in a graph

template <typename  T, class T2>
class WpxGenericGraph : public T2
{
public:
  sArray<sMatrix34CM> Matrices;       // matrices list
  sArray<T *> Childs;                 // childs objects

  ~WpxGenericGraph();

  virtual void Render(Wz4RenderContext &ctx, sMatrix34 &mat);     // render
  virtual void Transform(const sMatrix34 & mat, void * ptr);      // build list of model matrices with GRAPH!
  virtual void ClearMatricesR();                                  // clear matrices
  virtual void PhysxReset();                                      // clear physx

  void RenderChilds(Wz4RenderContext &ctx, sMatrix34 &mat);       // recurse to childs
  void TransformChilds(const sMatrix34 & mat, void * ptr);        // recurse to childs
  void PhysxResetChilds();                                        // recurse to childs
};

template <typename  T, class T2>
WpxGenericGraph<T, T2>::~WpxGenericGraph()
{
  sReleaseAll(Childs);
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::ClearMatricesR()
{
  T *c;
  Matrices.Clear();
  sFORALL(Childs, c)
    c->ClearMatricesR();
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::Transform(const sMatrix34 &mat, void * ptr)
{
  TransformChilds(mat, ptr);
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::TransformChilds(const sMatrix34 &mat, void * ptr)
{
  T *c;

  Matrices.AddTail(sMatrix34CM(mat));

  sFORALL(Childs, c)
    c->Transform(mat, ptr);
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::Render(Wz4RenderContext &ctx, sMatrix34 &mat)
{
  RenderChilds(ctx,mat);
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::RenderChilds(Wz4RenderContext &ctx, sMatrix34 &mat)
{
  // recurse to childs
  T *c;
  sFORALL(Childs, c)
    c->Render(ctx,mat);
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::PhysxReset()
{
  PhysxResetChilds();
}

template <typename  T, class T2>
void WpxGenericGraph<T, T2>::PhysxResetChilds()
{
  T * c;
  sFORALL(Childs, c)
    c->PhysxReset();
}

/****************************************************************************/
/****************************************************************************/

// WpxColliderBase is the base type for all colliders operators
// WpxColliderBase inherited classes are used to preview a graph of colliders

class WpxColliderBase : public WpxGenericGraph<WpxColliderBase, wObject>
{
public:
  WpxColliderBase();
  void AddCollidersChilds(wCommand *cmd);    // add childs
};

/****************************************************************************/
/****************************************************************************/

class WpxCollider : public WpxColliderBase
{
private:
  Wz4Mesh * MeshCollider;         // collider mesh, used to preview collider shape
  Wz4Mesh * MeshInput;            // ptr to optional mesh used to generate collider geometry (when GeometryType is hull or mesh)
  PxConvexMesh * ConvexMesh;      // convex mesh (when GeometryType is hull)
  PxTriangleMesh * TriMesh;       // triangle mesh (when GeometryType is mesh)

public:
  WpxColliderParaCollider ParaBase, Para;

  WpxCollider();
  ~WpxCollider();
  void Transform(const sMatrix34 & mat, void * ptr);
  void Render(Wz4RenderContext &ctx, sMatrix34 &mat);

  void CreateGeometry(Wz4Mesh * input);                               // create collider mesh (to preview collider shape)
  void CreatePhysxCollider(PxRigidActor * actor, sMatrix34 & mat);    // create physx collider for actor
};

/****************************************************************************/

class WpxColliderAdd : public WpxColliderBase
{
public:
  WpxColliderAddParaColliderAdd ParaBase, Para;
};

/****************************************************************************/

class WpxColliderTransform : public WpxColliderBase
{
public:
  WpxColliderTransformParaColliderTransform ParaBase, Para;
  void Transform(const sMatrix34 & mat, void * ptr);
};

/****************************************************************************/

class WpxColliderMul : public WpxColliderBase
{
public:
  WpxColliderMulParaColliderMul ParaBase, Para;
  void Transform(const sMatrix34 & mat, void * ptr);
};

/****************************************************************************/
/****************************************************************************/

// WpxActorBase is the base type for all actors operators
// WpxActorBase inherited classes are used to preview a graph of actors + associated colliders graph

class WpxActorBase : public WpxGenericGraph<WpxActorBase, Wz4Render>
{
public:
  WpxActorBase();
  void AddActorsChilds(wCommand *cmd);    // add childs
};

/****************************************************************************/
/****************************************************************************/

class WpxRigidBody : public WpxActorBase
{
private:
  void PhysxBuildActor(const sMatrix34 & mat, PxScene * scene);   // build physx actor

public:
  WpxColliderBase * RootCollider;     // associated colliders geometries, root collider in collider graph
  sArray<sActor*> AllActors;          // list of actors

  WpxRigidBodyParaRigidBody ParaBase, Para;

  WpxRigidBody();
  ~WpxRigidBody();
  void Transform(const sMatrix34 & mat, void * ptr);
  void Render(Wz4RenderContext &ctx, sMatrix34 &mat);
  void ClearMatricesR();
  void PhysxReset();
  void AddRootCollider(WpxColliderBase * col);
};

/****************************************************************************/

class WpxRigidBodyAdd : public WpxActorBase
{
public:
  WpxRigidBodyAddParaRigidBodyAdd ParaBase, Para;
};

/****************************************************************************/

class WpxRigidBodyTransform : public WpxActorBase
{
public:
  WpxRigidBodyTransformParaRigidBodyTransform ParaBase, Para;
  void Transform(const sMatrix34 & mat, void * ptr);
};

/****************************************************************************/

class WpxRigidBodyMul : public WpxActorBase
{
public:
  WpxRigidBodyMulParaRigidBodyMul ParaBase, Para;
  void Transform(const sMatrix34 & mat, void * ptr);
};

/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
// next Wz4RenderNodes, are nodes associated with actors operators,
// they are computed in the Wz4Render graph process at each render loop,
// they are used for :
// - rendering RenderNode binded with physx
// - simulate and process manually physx features like kinematics, add forces, etc...
/****************************************************************************/

class WpxRigidBodyNode : public  Wz4RenderNode
{
};

class WpxRigidBodyNodeDynamic : public WpxRigidBodyNode
{
public:
  sArray<sActor*> * AllActorsPtr;   // ptr to an actors list (in WpxRigidBody)

  WpxRigidBodyParaRigidBody ParaBase, Para;

  WpxRigidBodyNodeDynamic();
  ~WpxRigidBodyNodeDynamic();
  void Transform(Wz4RenderContext *ctx, const sMatrix34 & mat);
};

class WpxRigidBodyNodeDynamicTransform : public WpxRigidBodyNode
{
public:
  WpxRigidBodyTransformParaRigidBodyTransform ParaBase, Para;
};

class WpxRigidBodyNodeDynamicMul : public WpxRigidBodyNode
{
public:
  WpxRigidBodyMulParaRigidBodyMul ParaBase, Para;
};

/****************************************************************************/
/****************************************************************************/

class RNPhysx : public Wz4RenderNode
{
private:
  PxScene * Scene;                        // Physx scene
  sBool Executed;                         // flag for restart and pause simulation mechanism with F6/F5
  sF32 PreviousTimeLine;                  // delta time line use to restart simulation

  PxScene * CreateScene();                // create new physx scene

public:
  Wz4RenderParaPhysx ParaBase, Para;
  Wz4RenderAnimPhysx Anim;

  RNPhysx::RNPhysx();
  RNPhysx::~RNPhysx();
  void Simulate(Wz4RenderContext *ctx);

  sBool Init(wCommand *cmd);
};

#endif FILE_WZ4FRLIB_PHYSX_HPP
