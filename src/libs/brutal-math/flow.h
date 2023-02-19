#pragma once

#include "rect.h"

typedef enum
{
    M_FLOW_LEFT_TO_RIGHT,
    M_FLOW_RIGHT_TO_LEFT,
    M_FLOW_TOP_TO_BOTTOM,
    M_FLOW_BOTTOM_TO_TOP,
} MFlow;

MFlow m_flow_relative(MFlow parent, MFlow child);

MVec2f m_flow_to_vec(MFlow flow);

float m_flow_get_start(MFlow flow, MRectf rect);

float m_flow_get_end(MFlow flow, MRectf rect);

float m_flow_get_top(MFlow flow, MRectf rect);

float m_flow_get_bottom(MFlow flow, MRectf rect);

float m_flow_get_width(MFlow flow, MRectf rect);

float m_flow_get_height(MFlow flow, MRectf rect);

MVec2f m_flow_get_origin(MFlow flow, MRectf rect);

float m_flow_get_hcenter(MFlow flow, MRectf rect);

float m_flow_get_vcenter(MFlow flow, MRectf rect);

MRectf m_flow_set_start(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_x(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_end(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_top(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_y(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_bottom(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_origin(MFlow flow, MRectf rect, MVec2f value);

MRectf m_flow_set_width(MFlow flow, MRectf rect, float value);

MRectf m_flow_set_height(MFlow flow, MRectf rect, float value);
