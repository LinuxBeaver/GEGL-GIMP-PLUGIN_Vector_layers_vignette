/* This file is an image processing operation for GEGL
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
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * Beaver's Invert Transparency filter.  2022 
 */

/*
This is the GEGL Graph of invert transparency. So  users can test it without installing.

id=1  color-overlay value=#ffffff gimp:layer-mode layer-mode=behind opacity=1.00 aux=[ color value=#000000  ]  id=2 gimp:layer-mode layer-mode=color-erase opacity=1.00 aux=[ color value=#ffffff ] crop
color-overlay value=#0aff00

--
The code for this filter is very simple. Thus a good plugin to study for people who want to develop new GEGL filters. All it is is a embedded GEGL Graph and a color overlay.
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

#define TUTORIAL \
" id=1  color-overlay value=#ffffff gimp:layer-mode layer-mode=behind opacity=1.00 aux=[ color value=#000000  ]  id=2 gimp:layer-mode layer-mode=color-erase opacity=1.00 aux=[ color value=#ffffff ] crop aux=[ ref=1 ] "\



property_color (value, _("Color"), "#000000")
    description (_("The color to paint over the inverted transparency"))
    ui_meta     ("role", "color-primary")

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     invert_alpha
#define GEGL_OP_C_SOURCE invert-alpha.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *it, *col;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  it    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", TUTORIAL,
                                  NULL);

  col    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

      gegl_operation_meta_redirect (operation, "value", col, "value");

      gegl_node_link_many (input, it, col, output, NULL);
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:invert-transparency",
    "title",       _("Invert Transparency"),
    "reference-hash", "2kc15254a2385110001adc2544142af",
    "description", _("Replace Transparency with color and color with transparency "
                     ""),
    NULL);
}

#endif
