#pragma once

#include <brutal/math/rect.h>

typedef enum
{
    M_FLOW_LEFT_TO_RIGHT,
    M_FLOW_RIGHT_TO_LEFT,
    M_FLOW_TOP_TO_BOTTOM,
    M_FLOW_BOTTOM_TO_TOP,
} MFlow;

MFlow m_flow_relative(MFlow parent, MFlow child);

MVec2 m_flow_to_vec(MFlow flow);

float m_flow_get_start(MFlow flow, MRect rect);

float m_flow_get_end(MFlow flow, MRect rect);

float m_flow_get_top(MFlow flow, MRect rect);

float m_flow_get_bottom(MFlow flow, MRect rect);

float m_flow_get_width(MFlow flow, MRect rect);

float m_flow_get_height(MFlow flow, MRect rect);

MVec2 m_flow_get_origin(MFlow flow, MRect rect);

float m_flow_get_hcenter(MFlow flow, MRect rect);

float m_flow_get_vcenter(MFlow flow, MRect rect);

MRect m_flow_set_start(MFlow flow, MRect rect, float value);

MRect m_flow_set_x(MFlow flow, MRect rect, float value);

MRect m_flow_set_end(MFlow flow, MRect rect, float value);

MRect m_flow_set_top(MFlow flow, MRect rect, float value);

MRect m_flow_set_y(MFlow flow, MRect rect, float value);

MRect m_flow_set_bottom(MFlow flow, MRect rect, float value);

MRect m_flow_set_origin(MFlow flow, MRect rect, MVec2 value);

MRect m_flow_set_width(MFlow flow, MRect rect, float value);

MRect m_flow_set_height(MFlow flow, MRect rect, float value);
