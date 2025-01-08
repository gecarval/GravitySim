/*                                                        :::      ::::::::   */
/*   hashmap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 17:05:06 by gecarval          #+#    #+#             */
/*   Updated: 2024/09/02 14:34:58 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

t_hashmap	**create_hashmap(t_data *data)
{
  t_hashmap	**hashmap;
  int			i;
  int     j;

  hashmap = (t_hashmap **)malloc(sizeof(t_hashmap *) * HASHMAP_DIV);
  if (hashmap == NULL)
    display_error(data, "hashmap malloc error\n");
  i = -1;
  while (++i < HASHMAP_DIV)
  {
    hashmap[i] = (t_hashmap *)malloc(sizeof(t_hashmap) * HASHMAP_DIV);
    if (hashmap[i] == NULL)
      display_error(data, "hashmap malloc error\n");
    j = -1;
    while (++j < HASHMAP_DIV)
    {
      hashmap[i][j].hashquad = create_rectangle(
        ((data->winx / HASHMAP_DIV) * j) + ((data->winx / HASHMAP_DIV) / 2),
        ((data->winy / HASHMAP_DIV) * i) + ((data->winy / HASHMAP_DIV) / 2),
        data->winx / HASHMAP_DIV,
        data->winy / HASHMAP_DIV);
      hashmap[i][j].hashkey = NULL;
    }
  }
  return (hashmap);
}

void  insert_points_hashmap(t_hashmap **hashmap, t_data *data)
{
  int i;
  int posy;
  int posx;
  t_point *tmp2;
  t_particle *tmp;
  t_hashkey *key;

  i = -1;
  tmp = data->gsim->part;
  while (++i < data->num_of_particles)
  {
    key = (t_hashkey *)malloc(sizeof(t_hashkey));
    if (key == NULL)
      display_error(data, "hashkey malloc error\n");
    tmp2 = (t_point *)malloc(sizeof(t_point));
    if (tmp2 == NULL)
      display_error(data, "point malloc error\n");
    tmp2->x = tmp->pos.x;
    tmp2->y = tmp->pos.y;
    tmp2->part = tmp;
    key->point = tmp2;
    posy = (int)tmp->pos.y / (data->winy / HASHMAP_DIV);
    posx = (int)tmp->pos.x / (data->winx / HASHMAP_DIV);
    if (posy >= HASHMAP_DIV)
      posy = HASHMAP_DIV - 1;
    if (posy < 0)
      posy = 0;
    if (posx >= HASHMAP_DIV)
      posx = HASHMAP_DIV - 1;
    if (posx < 0)
      posx = 0;
    key->next = hashmap[posy][posx].hashkey;
    hashmap[posy][posx].hashkey = key;
    tmp = tmp->next;
  }
}

void  free_hashkey(t_hashkey *hashkey)
{
  t_hashkey *tmp;

  while (hashkey != NULL)
  {
    tmp = hashkey;
    hashkey = hashkey->next;
    if (tmp->point != NULL)
      free(tmp->point);
    if (tmp != NULL)
      free(tmp);
  }
}

void  free_hashmap(t_hashmap **hashmap)
{
  int i;
  int j;

  i = -1;
  while (++i < HASHMAP_DIV)
  {
    j = -1;
    while (++j < HASHMAP_DIV)
      if (hashmap[i][j].hashkey != NULL)
        free_hashkey(hashmap[i][j].hashkey);
    free(hashmap[i]);
  }
  free(hashmap);
}

void  display_hashmap(t_hashmap **hashmap, t_data *data)
{
  int i;
  int j;

  i = -1;
  while (++i < HASHMAP_DIV)
  {
    j = -1;
    while (++j < HASHMAP_DIV)
    {
      draw_rectangle(hashmap[i][j].hashquad, data, 0xDDDD00);
    }
  }
}
