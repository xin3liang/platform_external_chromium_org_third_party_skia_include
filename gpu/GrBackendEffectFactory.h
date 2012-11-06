/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrBackendEffectFactory_DEFINED
#define GrBackendEffectFactory_DEFINED

#include "GrTypes.h"
#include "SkTemplates.h"
#include "SkThread_platform.h"
#include "GrNoncopyable.h"

/** Given a GrEffect of a particular type, creates the corresponding graphics-backend-specific
    effect object. Also tracks equivalence of shaders generated via a key. Each factory instance
    is assigned a generation ID at construction. The ID of the return of GrEffect::getFactory()
    is used as a type identifier. Thus a GrEffect subclass must return a singleton from
    getFactory(). GrEffect subclasses should use the derived class GrTBackendEffectFactory that is
    templated on the GrEffect subclass as their factory object. It requires that the GrEffect
    subclass has a nested class (or typedef) GLEffect which is its GL implementation and a subclass
    of GrGLEffect.
 */

class GrEffect;
class GrEffectStage;
class GrGLEffect;
class GrGLCaps;

class GrBackendEffectFactory : public GrNoncopyable {
public:
    typedef uint32_t EffectKey;
    enum {
        kNoEffectKey = 0,
        kEffectKeyBits = 12,
        /**
         * Some aspects of the generated code may be determined by the particular textures that are
         * associated with the effect. These manipulations are performed by GrGLShaderBuilder beyond
         * GrGLEffects' control. So there is a dedicated part of the key which is combined
         * automatically with the bits produced by GrGLEffect::GenKey().
         */
        kTextureKeyBits = 6
    };

    virtual EffectKey glEffectKey(const GrEffectStage&, const GrGLCaps&) const = 0;
    virtual GrGLEffect* createGLInstance(const GrEffect&) const = 0;

    bool operator ==(const GrBackendEffectFactory& b) const {
        return fEffectClassID == b.fEffectClassID;
    }
    bool operator !=(const GrBackendEffectFactory& b) const {
        return !(*this == b);
    }

    virtual const char* name() const = 0;

protected:
    enum {
        kIllegalEffectClassID = 0,
    };

    GrBackendEffectFactory() {
        fEffectClassID = kIllegalEffectClassID;
    }

    static EffectKey GenID() {
        GR_DEBUGCODE(static const int32_t kClassIDBits = 8 * sizeof(EffectKey) -
                                            kTextureKeyBits -
                                    kEffectKeyBits);
        // fCurrEffectClassID has been initialized to kIllegalEffectClassID. The
        // atomic inc returns the old value not the incremented value. So we add
        // 1 to the returned value.
        int32_t id = sk_atomic_inc(&fCurrEffectClassID) + 1;
        GrAssert(id < (1 << kClassIDBits));
        return static_cast<EffectKey>(id);
    }

    EffectKey fEffectClassID;

private:
    static int32_t fCurrEffectClassID;
};

#endif
