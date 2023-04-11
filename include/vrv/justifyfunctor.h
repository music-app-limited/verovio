/////////////////////////////////////////////////////////////////////////////
// Name:        justifyfunctor.h
// Author:      David Bauer
// Created:     2023
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_JUSTIFYFUNCTOR_H__
#define __VRV_JUSTIFYFUNCTOR_H__

#include "functor.h"

namespace vrv {

//----------------------------------------------------------------------------
// JustifyXFunctor
//----------------------------------------------------------------------------

/**
 * This class justifies the X positions.
 */
class JustifyXFunctor : public DocFunctor {
public:
    /**
     * @name Constructors, destructors
     */
    ///@{
    JustifyXFunctor(Doc *doc);
    virtual ~JustifyXFunctor() = default;
    ///@}

    /*
     * Abstract base implementation
     */
    bool ImplementsEndInterface() const override { return false; }

    /*
     * Set the full system width
     */
    void SetSystemFullWidth(int width) { m_systemFullWidth = width; }

    /*
     * Functor interface
     */
    ///@{
    FunctorCode VisitAlignment(Alignment *alignment) override;
    FunctorCode VisitMeasure(Measure *measure) override;
    FunctorCode VisitMeasureAligner(MeasureAligner *measureAligner) override;
    FunctorCode VisitScoreDef(ScoreDef *scoreDef) override;
    FunctorCode VisitSection(Section *section) override;
    FunctorCode VisitSystem(System *system) override;
    ///@}

protected:
    //
private:
    //
public:
    //
private:
    // The relative X position of the next measure
    int m_measureXRel;
    // The justification ratio
    double m_justifiableRatio;
    // The left/right barline X position
    int m_leftBarLineX;
    int m_rightBarLineX;
    // The system full width (without system margins)
    int m_systemFullWidth;
    // Indicates shift of next measure due to section restart
    bool m_applySectionRestartShift;
};

//----------------------------------------------------------------------------
// JustifyYFunctor
//----------------------------------------------------------------------------

/**
 * This class justifies the Y positions.
 */
class JustifyYFunctor : public DocFunctor {
public:
    /**
     * @name Constructors, destructors
     */
    ///@{
    JustifyYFunctor(Doc *doc);
    virtual ~JustifyYFunctor() = default;
    ///@}

    /*
     * Abstract base implementation
     */
    bool ImplementsEndInterface() const override { return false; }

    /*
     * Functor interface
     */
    ///@{
    FunctorCode VisitStaffAlignment(StaffAlignment *staffAlignment) override;
    FunctorCode VisitSystem(System *system) override;
    ///@}

protected:
    //
private:
    //
public:
    //
private:
    // The cumulated shift
    int m_cumulatedShift;
    // The relative shift of the staff w.r.t. the system
    int m_relativeShift;
    // The amount of space for distribution
    int m_spaceToDistribute;
    // The sum of justification factors per page
    double m_justificationSum;
    // A map of calculated shifts per StaffAlignment
    // => this is transferred to the JustifyYAdjustCrossStaffFunctor
    std::map<StaffAlignment *, int> m_shiftForStaff;
};

} // namespace vrv

#endif // __VRV_JUSTIFYFUNCTOR_H__
