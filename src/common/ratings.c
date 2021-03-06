/*
    This file is part of darktable,
    copyright (c) 2011 henrik andersson.

    darktable is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    darktable is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "common/collection.h"
#include "common/darktable.h"
#include "common/debug.h"
#include "common/image_cache.h"
#include "control/conf.h"
#include "control/control.h"
#include "gui/gtk.h"


void dt_ratings_apply_to_image(int imgid, int rating)
{
  dt_image_t *image = dt_image_cache_get(darktable.image_cache, imgid, 'w');
  // one star is a toggle, so you can easily reject images by removing the last star:
  if(((image->flags & 0x7) == 1) && (rating == 1)) rating = 0;
  image->flags = (image->flags & ~0x7) | (0x7 & rating);
  // synch through:
  dt_image_cache_write_release(darktable.image_cache, image, DT_IMAGE_CACHE_SAFE);

  dt_collection_hint_message(darktable.collection);
}

void dt_ratings_apply_to_selection(int rating)
{
  uint32_t count = dt_collection_get_selected_count(darktable.collection);
  if(count)
  {
    if(rating == 6)
      dt_control_log(ngettext("rejecting %d image", "rejecting %d images", count), count);
    else
      dt_control_log(ngettext("applying rating %d to %d image", "applying rating %d to %d images", count),
                     rating, count);
#if 0 // not updating cache
    gchar query[1024]= {0};
    g_snprintf(query,sizeof(query), "UPDATE main.images SET flags=(flags & ~7) | (7 & %d) WHERE id IN "
                                    "(SELECT imgid FROM main.selected_images)", rating);
    DT_DEBUG_SQLITE3_EXEC(dt_database_get(darktable.db), query, NULL, NULL, NULL);
#endif

    /* for each selected image update rating */
    sqlite3_stmt *stmt;
    DT_DEBUG_SQLITE3_PREPARE_V2(dt_database_get(darktable.db), "SELECT imgid FROM main.selected_images", -1, &stmt,
                                NULL);
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
      dt_ratings_apply_to_image(sqlite3_column_int(stmt, 0), rating);
    }
    sqlite3_finalize(stmt);

    /* redraw view */
    /* dt_control_queue_redraw_center() */
    /* needs to be called in the caller function */
  }
  else
    dt_control_log(_("no images selected to apply rating"));
}

// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-spaces modified;
