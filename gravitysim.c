/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gravitysim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <gecarval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 12:18:49 by gecarval          #+#    #+#             */
/*   Updated: 2024/11/08 08:43:09 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

float_t	mapfloat_t(float_t value, float_t inmin, float_t inmax, float_t outmin,
		float_t outmax)
{
	return ((value - inmin) * (outmax - outmin) / (inmax - inmin) + outmin);
}

int	create_rgb_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

int	set_redshift(t_particle *tmp, t_data *data)
{
	int		redintensity;
	float_t	speedmag;

	speedmag = vector_magsq(tmp->vel);
	redintensity = (int)mapfloat_t(speedmag, 0.0f, data->num_of_particles / 500,
			255.0f, 0.0f);
	if (redintensity > 255)
		redintensity = 255;
	if (redintensity < 0)
		redintensity = 0;
	return (create_rgb_color(255, redintensity, redintensity));
}

void	render_particle(t_data *data)
{
	int			i;
	int			j;
	t_particle	*tmp;

	i = -1;
	tmp = data->gsim->part;
	render_background(data, 0x000000);
	while (++i < data->num_of_particles)
	{
		tmp->color = set_redshift(tmp, data);
		if (tmp->r == 1)
		{
			pixel_to_img((int)tmp->pos.x, (int)tmp->pos.y, data, tmp->color);
		}
		else
		{
			j = tmp->r;
			while (--j >= 0)
				circlebres((int)tmp->pos.x, (int)tmp->pos.y, j, data,
					tmp->color);
		}
		tmp->color = WHITE;
		tmp = tmp->next;
	}
}

void	applyforce(t_particle *mover, t_vector force)
{
	t_vector	f;

	f = vectordiv(force, mover->mass);
	mover->acel = vectoradd(mover->acel, f);
}

void	attraction(t_particle *mover, t_particle *other)
{
	t_vector	force;
	float_t		strength;
	float_t		dist;
	float_t		g;

	g = 0.001;
	force = (t_vector){0, 0};
	force = vectorsub(mover->pos, other->pos);
	dist = constrain_float_t(vector_magsq(force), 100, 1000);
	strength = (g * other->mass) / dist;
	force = vector_setmagmult(force, -strength);
	pthread_mutex_lock(&mover->mutex);
	applyforce(mover, force);
	pthread_mutex_unlock(&mover->mutex);
}

void	collision(t_particle *p1, t_particle *p2, float_t d)
{
	t_vector	impactvector;
	t_vector	deltava;
	t_vector	deltavb;
	t_vector	vdiff;
	t_vector	dir;
	float_t		overlap;
	float_t		msum;
	float_t		num;
	float_t		den;

	if (d < p1->r + p2->r)
	{
		impactvector = vectorsub(p2->pos, p1->pos);
		overlap = d - (p1->r + p2->r);
		dir = impactvector;
		dir = vector_setmagmult(dir, overlap * 0.5);
		pthread_mutex_lock(&p1->mutex);
		p1->pos = vectoradd(p1->pos, dir);
		pthread_mutex_unlock(&p1->mutex);
		dir = vectormult(dir, -1);
		pthread_mutex_lock(&p2->mutex);
		p2->pos = vectoradd(p2->pos, dir);
		pthread_mutex_unlock(&p2->mutex);
		d = p1->r + p2->r;
		impactvector = vector_setmagmult(impactvector, d);
		msum = p1->mass + p2->mass;
		vdiff = vectorsub(p2->vel, p1->vel);
		num = vdiff.x * impactvector.x + vdiff.y * impactvector.y;
		den = msum * d * d;
		deltava = impactvector;
		deltava = vectormult(deltava, 1 * p2->mass * num / den);
		pthread_mutex_lock(&p1->mutex);
		p1->vel = vectoradd(p1->vel, deltava);
		pthread_mutex_unlock(&p1->mutex);
		deltavb = impactvector;
		deltavb = vectormult(deltavb, -1 * p1->mass * num / den);
		pthread_mutex_lock(&p2->mutex);
		p2->vel = vectoradd(p2->vel, deltavb);
		pthread_mutex_unlock(&p2->mutex);
	}
}

void	limit_velocity(t_particle *mover, float_t limit)
{
	if (mover->vel.x > limit)
		mover->vel.x = limit;
	if (mover->vel.y > limit)
		mover->vel.y = limit;
	if (mover->vel.x < -limit)
		mover->vel.x = -limit;
	if (mover->vel.y < -limit)
		mover->vel.y = -limit;
}

void	update_position(t_particle *mover)
{
	mover->vel = vectoradd(mover->vel, mover->acel);
	limit_velocity(mover, 3000);
	mover->pos = vectoradd(mover->pos, mover->vel);
	mover->acel = create_vector(0, 0);
}

void	process_velocity(t_data *data)
{
	int			i;
	t_particle	*tmp;

	i = -1;
	tmp = (data->gsim->part);
	while (++i < data->num_of_particles)
	{
		tmp->prev_pos = tmp->pos;
		update_position(tmp);
		if (tmp->pos.x > data->winx - tmp->r || tmp->pos.x < 0 + tmp->r)
		{
			tmp->vel.x = tmp->vel.x * -1;
			if (tmp->pos.x > data->winx - tmp->r)
				tmp->pos.x = data->winx - tmp->r - 1;
			if (tmp->pos.x < 0 + tmp->r)
				tmp->pos.x = 0 + tmp->r + 1;
		}
		if (tmp->pos.y > data->winy - tmp->r || tmp->pos.y < 0 + tmp->r)
		{
			tmp->vel.y = tmp->vel.y * -1;
			if (tmp->pos.y > data->winy - tmp->r)
				tmp->pos.y = data->winy - tmp->r - 1;
			if (tmp->pos.y < 0 + tmp->r)
				tmp->pos.y = 0 + tmp->r + 1;
		}
		tmp = tmp->next;
	}
}

t_vector	getmidpoint_onquad(t_quadtree *qt)
{
	int	x;
	int	y;
	int	i;

	x = 0;
	y = 0;
	i = -1;
	while (++i < qt->point_count)
		x += qt->points[i].part->pos.x;
	i = -1;
	while (++i < qt->point_count)
		y += qt->points[i].part->pos.y;
	if (qt->point_count != 0)
	{
		x /= qt->point_count;
		y /= qt->point_count;
	}
	return (create_vector(x, y));
}

void	apply_attraction_onquad(t_particle *m, t_quadtree *qtree, t_data *data)
{
	t_particle	*p;
	t_particle	temp;
	t_vector	qdist;
	float_t		d;
	int			i;

	if (qtree->divided && qtree->points == NULL)
	{
		apply_attraction_onquad(m, qtree->northeast, data);
		apply_attraction_onquad(m, qtree->northwest, data);
		apply_attraction_onquad(m, qtree->southeast, data);
		apply_attraction_onquad(m, qtree->southwest, data);
		return ;
	}
	qdist = create_vector(qtree->boundary.x, qtree->boundary.y);
	qdist = vectorsub(m->pos, qdist);
	d = vector_magsqsqrt(qdist);
	if (d < 1)
	{
		i = -1;
		while (++i < qtree->point_count)
		{
			p = qtree->points[i].part;
			if (m != p)
				attraction(m, p);
		}
	}
	else
	{
		temp.pos = getmidpoint_onquad(qtree);
		temp.mass = m->mass * qtree->point_count;
		attraction(m, &temp);
	}
}

void	apply_collision_onquad(t_quadtree *qt, t_data *data)
{
	int			i;
	int			j;
	t_vector	dist;
	t_particle	*tmp;
	t_particle	*tmp2;

	if (qt->divided)
	{
		apply_collision_onquad(qt->northwest, data);
		apply_collision_onquad(qt->northeast, data);
		apply_collision_onquad(qt->southwest, data);
		apply_collision_onquad(qt->southeast, data);
		return ;
	}
	i = -1;
	while (++i < qt->point_count)
	{
		j = i - 1;
		while (++j < qt->point_count)
		{
			tmp = (qt->points[i]).part;
			tmp2 = (qt->points[j]).part;
			if (tmp != tmp2)
			{
				pthread_mutex_lock(&tmp->mutex);
				pthread_mutex_lock(&tmp2->mutex);
				dist = vectorsub(tmp2->pos, tmp->pos);
				pthread_mutex_unlock(&tmp->mutex);
				pthread_mutex_unlock(&tmp2->mutex);
				collision(tmp, tmp2, vector_magsqsqrt(dist));
			}
		}
	}
}

void	*partition_process(void *ptr)
{
	t_processor	*processors;
	t_particle	*tmp;
	int			i;

	i = 0;
	processors = (t_processor *)ptr;
	tmp = processors->data->gsim->part;
	while (i < processors->start)
	{
		tmp = tmp->next;
		i++;
	}
	while (i < processors->end)
	{
		apply_attraction_onquad(tmp, processors->data->qt, processors->data);
		tmp = tmp->next;
		i++;
	}
	//	printf("Thread %d finished\n", (processors->start
	//			/ (processors->data->num_of_particles / MAX_THREADS)) + 1);
	return (NULL);
}

void	*partition_collision(void *ptr)
{
	t_processor	*processors;

	processors = (t_processor *)ptr;
	apply_collision_onquad(processors->qt, processors->data);
	//	ft_putstr_fd("Collision applied\n", 1);
	return (NULL);
}

void	process_physics_quad(t_data *data)
{
	int			i;
	int			k;
	t_point		pt;
	t_particle	*tmp;
	t_quadtree	*qt;

	i = -1;
	tmp = data->gsim->part;
	data->qt = create_quadtree_fromglobals(data->winx, data->winy);
	// printf("Building quadtree\n");
	while (++i < data->num_of_particles)
	{
		pt = create_point(tmp->pos.x, tmp->pos.y, tmp);
		insert_point(data->qt, pt);
		tmp = tmp->next;
	}
	// printf("Quadtree built\n");
	// printf("Applying forces\n");
	i = 0;
	while (i < MAX_THREADS)
	{
		data->processors[i].data = data;
		data->processors[i].start = i * data->num_of_particles / MAX_THREADS;
		data->processors[i].end = (i + 1) * data->num_of_particles
			/ MAX_THREADS;
		if (pthread_create(&data->processors[i].processor, NULL,
				partition_process, &data->processors[i]) != 0)
		{
			free_quadtree(data->qt);
			display_error(data, "error on thread create\n");
		}
		i++;
	}
	if (data->show_tree == 1)
		display_quadtree_boundaries(data->qt, data, 0xDD00DD);
	k = -1;
	while (++k < COLLISION_STEPS)
	{
		i = -1;
		tmp = data->gsim->part;
		qt = create_quadtree_fromvalues(data->winx / 2, data->winy / 2,
				data->winx, data->winy, 3);
		// printf("Applying collision for the %d time\n", k + 1);
		// printf("Building quadtree number %d\n", k + 1);
		while (++i < data->num_of_particles)
		{
			pthread_mutex_lock(&tmp->mutex);
			pt = create_point(tmp->pos.x, tmp->pos.y, tmp);
			pthread_mutex_unlock(&tmp->mutex);
			insert_point(qt, pt);
			tmp = tmp->next;
		}
		// printf("Quadtree number %d built\n", k + 1);
		if (data->show_collision_tree == 1)
			display_quadtree_boundaries(qt, data, 0xDDDD00);
		data->col_processors[k].data = data;
		data->col_processors[k].qt = qt;
		if (pthread_create(&data->col_processors[k].processor, NULL,
				partition_collision, &data->col_processors[k]) != 0)
			display_error(data, "error on thread create\n");
	}
	k = -1;
	while (++k < COLLISION_STEPS)
	{
		// printf("Joining thread %d\n", k + 1);
		if (pthread_join(data->col_processors[k].processor, NULL) != 0)
			display_error(data, "error on thread join\n");
		free_quadtree(data->col_processors[k].qt);
	}
	i = -1;
	while (++i < MAX_THREADS)
	{
		if (pthread_join(data->processors[i].processor, NULL) != 0)
		{
			free_quadtree(data->qt);
			display_error(data, "error on thread join\n");
		}
	}
	free_quadtree(data->qt);
	// printf("all forces applied\n");
}

void	part_sim(t_data *data)
{
	process_velocity(data);
	render_particle(data);
	process_physics_quad(data);
}
