#include "proctree.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

namespace Sibyl {

float length_n(Sybil::fvec3 a) {
  return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Sybil::fvec3 normalize(Sybil::fvec3 a) {
  float l = length_n(a);
  if (l != 0) {
    l = 1.0f / l;
    a.x *= l;
    a.y *= l;
    a.z *= l;
  }
  return a;
}

Sybil::fvec3 cross(Sybil::fvec3 a, Sybil::fvec3 b) {
  fvec3 c = {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
  return c;
}

float dot(Sybil::fvec3 a, Sybil::fvec3 b) {
  return a.x * b.x +
         a.y * b.y +
         a.z * b.z;
}

Sybil::fvec3 sub(Sybil::fvec3 a, Sybil::fvec3 b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  return a;
}

Sybil::fvec3 add(Sybil::fvec3 a, Sybil::fvec3 b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

Sybil::fvec3 scaleVec(Sybil::fvec3 a, float b) {
  a.x *= b;
  a.y *= b;
  a.z *= b;
  return a;
}

Sybil::fvec3 scaleInDirection(Sybil::fvec3 aVector, Sybil::fvec3 aDirection, float aScale) {
  float currentMag = dot(aVector, aDirection);

  fvec3 change = scaleVec(aDirection, currentMag * aScale - currentMag);
  return add(aVector, change);
}

Sybil::fvec3 vecAxisAngle(Sybil::fvec3 aVec, Sybil::fvec3 aAxis, float aAngle) {
  float cosr = std::cos(aAngle);
  float sinr = std::sin(aAngle);
  return add(add(scaleVec(aVec, cosr), scaleVec(cross(aAxis, aVec), sinr)), 
               scaleVec(aAxis, dot(aAxis, aVec) * (1 - cosr)));
}

fvec3 mirrorBranch(fvec3 aVec, fvec3 aNorm, Properties &aProperties) {
  fvec3 v = cross(aNorm, cross(aVec, aNorm));
  float s = aProperties.mBranchFactor * dot(v, aVec);
  fvec3 res = {
    aVec.x - v.x * s,
    aVec.y - v.y * s,
    aVec.z - v.z * s
  };
  return res;
}



Tree::Tree()
{
  mRoot = 0;
  mVert = 0;
  mNormal = 0;
  mUV = 0;
  mTwigVert = 0;
  mTwigNormal = 0;
  mTwigUV = 0;
  mFace = 0;
  mTwigFace = 0;

  mVertCount = 0;
  mTwigVertCount = 0;
  mFaceCount = 0;
  mTwigFaceCount = 0;
}

Tree::~Tree()
{
  delete[] mRoot;
  delete[] mVert;
  delete[] mNormal;
  delete[] mUV;
  delete[] mTwigVert;
  delete[] mTwigNormal;
  delete[] mTwigUV;
  delete[] mFace;
  delete[] mTwigFace;
}

void Tree::init()
{
  mVertCount = 0;
  mTwigVertCount = 0;
  mFaceCount = 0;
  mTwigFaceCount = 0;

  delete[] mRoot;
  delete[] mVert;
  delete[] mNormal;
  delete[] mUV;
  delete[] mTwigVert;
  delete[] mTwigNormal;
  delete[] mTwigUV;
  delete[] mFace;
  delete[] mTwigFace;

  mRoot = 0;
  mVert = 0;
  mNormal = 0;
  mUV = 0;
  mTwigVert = 0;
  mTwigNormal = 0;
  mTwigUV = 0;
  mFace = 0;
  mTwigFace = 0;
}

void Tree::allocVertBuffers()
{
  mVert = new fvec3[mVertCount];
  mNormal = new fvec3[mVertCount];
  mUV = new fvec2[mVertCount];
  mTwigVert = new fvec3[mTwigVertCount];
  mTwigNormal = new fvec3[mTwigVertCount];
  mTwigUV = new fvec2[mTwigVertCount];
  mTwigFace = new ivec3[mTwigFaceCount];

  // Reset back to zero, we'll use these as counters

  mVertCount = 0;
  mTwigVertCount = 0;
  mTwigFaceCount = 0;
}

void Tree::allocFaceBuffers()
{
  mFace = new ivec3[mFaceCount];

  // Reset back to zero, we'll use these as counters

  mFaceCount = 0;
}

void Tree::generate()
{
  init();
  mProperties.mRseed = mProperties.mSeed;
  fvec3 starthead = { 0, mProperties.mTrunkLength, 0 };
  mRoot = new Branch(starthead, 0);
  mRoot->mLength = mProperties.mInitialBranchLength;
  mRoot->split(mProperties.mLevels, mProperties.mTreeSteps, mProperties);

  calcVertSizes(0);
  allocVertBuffers();
  createForks(0, 0);
  createTwigs(0);
  calcFaceSizes(0);
  allocFaceBuffers();
  doFaces(0);
  calcNormals();
  fixUVs();
  delete mRoot;
  mRoot = 0;
}

void Tree::fixUVs()
{
  // There'll never be more than 50% bad vertices
  int *badverttable = new int[mVertCount / 2];
  int i;
  int badverts = 0;

  // step 1: find bad verts
  // - If edge's U coordinate delta is over 0.5, texture has wrapped around. 
  // - The vertex that has zero U is the wrong one
  // - Care needs to be taken not to tag bad vertex more than once.

  for (i = 0; i < mFaceCount; i++)
  {
    // x/y edges (vertex 0 and 1)
    if ((std::fabs(mUV[mFace[i].x].u - mUV[mFace[i].y].u) > 0.5f) && (mUV[mFace[i].x].u == 0 || mUV[mFace[i].y].u == 0))
    {
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].y && mUV[mFace[i].y].u == 0)
          found = 1;
        if (badverttable[j] == mFace[i].x && mUV[mFace[i].x].u == 0)
          found = 1;
      }
      if (!found)
      {
        if (mUV[mFace[i].x].u == 0)
          badverttable[badverts] = mFace[i].x;
        if (mUV[mFace[i].y].u == 0)
          badverttable[badverts] = mFace[i].y;
        badverts++;
      }
    }

    // x/z edges (vertex 0 and 2)
    if ((std::fabs(mUV[mFace[i].x].u - mUV[mFace[i].z].u) > 0.5f) && (mUV[mFace[i].x].u == 0 || mUV[mFace[i].z].u == 0))
    {
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].z && mUV[mFace[i].z].u == 0)
          found = 1;
        if (badverttable[j] == mFace[i].x && mUV[mFace[i].x].u == 0)
          found = 1;
      }
      if (!found)
      {
        if (mUV[mFace[i].x].u == 0)
          badverttable[badverts] = mFace[i].x;
        if (mUV[mFace[i].z].u == 0)
          badverttable[badverts] = mFace[i].z;
        badverts++;
      }
    }

    // y/z edges (vertex 1 and 2)
    if ((std::fabs(mUV[mFace[i].y].u - mUV[mFace[i].z].u) > 0.5f) && (mUV[mFace[i].y].u == 0 || mUV[mFace[i].z].u == 0))
    {
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].z && mUV[mFace[i].z].u == 0)
          found = 1;
        if (badverttable[j] == mFace[i].y && mUV[mFace[i].y].u == 0)
          found = 1;
      }
      if (!found)
      {
        if (mUV[mFace[i].y].u == 0)
          badverttable[badverts] = mFace[i].y;
        if (mUV[mFace[i].z].u == 0)
          badverttable[badverts] = mFace[i].z;
        badverts++;
      }
    }
  }
  
  // step 2: allocate more space for our new duplicate verts

  fvec3 *nvert = new fvec3[mVertCount + badverts];
  memcpy(nvert, mVert, sizeof(fvec3) * mVertCount);
  delete[] mVert;
  mVert = nvert;

  fvec3 *nnorm = new fvec3[mVertCount + badverts];
  memcpy(nnorm, mNormal, sizeof(fvec3) * mVertCount);
  delete[] mNormal;
  mNormal = nnorm;

  fvec2 *nuv = new fvec2[mVertCount + badverts];
  memcpy(nuv, mUV, sizeof(fvec2) * mVertCount);
  delete[] mUV;
  mUV = nuv;

  // step 3: populate duplicate verts - otherwise identical except for U=1 instead of 0
  
  for (i = 0; i < badverts; i++)
  {
    mVert[mVertCount + i] = mVert[badverttable[i]];
    mNormal[mVertCount + i] = mNormal[badverttable[i]];
    mUV[mVertCount + i] = mUV[badverttable[i]];
    mUV[mVertCount + i].u = 1.0f;
  }

  // step 4: fix faces
  
  for (i = 0; i < mFaceCount; i++)
  {
    // x/y edges (vertex 0 and 1)
    if ((std::fabs(mUV[mFace[i].x].u - mUV[mFace[i].y].u) > 0.5f) && (mUV[mFace[i].x].u == 0 || mUV[mFace[i].y].u == 0))
    {        
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].y && mUV[mFace[i].y].u == 0)
          found = j;
        if (badverttable[j] == mFace[i].x && mUV[mFace[i].x].u == 0)
          found = j;
      }
      if (mUV[mFace[i].y].u == 0)
        mFace[i].y = mVertCount + found;
      if (mUV[mFace[i].x].u == 0)
        mFace[i].x = mVertCount + found;
    }

    // x/z edges (vertex 0 and 2)
    if ((std::fabs(mUV[mFace[i].x].u - mUV[mFace[i].z].u) > 0.5f) && (mUV[mFace[i].x].u == 0 || mUV[mFace[i].z].u == 0))
    {
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].z && mUV[mFace[i].z].u == 0)
          found = j;
        if (badverttable[j] == mFace[i].x && mUV[mFace[i].x].u == 0)
          found = j;
      }
      if (mUV[mFace[i].x].u == 0)
        mFace[i].x = mVertCount + found;
      if (mUV[mFace[i].z].u == 0)
        mFace[i].z = mVertCount + found;
    }

    // y/z edges (vertex 1 and 2)
    if ((std::fabs(mUV[mFace[i].y].u - mUV[mFace[i].z].u) > 0.5f) && (mUV[mFace[i].y].u == 0 || mUV[mFace[i].z].u == 0))
    {
      int found = 0, j;
      for (j = 0; j < badverts; j++)
      {
        if (badverttable[j] == mFace[i].z && mUV[mFace[i].z].u == 0)
          found = j;
        if (badverttable[j] == mFace[i].y && mUV[mFace[i].y].u == 0)
          found = j;
      }
      if (mUV[mFace[i].y].u == 0)
        mFace[i].y = mVertCount + found;
      if (mUV[mFace[i].z].u == 0)
        mFace[i].z = mVertCount + found;        
    }
  }

  // step 5: update vert count
  mVertCount += badverts;

  // and cleanup
  delete[] badverttable;
}

void Tree::calcVertSizes(Branch *aBranch)
{
  int segments = mProperties.mSegments;
  if (!aBranch)
    aBranch = mRoot;

  if (!aBranch->mParent)
  {
    mVertCount += segments;
  }

  if (aBranch->mChild0)
  {
    mVertCount +=
      1 +
      (segments / 2) - 1 +
      1 +
      (segments / 2) - 1 +
      (segments / 2) - 1;

    calcVertSizes(aBranch->mChild0);
    calcVertSizes(aBranch->mChild1);
  }
  else
  {
    mVertCount++;
    mTwigVertCount += 8;
    mTwigFaceCount += 4;
  }
}

void Tree::calcFaceSizes(Branch *aBranch)
{
  int segments = mProperties.mSegments;
  if (!aBranch)
    aBranch = mRoot;

  if (!aBranch->mParent)
  {
    mFaceCount += segments * 2;
  }

  if (aBranch->mChild0->mRing0 != 0)
  {
    mFaceCount += segments * 4;

    calcFaceSizes(aBranch->mChild0);
    calcFaceSizes(aBranch->mChild1);
  }
  else
  {
    mFaceCount += segments * 2;
  }
}

void Tree::calcNormals()
{
  int *normalCount = new int[mVertCount];
  memset(normalCount, 0, sizeof(int) * mVertCount);
  memset(mNormal, 0, sizeof(fvec3) * mVertCount);

  int i;
  for (i = 0; i < (int)mFaceCount; i++)
  {
    normalCount[mFace[i].x]++;
    normalCount[mFace[i].y]++;
    normalCount[mFace[i].z]++;

    fvec3 norm = normalize(cross(sub(mVert[mFace[i].y], mVert[mFace[i].z]), sub(mVert[mFace[i].y], mVert[mFace[i].x])));

    mNormal[mFace[i].x].x += norm.x;
    mNormal[mFace[i].x].y += norm.y;
    mNormal[mFace[i].x].z += norm.z;
    mNormal[mFace[i].y].x += norm.x;
    mNormal[mFace[i].y].y += norm.y;
    mNormal[mFace[i].y].z += norm.z;
    mNormal[mFace[i].z].x += norm.x;
    mNormal[mFace[i].z].y += norm.y;
    mNormal[mFace[i].z].z += norm.z;
  }

  for (i = 0; i < (int)mVertCount; i++)
  {
    float d = 1.0f / normalCount[i];
    mNormal[i].x *= d;
    mNormal[i].y *= d;
    mNormal[i].z *= d;
  }

  delete[] normalCount;
}

void Tree::doFaces(Branch *aBranch)
{
  if (!aBranch)
  {
    aBranch = mRoot;
  }
  int segments = mProperties.mSegments;
  int i;
  if (!aBranch->mParent)
  {
    fvec3 tangent = normalize(cross(sub(aBranch->mChild0->mHead, aBranch->mHead), sub(aBranch->mChild1->mHead, aBranch->mHead)));
    fvec3 normal = normalize(aBranch->mHead);
    fvec3 left = { -1, 0, 0 };
    float angle = std::acos(dot(tangent, left));
    if (dot(cross(left, tangent), normal) > 0)
    {
      angle = 2 * M_PI - angle;
    }
    int segOffset = (int)floor(0.5f + (angle / M_PI / 2 * segments));
    for (i = 0; i < segments; i++)
    {
      int v1 = aBranch->mRing0[i];
      int v2 = aBranch->mRootRing[(i + segOffset + 1) % segments];
      int v3 = aBranch->mRootRing[(i + segOffset) % segments];
      int v4 = aBranch->mRing0[(i + 1) % segments];

      ivec3 a;
      a = { v1, v4, v3 };
      mFace[mFaceCount++] = (a);
      a = { v4, v2, v3 };
      mFace[mFaceCount++] = (a);

      mUV[(i + segOffset) % segments] = { i / (float)segments, 0 };

      float len = length_n(sub(mVert[aBranch->mRing0[i]], mVert[aBranch->mRootRing[(i + segOffset) % segments]])) * mProperties.mVMultiplier;
      mUV[aBranch->mRing0[i]] = { i / (float)segments, len };
      mUV[aBranch->mRing2[i]] = { i / (float)segments, len };
    }
  }

  if (aBranch->mChild0->mRing0 != 0)
  {
    int segOffset0 = -1, segOffset1 = -1;
    float match0, match1;

    fvec3 v1 = normalize(sub(mVert[aBranch->mRing1[0]], aBranch->mHead));
    fvec3 v2 = normalize(sub(mVert[aBranch->mRing2[0]], aBranch->mHead));

    v1 = scaleInDirection(v1, normalize(sub(aBranch->mChild0->mHead, aBranch->mHead)), 0);
    v2 = scaleInDirection(v2, normalize(sub(aBranch->mChild1->mHead, aBranch->mHead)), 0);

    for (i = 0; i < segments; i++)
    {
      fvec3 d = normalize(sub(mVert[aBranch->mChild0->mRing0[i]], aBranch->mChild0->mHead));
      float l = dot(d, v1);
      if (segOffset0 == -1 || l > match0)
      {
        match0 = l;
        segOffset0 = segments - i;
      }
      d = normalize(sub(mVert[aBranch->mChild1->mRing0[i]], aBranch->mChild1->mHead));
      l = dot(d, v2);
      if (segOffset1 == -1 || l > match1)
      {
        match1 = l;
        segOffset1 = segments - i;
      }
    }

    float UVScale = mProperties.mMaxRadius / aBranch->mRadius;

    for (i = 0; i < segments; i++)
    {
      int v1 = aBranch->mChild0->mRing0[i];
      int v2 = aBranch->mRing1[(i + segOffset0 + 1) % segments];
      int v3 = aBranch->mRing1[(i + segOffset0) % segments];
      int v4 = aBranch->mChild0->mRing0[(i + 1) % segments];
      ivec3 a;
      a = { v1, v4, v3 };
      mFace[mFaceCount++] = (a);
      a = { v4, v2, v3 };
      mFace[mFaceCount++] = (a);

      v1 = aBranch->mChild1->mRing0[i];
      v2 = aBranch->mRing2[(i + segOffset1 + 1) % segments];
      v3 = aBranch->mRing2[(i + segOffset1) % segments];
      v4 = aBranch->mChild1->mRing0[(i + 1) % segments];

      a = { v1, v2, v3 };
      mFace[mFaceCount++] = (a);
      a = { v1, v4, v2 };
      mFace[mFaceCount++] = (a);

      float len1 = length_n(sub(mVert[aBranch->mChild0->mRing0[i]], mVert[aBranch->mRing1[(i + segOffset0) % segments]])) * UVScale;
      fvec2 uv1 = mUV[aBranch->mRing1[(i + segOffset0 - 1) % segments]];

      mUV[aBranch->mChild0->mRing0[i]] = { uv1.u, uv1.v + len1 * mProperties.mVMultiplier };
      mUV[aBranch->mChild0->mRing2[i]] = { uv1.u, uv1.v + len1 * mProperties.mVMultiplier };

      float len2 = length_n(sub(mVert[aBranch->mChild1->mRing0[i]], mVert[aBranch->mRing2[(i + segOffset1) % segments]])) * UVScale;
      fvec2 uv2 = mUV[aBranch->mRing2[(i + segOffset1 - 1) % segments]];

      mUV[aBranch->mChild1->mRing0[i]] = { uv2.u, uv2.v + len2 * mProperties.mVMultiplier };
      mUV[aBranch->mChild1->mRing2[i]] = { uv2.u, uv2.v + len2 * mProperties.mVMultiplier };
    }

    doFaces(aBranch->mChild0);
    doFaces(aBranch->mChild1);
  }
  else
  {
    for (i = 0; i < segments; i++)
    {
      ivec3 a;
      a = {
        aBranch->mChild0->mEnd,
        aBranch->mRing1[(i + 1) % segments],
        aBranch->mRing1[i]
      };
      mFace[mFaceCount++] = (a);
      a = {
        aBranch->mChild1->mEnd,
        aBranch->mRing2[(i + 1) % segments],
        aBranch->mRing2[i]
      };
      mFace[mFaceCount++] = (a);

      float len = length_n(sub(mVert[aBranch->mChild0->mEnd], mVert[aBranch->mRing1[i]]));
      mUV[aBranch->mChild0->mEnd] = { i / (float)segments - 1, len * mProperties.mVMultiplier };
      len = length_n(sub(mVert[aBranch->mChild1->mEnd], mVert[aBranch->mRing2[i]]));
      mUV[aBranch->mChild1->mEnd] = { i / (float)segments, len * mProperties.mVMultiplier };
    }
  }
}

void Tree::createTwigs(Branch *aBranch)
{
  if (!aBranch)
  {
    aBranch = mRoot;
  }

  if (!aBranch->mChild0)
  {
    fvec3 tangent = normalize(cross(sub(aBranch->mParent->mChild0->mHead, aBranch->mParent->mHead), sub(aBranch->mParent->mChild1->mHead, aBranch->mParent->mHead)));
    fvec3 binormal = normalize(sub(aBranch->mHead, aBranch->mParent->mHead));
    //fvec3 normal = cross(tangent, binormal); //never used

    int vert1 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, mProperties.mTwigScale)), scaleVec(binormal, mProperties.mTwigScale * 2 - aBranch->mLength)));
    int vert2 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, -mProperties.mTwigScale)), scaleVec(binormal, mProperties.mTwigScale * 2 - aBranch->mLength)));
    int vert3 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, -mProperties.mTwigScale)), scaleVec(binormal, -aBranch->mLength)));
    int vert4 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, mProperties.mTwigScale)), scaleVec(binormal, -aBranch->mLength)));

    int vert8 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, mProperties.mTwigScale)), scaleVec(binormal, mProperties.mTwigScale * 2 - aBranch->mLength)));
    int vert7 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, -mProperties.mTwigScale)), scaleVec(binormal, mProperties.mTwigScale * 2 - aBranch->mLength)));
    int vert6 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, -mProperties.mTwigScale)), scaleVec(binormal, -aBranch->mLength)));
    int vert5 = mTwigVertCount;
    mTwigVert[mTwigVertCount++] = (add(add(aBranch->mHead, scaleVec(tangent, mProperties.mTwigScale)), scaleVec(binormal, -aBranch->mLength)));

    mTwigFace[mTwigFaceCount++] = { vert1, vert2, vert3 };
    mTwigFace[mTwigFaceCount++] = { vert4, vert1, vert3 };      
    mTwigFace[mTwigFaceCount++] = { vert6, vert7, vert8 };      
    mTwigFace[mTwigFaceCount++] = { vert6, vert8, vert5 };

    fvec3 normal = normalize(cross(sub(mTwigVert[vert1], mTwigVert[vert3]), sub(mTwigVert[vert2], mTwigVert[vert3])));
    fvec3 normal2 = normalize(cross(sub(mTwigVert[vert7], mTwigVert[vert6]), sub(mTwigVert[vert8], mTwigVert[vert6])));

    mTwigNormal[vert1] = (normal);
    mTwigNormal[vert2] = (normal);
    mTwigNormal[vert3] = (normal);
    mTwigNormal[vert4] = (normal);

    mTwigNormal[vert8] = (normal2);
    mTwigNormal[vert7] = (normal2);
    mTwigNormal[vert6] = (normal2);
    mTwigNormal[vert5] = (normal2);

    mTwigUV[vert1] = { 0, 0 };
    mTwigUV[vert2] = { 1, 0 };
    mTwigUV[vert3] = { 1, 1 };
    mTwigUV[vert4] = { 0, 1 };

    mTwigUV[vert8] = { 0, 0 };
    mTwigUV[vert7] = { 1, 0 };
    mTwigUV[vert6] = { 1, 1 };
    mTwigUV[vert5] = { 0, 1 };
  }
  else
  {
    createTwigs(aBranch->mChild0);
    createTwigs(aBranch->mChild1);
  }
}

void Tree::createForks(Branch *aBranch, float aRadius) {
  if (!aBranch) aBranch = mRoot;
  if (!aRadius) aRadius = mProperties.mMaxRadius;

  aBranch->mRadius = aRadius;

  if (aRadius > aBranch->mLength) aRadius = aBranch->mLength;

  int segments = mProperties.mSegments;

  float segmentAngle = M_PI * 2 / (float)segments;

  if (!aBranch->mParent)
  {
    aBranch->mRootRing = new int[segments];
    //create the root of the tree
    //branch.root = [];
    fvec3 axis = { 0, 1, 0 };
    int i;
    for (i = 0; i < segments; i++)
    {
      fvec3 left = { -1, 0, 0 };
      fvec3 vec = vecAxisAngle(left, axis, -segmentAngle * i);
      aBranch->mRootRing[i] = mVertCount;
      mVert[mVertCount++] = (scaleVec(vec, aRadius / mProperties.mRadiusFalloffRate));
    }
  }

  //cross the branches to get the left
  //add the branches to get the up
  if (aBranch->mChild0)
  {
    fvec3 axis;
    if (aBranch->mParent)
    {
      axis = normalize(sub(aBranch->mHead, aBranch->mParent->mHead));
    }
    else
    {
      axis = normalize(aBranch->mHead);
    }

    fvec3 axis1 = normalize(sub(aBranch->mHead, aBranch->mChild0->mHead));
    fvec3 axis2 = normalize(sub(aBranch->mHead, aBranch->mChild1->mHead));
    fvec3 tangent = normalize(cross(axis1, axis2));
    aBranch->mTangent = tangent;

    fvec3 axis3 = normalize(cross(tangent, normalize(add(scaleVec(axis1, -1), scaleVec(axis2, -1)))));
    fvec3 dir = { axis2.x, 0, axis2.z };
    fvec3 centerloc = add(aBranch->mHead, scaleVec(dir, -mProperties.mMaxRadius / 2));

    aBranch->mRing0 = new int[segments];
    aBranch->mRing1 = new int[segments];
    aBranch->mRing2 = new int[segments];

    int ring0count = 0;
    int ring1count = 0;
    int ring2count = 0;

    float scale = mProperties.mRadiusFalloffRate;

    if (aBranch->mChild0->mTrunktype || aBranch->mTrunktype)
    {
      scale = 1.0f / mProperties.mTaperRate;
    }

    //main segment ring
    int linch0 = mVertCount;
    aBranch->mRing0[ring0count++] = linch0;
    aBranch->mRing2[ring2count++] = linch0;
    mVert[mVertCount++] = (add(centerloc, scaleVec(tangent, aRadius * scale)));

    int start = mVertCount - 1;
    fvec3 d1 = vecAxisAngle(tangent, axis2, 1.57f);
    fvec3 d2 = normalize(cross(tangent, axis));
    float s = 1 / dot(d1, d2);
    int i;
    for (i = 1; i < segments / 2; i++)
    {
      fvec3 vec = vecAxisAngle(tangent, axis2, segmentAngle * i);
      aBranch->mRing0[ring0count++] = start + i;
      aBranch->mRing2[ring2count++] = start + i;
      vec = scaleInDirection(vec, d2, s);
      mVert[mVertCount++] = (add(centerloc, scaleVec(vec, aRadius * scale)));
    }
    int linch1 = mVertCount;
    aBranch->mRing0[ring0count++] = linch1;
    aBranch->mRing1[ring1count++] = linch1;
    mVert[mVertCount++] = (add(centerloc, scaleVec(tangent, -aRadius * scale)));
    for (i = segments / 2 + 1; i < segments; i++) {
      fvec3 vec = vecAxisAngle(tangent, axis1, segmentAngle * i);
      aBranch->mRing0[ring0count++] = mVertCount;
      aBranch->mRing1[ring1count++] = mVertCount;
      mVert[mVertCount++] = (add(centerloc, scaleVec(vec, aRadius * scale)));
    }
    aBranch->mRing1[ring1count++] = linch0; 
    aBranch->mRing2[ring2count++] = linch1; 
    start = mVertCount - 1;
    for (i = 1; i < segments / 2; i++) {
      fvec3 vec = vecAxisAngle(tangent, axis3, segmentAngle * i);
      aBranch->mRing1[ring1count++] = start + i;
      aBranch->mRing2[ring2count++] = start + (segments / 2 - i);
      fvec3 v = scaleVec(vec, aRadius * scale);
      mVert[mVertCount++] = (add(centerloc, v));
    }

    float radius0 = 1 * aRadius * mProperties.mRadiusFalloffRate;
    float radius1 = 1 * aRadius * mProperties.mRadiusFalloffRate;
    if (aBranch->mChild0->mTrunktype) {
      radius0 = aRadius * mProperties.mTaperRate;
    }
    createForks(aBranch->mChild0, radius0);
    createForks(aBranch->mChild1, radius1);
  } else {
    aBranch->mEnd = mVertCount;
    mVert[mVertCount++] = (aBranch->mHead);
  }
}

} // End of namespace.
