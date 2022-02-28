/////////////////////////////////////////////////////////////////////////////
// Name:        staffdef.cpp
// Author:      Laurent Pugin
// Created:     2017
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "staffdef.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "functorparams.h"
#include "instrdef.h"
#include "label.h"
#include "labelabbr.h"
#include "layerdef.h"
#include "metersiggrp.h"
#include "staffgrp.h"
#include "tuning.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// StaffDef
//----------------------------------------------------------------------------

static const ClassRegistrar<StaffDef> s_factory("staffDef", STAFFDEF);

StaffDef::StaffDef()
    : ScoreDefElement(STAFFDEF, "staffdef-")
    , AttDistances()
    , AttLabelled()
    , AttNInteger()
    , AttNotationType()
    , AttScalable()
    , AttStaffDefLog()
    , AttStaffDefVis()
    , AttTimeBase()
    , AttTransposition()
{
    this->RegisterAttClass(ATT_DISTANCES);
    this->RegisterAttClass(ATT_LABELLED);
    this->RegisterAttClass(ATT_NINTEGER);
    this->RegisterAttClass(ATT_NOTATIONTYPE);
    this->RegisterAttClass(ATT_SCALABLE);
    this->RegisterAttClass(ATT_STAFFDEFLOG);
    this->RegisterAttClass(ATT_STAFFDEFVIS);
    this->RegisterAttClass(ATT_TIMEBASE);
    this->RegisterAttClass(ATT_TRANSPOSITION);

    this->Reset();
}

StaffDef::~StaffDef() {}

void StaffDef::Reset()
{
    ScoreDefElement::Reset();
    StaffDefDrawingInterface::Reset();
    this->ResetDistances();
    this->ResetLabelled();
    this->ResetNInteger();
    this->ResetNotationType();
    this->ResetScalable();
    this->ResetStaffDefLog();
    this->ResetStaffDefVis();
    this->ResetTimeBase();
    this->ResetTransposition();

    m_drawingVisibility = OPTIMIZATION_NONE;
}

bool StaffDef::IsSupportedChild(Object *child)
{
    if (child->Is(CLEF)) {
        assert(dynamic_cast<Clef *>(child));
    }
    else if (child->Is(INSTRDEF)) {
        assert(dynamic_cast<InstrDef *>(child));
    }
    else if (child->Is(KEYSIG)) {
        assert(dynamic_cast<KeySig *>(child));
    }
    else if (child->Is(LABEL)) {
        assert(dynamic_cast<Label *>(child));
    }
    else if (child->Is(LABELABBR)) {
        assert(dynamic_cast<LabelAbbr *>(child));
    }
    else if (child->Is(LAYERDEF)) {
        assert(dynamic_cast<LayerDef *>(child));
    }
    else if (child->Is(MENSUR)) {
        assert(dynamic_cast<Mensur *>(child));
    }
    else if (child->Is(METERSIG)) {
        assert(dynamic_cast<MeterSig *>(child));
    }
    else if (child->Is(METERSIGGRP)) {
        assert(dynamic_cast<MeterSigGrp *>(child));
    }
    else if (child->Is(TUNING)) {
        assert(dynamic_cast<Tuning *>(child));
    }
    else {
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------
// StaffDef functor methods
//----------------------------------------------------------------------------

int StaffDef::ReplaceDrawingValuesInStaffDef(FunctorParams *functorParams)
{
    ReplaceDrawingValuesInStaffDefParams *params
        = vrv_params_cast<ReplaceDrawingValuesInStaffDefParams *>(functorParams);
    assert(params);

    if (params->m_clef) {
        this->SetCurrentClef(params->m_clef);
    }
    if (params->m_keySig) {
        this->SetCurrentKeySig(params->m_keySig);
    }
    if (params->m_mensur) {
        this->SetCurrentMensur(params->m_mensur);
    }
    if (params->m_meterSig) {
        this->SetCurrentMeterSig(params->m_meterSig);
    }
    if (params->m_meterSigGrp) {
        this->SetCurrentMeterSigGrp(params->m_meterSigGrp);
    }

    return FUNCTOR_CONTINUE;
}

int StaffDef::SetStaffDefRedrawFlags(FunctorParams *functorParams)
{
    SetStaffDefRedrawFlagsParams *params = vrv_params_cast<SetStaffDefRedrawFlagsParams *>(functorParams);
    assert(params);

    const bool forceRedraw = params->m_redrawFlags & StaffDefRedrawFlags::FORCE_REDRAW;
    const bool redrawClef = params->m_redrawFlags & StaffDefRedrawFlags::REDRAW_CLEF;
    if (redrawClef || forceRedraw) {
        this->SetDrawClef(redrawClef);
    }
    const bool redrawKeySig = params->m_redrawFlags & StaffDefRedrawFlags::REDRAW_KEYSIG;
    if (redrawKeySig || forceRedraw) {
        this->SetDrawKeySig(redrawKeySig);
    }
    const bool redrawMensur = params->m_redrawFlags & StaffDefRedrawFlags::REDRAW_MENSUR;
    if (redrawMensur || forceRedraw) {
        this->SetDrawMensur(redrawMensur);
    }
    const bool redrawMeterSig = params->m_redrawFlags & StaffDefRedrawFlags::REDRAW_METERSIG;
    if (redrawMeterSig || forceRedraw) {
        this->SetDrawMeterSig(redrawMeterSig);
    }
    const bool redrawMeterSigGrp = params->m_redrawFlags & StaffDefRedrawFlags::REDRAW_METERSIGGRP;
    if (redrawMeterSigGrp || forceRedraw) {
        this->SetDrawMeterSigGrp(redrawMeterSigGrp);
    }

    return FUNCTOR_CONTINUE;
}

int StaffDef::PrepareDuration(FunctorParams *functorParams)
{
    PrepareDurationParams *params = vrv_params_cast<PrepareDurationParams *>(functorParams);
    assert(params);

    if (this->HasDurDefault() && this->HasN()) {
        params->m_durDefaultForStaffN[this->GetN()] = this->GetDurDefault();
    }

    return FUNCTOR_CONTINUE;
}

} // namespace vrv
