//
//  directional_distribution_functions.h
//
//  Created by 渡部 心 on 2015/06/28.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR__directional_distribution_functions__
#define __SLR__directional_distribution_functions__

#include "../defines.h"
#include "../references.h"
#include "../BasicTypes/Spectrum.h"
#include "geometry.h"
#include <type_traits>

namespace SLR {
#define BSDF_SAMPLE_ASSERT \
SLRAssert(ret.hasInf() == false && ret.hasNaN() == false,\
"BSDF value is an expected value.\n"\
"uComponent: %f, uDir: (%f, %f), dirIn: (%f, %f, %f), dirOut: (%f, %f, %f)",\
smp.uComponent, smp.uDir[0], smp.uDir[1], \
query.dir_sn.x, query.dir_sn.y, query.dir_sn.z, \
result->dir_sn.x, result->dir_sn.y, result->dir_sn.z)
    
#define BSDF_EVALUATE_ASSERT \
SLRAssert(ret.hasInf() == false && ret.hasNaN() == false,\
"BSDF value is an expected value.\n"\
"dirIn: (%f, %f, %f), dirOut: (%f, %f, %f)",\
query.dir_sn.x, query.dir_sn.y, query.dir_sn.z, \
dir.x, dir.y, dir.z)
    
#define BSDF_EVALUATE_PDF_ASSERT \
SLRAssert(std::isinf(ret) == false && std::isnan(ret) == false,\
"PDF value is an expected value. : dirIn: (%f, %f, %f)",\
query.dir_sn.x, query.dir_sn.y, query.dir_sn.z);
    
    struct DirectionType {
        enum InternalEnum : uint32_t {
            IE_LowFreq = 1 << 0,
            IE_HighFreq = 1 << 1,
            IE_Delta0D = 1 << 2,
            IE_Delta1D = 1 << 3,
            IE_Dispersive = 1 << 4,
            IE_NonDelta = IE_LowFreq | IE_HighFreq | IE_Dispersive,
            IE_Delta = IE_Delta0D | IE_Delta1D | IE_Dispersive,
            IE_AllFreq = IE_NonDelta | IE_Delta,
            
            IE_Reflection = 1 << 5,
            IE_Transmission = 1 << 6,
            IE_Emission = IE_Reflection,
            IE_Acquisition = IE_Reflection,
            IE_WholeSphere = IE_Reflection | IE_Transmission,
            
            IE_All = IE_AllFreq | IE_WholeSphere,
            
            IE_LowFreqReflection = IE_LowFreq | IE_Reflection,
            IE_LowFreqTransmission = IE_LowFreq | IE_Transmission,
            IE_LowFreqScattering = IE_LowFreqReflection | IE_LowFreqTransmission,
            IE_HighFreqReflection = IE_HighFreq | IE_Reflection,
            IE_HighFreqTransmission = IE_HighFreq | IE_Transmission,
            IE_HighFreqScattering = IE_HighFreqReflection | IE_HighFreqTransmission,
            IE_Delta0DReflection = IE_Delta0D | IE_Reflection,
            IE_Delta0DTransmission = IE_Delta0D | IE_Transmission,
            IE_Delta0DScattering = IE_Delta0DReflection | IE_Delta0DTransmission,
        };
        static const DirectionType LowFreq;
        static const DirectionType HighFreq;
        static const DirectionType Delta0D;
        static const DirectionType Delta1D;
        static const DirectionType Dispersive;
        static const DirectionType NonDelta;
        static const DirectionType Delta;
        static const DirectionType AllFreq;
        static const DirectionType Reflection;
        static const DirectionType Transmission;
        static const DirectionType Emission;
        static const DirectionType Acquisition;
        static const DirectionType WholeSphere;
        static const DirectionType All;
        static const DirectionType LowFreqReflection;
        static const DirectionType LowFreqTransmission;
        static const DirectionType LowFreqScattering;
        static const DirectionType HighFreqReflection;
        static const DirectionType HighFreqTransmission;
        static const DirectionType HighFreqScattering;
        static const DirectionType Delta0DReflection;
        static const DirectionType Delta0DTransmission;
        static const DirectionType Delta0DScattering;
        
        InternalEnum value;
        
        DirectionType(InternalEnum v = (InternalEnum)0) : value(v) { }
        DirectionType operator&(const DirectionType &r) const { return (InternalEnum)(value & r.value); }
        DirectionType operator|(const DirectionType &r) const { return (InternalEnum)(value | r.value); }
        DirectionType &operator&=(const DirectionType &r) { value = (InternalEnum)(value & r.value); return *this; }
        DirectionType &operator|=(const DirectionType &r) { value = (InternalEnum)(value | r.value); return *this; }
        DirectionType flip() const {
            bool r = value & IE_Reflection;
            bool t = value & IE_Transmission;
            return InternalEnum((value & IE_AllFreq) | (r ? IE_Transmission : 0) | (t ? IE_Reflection : 0));
        }
        operator bool() const { return value; }
        bool operator==(const DirectionType &r) const { return value == r.value; }
        bool operator!=(const DirectionType &r) const { return value != r.value; }
        
        bool matches(DirectionType t) const { return (value & t.value) == t.value; }
        bool isDelta() const { return (*this & (Delta | WholeSphere)) == *this; }
        bool hasNonDelta() const { return *this & NonDelta; }
        bool hasDelta() const { return *this & Delta; }
        bool isDispersive() const { return *this & Dispersive; }
    };
    
    
    struct EDFQuery {
        DirectionType flags;
        EDFQuery(DirectionType f = DirectionType::All) : flags(f) { };
    };
    
    struct EDFSample {
        float uComponent;
        float uDir[2];
        EDFSample(float uComp, float uDir0, float uDir1) : uComponent(uComp), uDir{uDir0, uDir1} {}
    };
    
    struct EDFQueryResult {
        Vector3D dir_sn;
        float dirPDF;
        DirectionType dirType;
    };
    
    struct BSDFQuery {
        Vector3D dir_sn;
        Normal3D gNormal_sn;
        int16_t wlHint;
        DirectionType flags;
        bool adjoint;
        
        BSDFQuery(const Vector3D &dirSN, const Normal3D &gNormalSN, int16_t wl, DirectionType f = DirectionType::All, bool adj = false) :
        dir_sn(dirSN), gNormal_sn(gNormalSN), wlHint(wl), flags(f), adjoint(adj) { }
    };
    
    struct BSDFSample {
        float uComponent;
        float uDir[2];
        BSDFSample(float uComp, float uDir0, float uDir1) : uComponent(uComp), uDir{uDir0, uDir1} { }
    };
    
    struct BSDFQueryResult {
        Vector3D dir_sn;
        float dirPDF;
        DirectionType dirType;
    };
    
    struct IDFSample {
        float uDir[2];
        IDFSample(float uDir0, float uDir1) : uDir{uDir0, uDir1} { }
    };
    
    struct IDFQueryResult {
        Vector3D dirLocal;
        float dirPDF;
    };
    
    
    class EDF {
    protected:
        const DirectionType m_type;
        
        bool matches(DirectionType flags, EDFQueryResult* result) const {
            if (matches(flags))
                return true;
            result->dirPDF = 0.0f;
            result->dirType = DirectionType();
            return false;
        }
        friend class MultiEDF;
    public:
        EDF(DirectionType type) : m_type(type) { };
        virtual ~EDF() { };
        
        virtual SampledSpectrum sample(const EDFQuery &query, const EDFSample &smp, EDFQueryResult* result) const = 0;
        virtual SampledSpectrum evaluate(const EDFQuery &query, const Vector3D &dir) const = 0;
        virtual float evaluatePDF(const EDFQuery &query, const Vector3D &dir) const = 0;
        virtual float weight(const EDFQuery &query) const = 0;
        
        virtual bool matches(DirectionType flags) const { return m_type & flags; }
        bool hasNonDelta() const { return matches(DirectionType::WholeSphere | DirectionType::NonDelta); }
    };
    
    class BSDF {
    protected:
        const DirectionType m_type;
        
        DirectionType sideTest(const Normal3D& ng, const Vector3D& d0, const Vector3D &d1) const {
            bool reflect = dot(Vector3D(ng), d0) * dot(Vector3D(ng), d1) > 0;
            return DirectionType::AllFreq | (reflect ? DirectionType::Reflection : DirectionType::Transmission);
        }
        bool matches(DirectionType flags, BSDFQueryResult* result) const {
            if (matches(flags))
                return true;
            result->dirPDF = 0.0f;
            result->dirType = DirectionType();
            return false;
        }
        virtual SampledSpectrum sampleInternal(const BSDFQuery &query, const BSDFSample &smp, BSDFQueryResult* result) const = 0;
        virtual SampledSpectrum evaluateInternal(const BSDFQuery &query, const Vector3D &dir) const = 0;
        virtual float evaluatePDFInternal(const BSDFQuery &query, const Vector3D &dir) const = 0;
        virtual float weightInternal(const BSDFQuery &query, const BSDFSample &smp) const = 0;
        virtual float weightInternal(const BSDFQuery &query, const Vector3D &dir) const = 0;
        virtual SampledSpectrum getBaseColorInternal(DirectionType flags) const = 0;
        friend class MultiBSDF;
        friend class InverseBSDF;
    public:
        BSDF(DirectionType type) : m_type(type) { }
        virtual ~BSDF() { }
        
        SampledSpectrum sample(const BSDFQuery &query, const BSDFSample &smp, BSDFQueryResult* result) const {
            if (!matches(query.flags, result))
                return SampledSpectrum::Zero;
            return sampleInternal(query, smp, result);
        }
        SampledSpectrum evaluate(const BSDFQuery &query, const Vector3D &dir) const {
            BSDFQuery mQuery = query;
            mQuery.flags &= sideTest(query.gNormal_sn, query.dir_sn, dir);
            if (!matches(mQuery.flags))
                return SampledSpectrum::Zero;
            return evaluateInternal(mQuery, dir);
        }
        float evaluatePDF(const BSDFQuery &query, const Vector3D &dir) const {
            if (!matches(query.flags))
                return 0;
            return evaluatePDFInternal(query, dir);
        }
        float weight(const BSDFQuery &query, const BSDFSample &smp) const {
            if (!matches(query.flags))
                return 0;
            return weightInternal(query, smp);
        }
        float weight(const BSDFQuery &query, const Vector3D &dir) const {
            if (!matches(query.flags))
                return 0;
            return weightInternal(query, dir);
        }
        
        SampledSpectrum getBaseColor(DirectionType flags) const {
            if (!matches(flags))
                return SampledSpectrum::Zero;
            return getBaseColorInternal(flags);
        }
        
        virtual bool matches(DirectionType flags) const { return (m_type & flags) == m_type; };
        bool hasNonDelta() const { return matches(DirectionType::WholeSphere | DirectionType::NonDelta); }
    };
    
    class IDF {
    protected:
        const DirectionType m_type;
        
        bool matches(DirectionType flags, BSDFQueryResult* result) const {
            if (matches(flags))
                return true;
            result->dirPDF = 0.0f;
            result->dirType = DirectionType();
            return false;
        };
    public:
        IDF(DirectionType type) : m_type(type) { };
        virtual ~IDF() { };
        
        virtual SampledSpectrum sample(const IDFSample &smp, IDFQueryResult* result) const = 0;
        virtual SampledSpectrum evaluate(const Vector3D &dirIn) const = 0;
        virtual float evaluatePDF(const Vector3D &dirIn) const = 0;
        
        virtual bool matches(DirectionType flags) const { return m_type & flags; }
        bool hasNonDelta() const { return matches(DirectionType::WholeSphere | DirectionType::NonDelta); }
    };
    
    
    class Fresnel {
    public:
        virtual ~Fresnel() { };
        virtual SampledSpectrum evaluate(float cosEnter) const = 0;
    };
    
    class FresnelNoOp : public Fresnel {
    public:
        SampledSpectrum evaluate(float cosEnter) const override;
    };
    
    class FresnelConductor : public Fresnel {
        SampledSpectrum m_eta;
        SampledSpectrum m_k;
    public:
        FresnelConductor(const SampledSpectrum &eta, const SampledSpectrum &k) : m_eta(eta), m_k(k) { };
        
        SampledSpectrum evaluate(float cosEnter) const override;
    };
    
    class FresnelDielectric : public Fresnel {
        SampledSpectrum m_etaExt;
        SampledSpectrum m_etaInt;
    public:
        FresnelDielectric(const SampledSpectrum &etaExt, const SampledSpectrum &etaInt) : m_etaExt(etaExt), m_etaInt(etaInt) { };
        
        SampledSpectrum etaExt() const { return m_etaExt; };
        SampledSpectrum etaInt() const { return m_etaInt; };
        
        SampledSpectrum evaluate(float cosEnter) const override;
        
        static float evalF(float etaEnter, float etaExit, float cosEnter, float cosExit);
    };    
}

#endif