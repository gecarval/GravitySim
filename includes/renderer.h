/*                                                        :::      ::::::::   */
/*   renderer.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 17:05:06 by gecarval          #+#    #+#             */
/*   Updated: 2024/09/02 14:34:58 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERER_H
# define RENDERER_H

// LIBRARIES
# include "../libft/libft.h"
# include "../minilibx-linux/mlx.h"
# include <X11/X.h>
# include <X11/keysym.h>
# include <aio.h>
# include <fcntl.h>
# include <math.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <unistd.h>

// MACROS
# define MAX_THREADS 20

// WINDOW MACROS
# define ESC 65307
# define WINX 1200
# define WINY 800

# define HASHMAP_DIV 200

# define COLLISION_STEPS 6
# define WHITE 0xDDDDFF

// QUAD TREE
# define DEFAULT_CAPACITY 512
# define MAX_DEPTH 1024

// STRUCTS
typedef struct s_data		t_data;
typedef struct s_quadtree	t_quadtree;
typedef struct s_hashkey t_hashkey;
typedef struct s_hashmap t_hashmap;

typedef struct s_processor
{
	pthread_t				processor;
  t_hashmap       **hashmap;
	t_quadtree				*qt;
	t_data					*data;
	int						start;
	int						end;
}							t_processor;

typedef struct s_objinf
{
	float_t					pox;
	float_t					poy;
	float_t					sx;
	float_t					sy;
	float_t					color;
}							t_objinf;

typedef struct s_delta
{
	float_t					ini;
	float_t					fin;
}							t_delta;

typedef struct s_vector
{
	float_t					x;
	float_t					y;
}							t_vector;

typedef struct s_circle
{
	float_t					x;
	float_t					y;
	float_t					r;
	float_t					rsqrt;
}							t_circle;

typedef struct s_rectangle
{
	float_t					x;
	float_t					y;
	float_t					w;
	float_t					h;
	float_t					left;
	float_t					right;
	float_t					top;
	float_t					bottom;
}							t_rectangle;

typedef struct s_particle
{
	pthread_mutex_t			mutex;
	t_vector				prev_pos;
	t_vector				pos;
	t_vector				vel;
	t_vector				acel;
	int						id;
	int						color;
	int						mass;
	int						r;
	struct s_particle		*next;
}							t_particle;

typedef struct s_point
{
	float_t					x;
	float_t					y;
	t_particle				*part;
}							t_point;

typedef struct s_quadtree
{
	t_rectangle				boundary;
	t_point					*points;
	int						point_count;
	int						capacity;
	int						depth;
	bool					divided;
	struct s_quadtree		*northeast;
	struct s_quadtree		*northwest;
	struct s_quadtree		*southeast;
	struct s_quadtree		*southwest;
}							t_quadtree;

typedef struct s_gravsim
{
	t_particle				*part;
}							t_gravsim;

typedef struct s_hashkey
{
  t_point *point;
  struct s_hashkey *next;
}							t_hashkey;

typedef struct s_hashmap
{
  t_rectangle   hashquad;
  t_hashkey     *hashkey;
} t_hashmap;

typedef struct s_img
{
	void					*img_ptr;
	char					*img_px;
	int						end;
	int						llen;
	int						bpp;
}							t_img;

typedef struct s_button
{
	t_vector				pos;
	char					*text;
	int						txt_col;
	int						btt_col;
	int						outln_color;
}							t_button;

typedef struct s_textbox
{
	t_vector				pos;
	char					*text;
	int						txt_col;
	int						outln_color;
}							t_textbox;

typedef struct s_menu
{
	t_vector				pos;
	t_button				*button;
	t_textbox				*text_box;
	int						num_of_particlesicle;
	int						part_num;
	int						radius;
	int						bg_col;
	int						outln_col;
}							t_menu;

typedef struct s_data
{
	void					*ini;
	void					*win;
	t_img					*img;
	t_menu					*menu;
  t_hashmap       **hashmap;
	t_gravsim				*gsim;
	t_quadtree				*qt;
	t_processor				*processors;
	t_processor				*col_processors;
	int						num_of_particles;
	int						part_num;
	int						radius;
	int						winx;
	int						winy;
	int						anigsim;
	int						click_hold;
	int						show_tree;
	int						show_collision_tree;
	int						pmposx;
	int						pmposy;
	int						mposx;
	int						mposy;
	int						timing;
}							t_data;

// PROCESS CALLS
void						display_error(t_data *data, char *msg);
void						create_tensor(t_data *data);
void						ft_init_program(t_data *data);
void						ft_free_matrix(void **vec);
void						ft_free_tensor(void ***z);
int							exit_data(t_data *data, int fd);
// MAP READ
void						create_gsim(t_data *data);
// HOOKS
int							mlx_cooked(int key, t_data *data);
int							mlx_anim(t_data *data);
int							mouse_click(int key, int x, int y, t_data *data);
int							mouse_released(int key, t_data *data);
// PIXEL PUTS
void						pixel_to_img(int x, int y, t_data *data, int color);
void						pixel_to_img_float(float_t x, float_t y,
								t_data *data, int color);
void						render_background(t_data *data, int color);
void						draw_line_y(t_delta x, t_delta y, t_data *data,
								int color);
void						draw_line_x(t_delta x, t_delta y, t_data *data,
								int color);
void						draw_line(t_delta x, t_delta y, t_data *data,
								int color);
void						drawcircle(int xc, int yc, int x, int y,
								t_data *data, int color);
void						circlebres(int xc, int yc, int r, t_data *data,
								int color);
// WATER MARKS
void						water_mark(t_data *data);
void						controls_mark(t_data *data);
// GRAVITY SIM
void						part_sim(t_data *data);
void						reset_pos(t_data *data);
void						ft_set_params(t_data *data, int part_n, int radius);
// VECTORS
void						limit_velocity(t_particle *mover, float_t limit);
t_vector					create_vector(float_t x, float_t y);
t_vector					vectoradd(t_vector v1, t_vector v2);
t_vector					vectorsub(t_vector v1, t_vector v2);
t_vector					vectordot(t_vector v1, t_vector v2);
t_vector					vectormult(t_vector v, float_t scalar);
t_vector					vectordiv(t_vector v, float_t scalar);
t_vector					vector_setmagmult(t_vector v, float_t magnitude);
t_vector					vector_setmagdiv(t_vector v, float_t magnitude);
t_vector					mirror_forces(t_vector v, t_data *data);
t_vector					limit_vector(t_vector v, t_data *data);
t_vector					reverse_vector(t_vector v);
float_t						vector_magsqsqrt(t_vector v);
float_t						vector_magsq(t_vector v);
float_t						constrain_float_t(float_t val, float_t min,
								float_t max);
// HASHMAP
t_hashmap         **create_hashmap(t_data *data);
void              insert_points_hashmap(t_hashmap **hashmap, t_data *data);
void              free_hashmap(t_hashmap **hashmap);
void              free_hashkey(t_hashkey *hashkey);
void              display_hashmap(t_hashmap **hashmap, t_data *data);
// QUAD TREE
t_point						create_point(float_t x, float_t y,
								t_particle *part);
float_t						sqdistfrompt(t_point *p, t_point *other);
float_t						distfrompt(t_point *p, t_point *other);
void						highlight_point(t_point pt, int size, t_data *data);
t_rectangle					create_rectangle(float_t x, float_t y, float_t w,
								float_t h);
int							quadcontains(t_rectangle *r, t_point *p);
int							quadintersects(t_rectangle *r, t_rectangle *range);
t_rectangle					subdivide_rectangle(t_rectangle *r,
								const char *quadrant);
float_t						xdistfromrec(t_rectangle *rect, t_point *point);
float_t						ydistfromrec(t_rectangle *rect, t_point *point);
float_t						sqdistfromrec(t_rectangle *rect, t_point *point);
float_t						distfromrec(t_rectangle *rect, t_point *point);
t_circle					create_circle(float_t x, float_t y, float_t r);
int							circontains(t_circle *c, t_point *p);
int							cirintersects(t_circle *c, t_rectangle *range);
t_quadtree					*create_quadtree(t_rectangle boundary,
								unsigned int capacity, unsigned int depth);
t_quadtree					*create_quadtree_fromglobals(float_t width,
								float_t height);
t_quadtree					*create_quadtree_fromrectangle(t_rectangle rect,
								int capacity);
t_quadtree					*create_quadtree_fromvalues(float_t x, float_t y,
								float_t w, float_t h, int capacity);
t_quadtree					**get_all_children(t_quadtree *qt);
t_quadtree					*get_one_children(t_quadtree *qt,
								const char *quadrant);
void						subdivide_tree(t_quadtree *qt);
bool						insert_point(t_quadtree *qt, t_point p);
void						query_quadtree(t_quadtree *qt, t_rectangle *range,
								t_point *found[], int *found_count);
void						query_quadtree_circle(t_quadtree *qt,
								t_circle *range, t_point *found[],
								int *found_count);
int							report_query(t_quadtree *qt, t_data *data,
								t_rectangle range);
int							report_query_circle(t_quadtree *qt, t_data *data,
								t_circle range);
void						display_quadtree_boundaries(t_quadtree *qt,
								t_data *data, int color);
void						draw_rectangle(t_rectangle r, t_data *data,
								int color);
void						print_quadtree(t_quadtree *qt);
void						free_quadtree(t_quadtree *qt);
// DELTAS
void						defdel(t_delta *a, float_t ini, float_t fin);
t_delta						revdel(t_delta a);
// MATH
float_t						rand_float_t(float_t min, float_t max);

#endif
