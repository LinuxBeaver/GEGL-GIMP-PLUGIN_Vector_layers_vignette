/* This file is an image prtheessing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2024 Beaver - GEGL Shapes
 * 
 
vignette radius=0.33 squeeze=0.40 softness=0.01
lb:invert-transparency
lb:outlinebevel mode=outline
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES



property_boolean (vignette_switch, _("Use internal vignette"), TRUE)
  description    (_("If this option is not checked the user must use the vignette in GIMP 3 below this filter, non-destructively of course"))


property_boolean (fill_switch, _("Remove fill"), FALSE)
  description    (_("If this option is checked the fill will be removed"))


property_boolean (outline_switch, _("Apply Outline"), TRUE)
  description    (_("Outline of the vignette"))


/*
property_boolean (outline_normal, _("Apply fill"), FALSE)
  description    (_("Should outline be accompanied by a fill or by itself"))
ui_meta ("visible", "guichange {outline, outlinebevel}")
*/


property_color (outline_color, _("Outline color"), "#000000")
    description (_("The color to paint over the outline. It defaults to black"))


property_color (shape_color, _("Shape color"), "#00db54")
    description (_("The color to paint over the shape. It defaults to a green"))


enum_start (gegl_vignette_shape_bv)
  enum_value (GEGL_VIGNETTE_SHAPE_CIRCLE,     "circle",     N_("Circle"))
  enum_value (GEGL_VIGNETTE_SHAPE_SQUARE,     "square",     N_("Square"))
  enum_value (GEGL_VIGNETTE_SHAPE_DIAMOND,    "diamond",    N_("Diamond"))
  enum_value (GEGL_VIGNETTE_SHAPE_HORIZONTAL, "horizontal", N_("Horizontal"))
  enum_value (GEGL_VIGNETTE_SHAPE_VERTICAL,   "vertical",   N_("Vertical"))
enum_end (GeglVignetteShape_bv)

property_enum (shape, _("Shape to use"),
    GeglVignetteShape_bv, gegl_vignette_shape_bv,
    GEGL_VIGNETTE_SHAPE_CIRCLE)
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")

property_double (shape_radius, _("Shape radius"), 0.215) 
    description (_("How far out shape goes as portion of half image diagonal"))
    value_range (0.0, G_MAXDOUBLE)
    ui_range    (0.0, 0.60)
    ui_meta     ("unit", "relative-distance")
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")

property_double (shape_proportion, _("Shape proportion"), 1.0)
    description(_("How close we are to image proportions"))
    value_range (0.0, 1.0)
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")


property_double (shape_squeeze, _("Shape Squeeze (oval to circle fix)"), -0.40)
    description (_("On 16:9 canvases it is guaranteed to make a circle but on non 16:9 canvases the user will need to manually adjust things"))
    value_range (-1.0, 1.0)
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")


property_double (shape_x, _("Shape center X"), 0.5)
    ui_range    (0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "x")
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")

property_double (shape_y, _("Shape center Y"), 0.5)
    ui_range    (0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "y")
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")

property_double (shape_rotation, _("Shape rotation"), 0.0)
    value_range (0.0, 360.0)
    ui_meta     ("unit", "degree") /* XXX: change to radians */
    ui_meta     ("direction", "cw")
  ui_meta     ("sensitive", " vignette_switch")
  ui_meta ("visible", "vignette_switch")

enum_start (gegl_stroke_grow_shapeoutlinesb)
  enum_value (GEGL_stroke_GROW_SHAPE_SQUARE2,  "square",  N_("Square"))
  enum_value (GEGL_stroke_GROW_SHAPE_CIRCLE2,  "circle",  N_("Circle"))
  enum_value (GEGL_stroke_GROW_SHAPE_DIAMOND2, "diamond", N_("Diamond"))
enum_end (GeglstrokeGrowShapeoutlinesb)

property_enum   (grow_shape, _("Outline's internal median"),
                 GeglstrokeGrowShapeoutlinesb, gegl_stroke_grow_shapeoutlinesb,
                 GEGL_stroke_GROW_SHAPE_CIRCLE2)
  description   (_("The shape of the border's internal median. This is not the same thing as the shape. Dropshadow has the same option"))



property_double (stroke, _("Grow radius of outline"), 9.0)
  value_range   (0, 100.0)
  ui_range      (0, 50.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the outline."))

property_double (opacity, _("Opacity of outline"), 2)
  value_range   (0.0, 4.0)
  ui_steps      (0.0, 4.0)


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     gshapes
#define GEGL_OP_C_SOURCE gshapes.c

#include "gegl-op.h"

typedef struct
{
 GeglNode *input;
 GeglNode *outlinedeluxe;
 GeglNode *inverttrans;
 GeglNode *vignette;
 GeglNode *nop1;
 GeglNode *nop2;
 GeglNode *nop3;
 GeglNode *erase;
 GeglNode *normal;
 GeglNode *input2;
 GeglNode *input3;
 GeglNode *none;
 GeglNode *opacity;
 GeglNode *median;
 GeglNode *output;
}State;

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);

  State *state = o->user_data = g_malloc0 (sizeof (State));
 

  state->input    = gegl_node_get_input_proxy (gegl, "input");
  state->output   = gegl_node_get_output_proxy (gegl, "output");

  state->outlinedeluxe = gegl_node_new_child (gegl,
                                  "operation", "lb:ssg", "radius", 0,
                                  NULL);
  state->inverttrans = gegl_node_new_child (gegl,
                                  "operation", "lb:invert-transparency",
                                  NULL);

  state->vignette = gegl_node_new_child (gegl,
                                  "operation", "gegl:vignette", "shape", 0, "squeeze", -0.40, "softness", 0.01, 
                                  NULL);

  state->nop1 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);
  state->nop2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);
  state->nop3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);
  state->normal = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);
  state->input2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);
  state->input3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);
  state->erase = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst-out",
                                  NULL);
  state->none = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst",
                                  NULL);
  state->opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 2.0,
                                  NULL);
  state->median = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 0,
                                  NULL);


  gegl_operation_meta_redirect (operation, "outline_color",   state->outlinedeluxe, "colorssg");
  gegl_operation_meta_redirect (operation, "shape_color",   state->inverttrans, "value");

  gegl_operation_meta_redirect (operation, "shape",    state->vignette, "shape");
  gegl_operation_meta_redirect (operation, "shape_radius",   state->vignette, "radius");
  gegl_operation_meta_redirect (operation, "shape_proportion",    state->vignette, "proportion");
  gegl_operation_meta_redirect (operation, "shape_squeeze",    state->vignette, "squeeze");
  gegl_operation_meta_redirect (operation, "shape_x",    state->vignette, "x");
  gegl_operation_meta_redirect (operation, "shape_y",    state->vignette, "y");
  gegl_operation_meta_redirect (operation, "shape_rotation",    state->vignette, "rotation");

  gegl_operation_meta_redirect (operation, "opacity",   state->outlinedeluxe, "opacityssg");
  gegl_operation_meta_redirect (operation, "stroke",   state->outlinedeluxe, "stroke");
  gegl_operation_meta_redirect (operation, "grow_shape",   state->outlinedeluxe, "grow_shape");


} 

static void update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;
  GeglNode *vignette;
/*
  GeglNode *inverttrans;
*/
  GeglNode *outlinedeluxe;
  GeglNode *erase;

  if (o->vignette_switch)
vignette = state->vignette;

  if (!o->vignette_switch)
vignette = state->nop1;

  if (o->fill_switch)
erase = state->erase;

  if (!o->fill_switch)
erase = state->none;

  if (o->outline_switch)
outlinedeluxe = state->outlinedeluxe;

  if (!o->outline_switch)
outlinedeluxe = state->nop3;

/*
  if (!o->outline_normal)
  gegl_node_link_many (state->input, vignette, inverttrans, state->input3, outlinedeluxe,  state->output, NULL);

  if (o->outline_normal)
*/

  gegl_node_link_many (state->input, vignette, state->inverttrans, state->input3, state->input2, state->normal,  erase, state->median, state->output, NULL);
  gegl_node_connect (state->normal, "aux", outlinedeluxe, "output");
  gegl_node_link_many (state->input2, outlinedeluxe, NULL);
  gegl_node_link_many (state->input3, state->opacity, NULL);
  gegl_node_connect (erase, "aux", state->opacity, "output");

}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:shapes",
    "title",       _("Draw Shapes"),
    "reference-hash", "gimpcandrawacircle",
    "description", _("Draw circles and squares in GIMP with GEGL. Alternatively there is a mode to hide the internal vignette and use GIMP's built in vignette'"),
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Render Shapes..."),
    NULL);
}

#endif
