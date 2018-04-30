#include <cmath>
#include <string.h>

namespace Sybil {

typedef struct {
  float x, y, z;
} fvec3;

typedef struct {
  float u, v;
} fvec2;

typedef struct {
  int x, y, z;
} ivec3;

class Properties {
 public:
  float mClumpMax;
  float mClumpMin;
  float mLengthFalloffFactor;
  float mLengthFalloffPower;
  float mBranchFactor;
  float mRadiusFalloffRate;
  float mClimbRate;
  float mTrunkKink;
  float mMaxRadius;
  int mTreeSteps;
  float mTaperRate;
  float mTwistRate;
  int mSegments;
  int mLevels;
  float mSweepAmount;
  float mInitialBranchLength;
  float mTrunkLength;
  float mDropAmount;
  float mGrowAmount;
  float mVMultiplier;
  float mTwigScale;
  int mSeed;
  int mRseed;

  Properties();
  Properties(
    float aClumpMax,
    float aClumpMin,
    float aLengthFalloffFactor,
    float aLengthFalloffPower,
    float aBranchFactor,
    float aRadiusFalloffRate,
    float aClimbRate,
    float aTrunkKink,
    float aMaxRadius,
    int aTreeSteps,
    float aTaperRate,
    float aTwistRate,
    int aSegments,
    int aLevels,
    float aSweepAmount,
    float aInitialBranchLength,
    float aTrunkLength,
    float aDropAmount,
    float aGrowAmount,
    float aVMultiplier,
    float aTwigScale,
    int aSeed
  );
  float random(float aFixed);
};

class Branch {
 public:
  Branch *mChild0;
  Branch *mChild1;
  Branch *mParent;
  fvec3 mHead;
  fvec3 mTangent;
  float mLength;
  int mTrunktype;
  int *mRing0, *mRing1, *mRing2;
  int *mRootRing;
  float mRadius;
  int mEnd;

  ~Branch();
  Branch();
  Branch(fvec3 aHead, Branch *aParent);
  void split(int aLevel, int aSteps, Properties &aProperties, int aL1 = 1, int aL2 = 1);
};

class Tree {
  Branch *mRoot;
  void init();
  void allocVertBuffers();
  void allocFaceBuffers();
  void calcVertSizes(Branch *aBranch);
  void calcFaceSizes(Branch *aBranch);
  void calcNormals();
  void doFaces(Branch *aBranch);
  void createTwigs(Branch *aBranch);
  void createForks(Branch *aBranch, float aRadius);
  void fixUVs();

 public:
  Properties mProperties;
  int mVertCount;
  int mTwigVertCount;
  int mFaceCount;
  int mTwigFaceCount;

  fvec3 *mVert;
  fvec3 *mNormal;
  fvec2 *mUV;
  fvec3 *mTwigVert;
  fvec3 *mTwigNormal;
  fvec2 *mTwigUV;
  ivec3 *mFace;
  ivec3 *mTwigFace;

  Tree();
  ~Tree();
  void generate();
};

//////////////////////
fvec3 mirrorBranch(fvec3 aVec, fvec3 aNorm, Properties &aProperties);
fvec3 axisAngle(fvec3 aVec, fvec3 aAxis, float aAngle);
fvec3 scaleInDirection(fvec3 aVector, fvec3 aDirection, float aScale);
fvec3 scaleVec(fvec3 a, float b);
fvec3 add(fvec3 a, fvec3 b);
fvec3 sub(fvec3 a, fvec3 b);
float dot(fvec3 a, fvec3 b);
fvec3 cross(fvec3 a, fvec3 b);
fvec3 normalize(fvec3 a);
float length_n(Sybil::fvec3 a);

} // End of namespace.
